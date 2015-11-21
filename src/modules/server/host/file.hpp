// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <regex>

#include <boost/filesystem.hpp>

#include <base/json_utils.h>
#include <server/host/exceptions.h>
#include <server/host/file_handler_exceptions.h>


// struct server::host::file<HostType>::current_parameters
template<class HostType>
server::host::file<HostType>::current_parameters::current_parameters(const nlohmann::json &config):
	root(base::json_utils::get<decltype(this->root)>(config, "root"))
{
	if (!boost::filesystem::exists(this->root))
		throw server::path_not_found(this->root.string());
	this->root = std::move(boost::filesystem::canonical(this->root, "/"));
	
	
	// Allow mode
	{
		const auto mode = base::json_utils::get<std::string>(config, "allow_match_mode");
		if (mode == "any") {
			this->mode = server::host::file::parameters::allow_match_mode::any;
		} else if (mode == "all") {
			this->mode = server::host::file::parameters::allow_match_mode::all;
		} else {
			throw std::logic_error("Incorrect config");
		}
	}
	
	
	// Optional parameters
	{
		static const auto fill_regexes = [](auto &regex_vector, const auto &regexes)
		{
			regex_vector.clear();
			for (const std::string &regex: regexes)
				regex_vector.emplace_back(regex);
		};
		
		
		try {
			fill_regexes(this->allow_regexes, base::json_utils::at(config, "allow_regexes"));
		} catch (const std::out_of_range &) {}
		
		try {
			fill_regexes(this->deny_regexes, base::json_utils::at(config, "deny_regexes"));
		} catch (const std::out_of_range &) {}
	}
}



// class server::host::file<HostType>
template<class HostType>
server::host::file<HostType>::file(logger::logger &logger,
								   const server::file_host_parameters &parameters,
								   HostType &&handler):
	server::host(logger, parameters),
	
	parameters_(parameters),
	handler_(std::move(handler))
{}


template<class HostType>
server::host::file<HostType>::file(logger::logger &logger,
								   const server::file_host_parameters &parameters,
								   const HostType &handler):
	server::host(logger, parameters),
	
	parameters_(parameters),
	handler_(handler)
{}


template<class HostType>
// virtual
server::protocol::http::response::ptr_type
server::host::file<HostType>::response(const server::worker &worker,
									   server::protocol::http::request::ptr_type request_ptr)
{
	using namespace server::protocol::http::status;
	
	
	try {
		// Validating request
		this->validate_method(request_ptr->method);
		this->validate_path(request_ptr->path);
		
		
		static const boost::filesystem::path path_current = ".";
		
		// boost::filesystem::canonical throws, if path not found
		auto path = boost::filesystem::canonical(path_current / request_ptr->path, this->parameters_.root);
		if (boost::filesystem::is_directory(path)) {
			if (this->parameters_.default_index_file == "")
				throw server::host::path_is_directory(path.string());
			
			path = boost::filesystem::canonical(this->parameters_.default_index_file, path);
			
			if (boost::filesystem::is_directory(path))	// Path is still directory, WTF?..
				throw server::host::path_is_directory(path.string());
		}
		
		
		return this->handler_(*this, worker, request_ptr, path);
	}
	
	// Filesystem errors
	catch (const boost::filesystem::filesystem_error &e) {
		switch (e.code().value()) {
			case boost::system::errc::no_such_file_or_directory:
				return this->handle_error(std::move(request_ptr), e,	not_found);
			case boost::system::errc::permission_denied:
				return this->handle_error(std::move(request_ptr), e,	forbidden);
			default:
				return this->handle_error(std::move(request_ptr), e,	internal_server_error);
		}
	}
	
	// Incorrect path
	catch (const server::host::path_is_directory &e) {
		return this->handle_error(std::move(request_ptr), e,			forbidden);
	}
	catch (const server::host::path_forbidden &e) {
		return this->handle_error(std::move(request_ptr), e,			forbidden);
	}
	catch (const server::host::path_not_found &e) {
		return this->handle_error(std::move(request_ptr), e,			not_found);
	}
	
	// Other (handler) errors
	catch (const server::host::method_not_allowed &e) {
		return this->handle_error(std::move(request_ptr), e,			forbidden);
	}
	catch (const server::host::error &e) {
		return this->handle_error(std::move(request_ptr), e,			internal_server_error);
	}
	
	// Other errors (maybe, file host handler or logger?)
	catch (const std::exception &e) {
		return this->handle_error(std::move(request_ptr), e,			internal_server_error);
	}
	
	// Other errors (impossible)
	catch (...) {
		static const char e[] = "Unknown error";
		return this->handle_error(std::move(request_ptr), e,			internal_server_error);
	}
}


template<class HostType>
inline
void
server::host::file<HostType>::validate_method(server::http::method method) const
{
	using namespace server::protocol::http::method;
	
	// This host only supports GET and HEAD methods
	if (method != GET && method != HEAD)
		throw server::host::method_not_allowed(server::protocol::http::method_to_str(method));
}


template<class HostType>
void
server::host::file<HostType>::validate_path(const std::string &path) const
{
	// Checks for denied regexes
	for (const auto &deny_regex: this->parameters_.deny_regexes)
		if (std::regex_match(path, deny_regex))
			throw server::host::path_forbidden(path);
	
	
	// Checks for allowed regexes
	bool verification_status = false;
	
	if (this->parameters_.allow_regexes.empty()) {
		verification_status = false;
	} else if (this->parameters_.mode == server::host::file::parameters::allow_match_mode::any) {	// Any
		verification_status = false;
		for (const auto &allow_regex: this->parameters_.allow_regexes)
			if (std::regex_match(path, allow_regex)) {
				verification_status = true;
				break;
			}
	} else {																						// All
		verification_status = true;
		for (const auto &allow_regex: this->parameters_.allow_regexes)
			if (!std::regex_match(path, allow_regex)) {
				verification_status = false;
				break;
			}
	}
	
	if (!verification_status)
		throw server::host::path_forbidden(path);
}


template<class HostType>
server::protocol::http::response::ptr_type
server::host::file<HostType>::handle_error(server::protocol::http::request::ptr_type request_ptr,
										   const char *what,
										   const server::http::status &status)
{
	this->logger().stream(logger::level::error)
		<< "File host: \"" << this->name()
		<< "\" (client: " << request_ptr->client_address
		<< "): " << what
		<< " => " << status.code() << '.';
	
	return this->phony_response(std::move(request_ptr), status);
}


template<class HostType>
inline
server::protocol::http::response::ptr_type
server::host::file<HostType>::handle_error(server::protocol::http::request::ptr_type request_ptr,
										   const std::exception &e,
										   const server::http::status &status)
{
	return this->handle_error(std::move(request_ptr), e.what(), status);
}
