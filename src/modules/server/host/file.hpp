// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <boost/filesystem.hpp>

#include <base/json_utils.h>
#include <logger/logger.h>
#include <server/worker.h>
#include <server/host/exceptions.h>


template<class HostType>
server::host::file<HostType>::file(const nlohmann::json &config,
								   HostType &&handler):
	::server::host::base{config},
	
	handler_{std::move(handler)}
{
	this->parse_config(config);
}


template<class HostType>
server::host::file<HostType>::file(const nlohmann::json &config,
								   const HostType &handler):
	::server::host::base{config},
	
	handler_{handler}
{
	this->parse_config(config);
}


template<class HostType>
// virtual
std::unique_ptr<::server::protocol::http::response>
server::host::file<HostType>::response(const worker &worker,
									   ::server::protocol::http::request &request) const
{
	using namespace std::literals;
	using ::server::protocol::http::status;
	
	
	try {
		// Validating request
		this->validate_method(request.method);
		this->validate_path(request.path);
		
		
		static const boost::filesystem::path path_current = "."s;
		
		// boost::filesystem::canonical throws, if path not found
		auto path = boost::filesystem::canonical(path_current / request.path, this->root_);
		if (boost::filesystem::is_directory(path)) {
			if (this->default_index_file_.empty())
				throw ::server::host::path_is_directory{path.string()};
			
			path = boost::filesystem::canonical(this->default_index_file_, path);
			
			if (boost::filesystem::is_directory(path))	// Path is still directory, WTF?..
				throw ::server::host::path_is_directory{path.string()};
		}
		
		
		return this->handler_(*this, worker, request, path);
	}
	
	// Filesystem errors
	catch (const boost::filesystem::filesystem_error &e) {
		switch (e.code().value()) {
			case boost::system::errc::no_such_file_or_directory:
				return	this->handle_error(worker, request, e, status::not_found);
			case boost::system::errc::permission_denied:
				return	this->handle_error(worker, request, e, status::forbidden);
			default:
				return	this->handle_error(worker, request, e, status::internal_server_error);
		}
	}
	
	// Incorrect path
	catch (const ::server::host::path_is_directory &e) {
		return			this->handle_error(worker, request, e, status::forbidden);
	}
	catch (const ::server::host::path_forbidden &e) {
		return			this->handle_error(worker, request, e, status::forbidden);
	}
	catch (const ::server::host::path_not_found &e) {
		return			this->handle_error(worker, request, e, status::not_found);
	}
	
	// Other (handler) errors
	catch (const ::server::host::method_not_allowed &e) {
		return			this->handle_error(worker, request, e, status::forbidden);
	}
	catch (const ::server::host::error &e) {
		return			this->handle_error(worker, request, e, status::internal_server_error);
	}
	
	// Other errors (maybe, file host handler or logger?)
	catch (const std::exception &e) {
		return			this->handle_error(worker, request, e, status::internal_server_error);
	}
	
	// Other errors (impossible)
	catch (...) {
		static const char e[] = "Unknown error";
		return			this->handle_error(worker, request, e, status::internal_server_error);
	}
}


template<class HostType>
// virtual
void
server::host::file<HostType>::clear_cache() noexcept
{
	this->handler_.clear_cache();
	this->::server::host::base::clear_cache();
}


template<class HostType>
inline
void
server::host::file<HostType>::validate_method(::server::protocol::http::method m) const
{
	using ::server::protocol::http::method;
	
	// This host only supports GET and HEAD methods
	if (m != method::GET && m != method::HEAD)
		throw ::server::host::method_not_allowed{::server::protocol::http::method_to_str(m)};
}


template<class HostType>
void
server::host::file<HostType>::validate_path(const std::string &path) const
{
	// Checks for denied regexes
	for (const auto &deny_regex: this->deny_regexes_)
		if (std::regex_match(path, deny_regex))
			throw ::server::host::path_forbidden{path};
	
	
	// Checks for allowed regexes
	bool verification_status = false;
	
	if (this->allow_regexes_.empty()) {
		verification_status = false;
	} else if (this->mode_ == allow_match_mode::any) {	// Any
		verification_status = false;
		for (const auto &allow_regex: this->allow_regexes_)
			if (std::regex_match(path, allow_regex)) {
				verification_status = true;
				break;
			}
	} else {														// All
		verification_status = true;
		for (const auto &allow_regex: this->allow_regexes_)
			if (!std::regex_match(path, allow_regex)) {
				verification_status = false;
				break;
			}
	}
	
	if (!verification_status)
		throw ::server::host::path_forbidden{path};
}


// Config parser
template<class HostType>
void
server::host::file<HostType>::parse_config(const nlohmann::json &config)
{
	using namespace std::literals;
	
	
	this->root_ = ::base::json_utils::get<decltype(this->root_)>(config, "root"s);
	if (!boost::filesystem::exists(this->root_))
		throw ::server::host::path_not_found{this->root_.string()};
	this->root_ = boost::filesystem::canonical(this->root_, "/"s);
	
	
	// Allow mode
	{
		const auto mode = ::base::json_utils::get<std::string>(config, "allow_match_mode"s);
		if (mode == "any"s) {
			this->mode_ = allow_match_mode::any;
		} else if (mode == "all"s) {
			this->mode_ = allow_match_mode::all;
		} else {
			throw ::server::host::incorrect_config{"Incorrect allow_match_mode: \""s + mode
												   + "\", correct values are: \"any\", \"all\""s};
		}
	}
	
	
	// Optional parameters
	{
		static const auto fill_regexes =
			[](auto &regex_vector, const auto &regexes)
			{
				regex_vector.clear();
				for (const std::string &regex: regexes)
					regex_vector.emplace_back(regex);
			};
		
		
		try {
			fill_regexes(this->allow_regexes_, ::base::json_utils::at(config, "allow_regexes"s));
		} catch (const std::out_of_range &) {}
		
		try {
			fill_regexes(this->deny_regexes_, ::base::json_utils::at(config, "deny_regexes"s));
		} catch (const std::out_of_range &) {}
	}
	
	try {
		this->default_index_file_ = ::base::json_utils::get<std::string>(config, "default_index_file"s);
	} catch (const std::out_of_range &) {}
}
