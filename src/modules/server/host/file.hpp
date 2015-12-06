// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <boost/filesystem.hpp>

#include <logger/logger.h>
#include <server/worker.h>
#include <server/host/exceptions.h>


// class server::host::file<HostType>
template<class HostType>
server::host::file<HostType>::file(const ::server::host::file_parameters &parameters,
								   HostType &&handler):
	::server::host::base{parameters},
	
	parameters_{parameters},
	handler_{std::move(handler)}
{}


template<class HostType>
server::host::file<HostType>::file(const ::server::host::file_parameters &parameters,
								   const HostType &handler):
	::server::host::base{parameters},
	
	parameters_{parameters},
	handler_{handler}
{}


template<class HostType>
// virtual
std::unique_ptr<::server::protocol::http::response>
server::host::file<HostType>::response(const worker &worker,
									   const ::server::protocol::http::request &request) const
{
	using namespace std::literals;
	using ::server::protocol::http::status;
	
	
	try {
		// Validating request
		this->validate_method(request.method);
		this->validate_path(request.path);
		
		
		static const boost::filesystem::path path_current = "."s;
		
		// boost::filesystem::canonical throws, if path not found
		auto path = boost::filesystem::canonical(path_current / request.path, this->parameters_.root);
		if (boost::filesystem::is_directory(path)) {
			if (this->parameters_.default_index_file.empty())
				throw ::server::host::path_is_directory{path.string()};
			
			path = boost::filesystem::canonical(this->parameters_.default_index_file, path);
			
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
inline
void
server::host::file<HostType>::validate_method(::server::protocol::http::method method) const
{
	using ::server::protocol::http::method::GET;
	using ::server::protocol::http::method::HEAD;
	
	// This host only supports GET and HEAD methods
	if (method != GET && method != HEAD)
		throw ::server::host::method_not_allowed{::server::protocol::http::method_to_str(method)};
}


template<class HostType>
void
server::host::file<HostType>::validate_path(const std::string &path) const
{
	// Checks for denied regexes
	for (const auto &deny_regex: this->parameters_.deny_regexes)
		if (std::regex_match(path, deny_regex))
			throw ::server::host::path_forbidden{path};
	
	
	// Checks for allowed regexes
	bool verification_status = false;
	
	if (this->parameters_.allow_regexes.empty()) {
		verification_status = false;
	} else if (this->parameters_.mode == file_parameters::allow_match_mode::any) {	// Any
		verification_status = false;
		for (const auto &allow_regex: this->parameters_.allow_regexes)
			if (std::regex_match(path, allow_regex)) {
				verification_status = true;
				break;
			}
	} else {																		// All
		verification_status = true;
		for (const auto &allow_regex: this->parameters_.allow_regexes)
			if (!std::regex_match(path, allow_regex)) {
				verification_status = false;
				break;
			}
	}
	
	if (!verification_status)
		throw ::server::host::path_forbidden{path};
}


template<class HostType>
std::unique_ptr<::server::protocol::http::response>
server::host::file<HostType>::handle_error(const worker &worker,
										   const ::server::protocol::http::request &request,
										   const char *what,
										   const ::server::protocol::http::status &status) const
{
	using namespace std::literals;
	logger::stream(logger::level::error)
		<< "File host: \""s << this->name()
		<< "\" (client: "s << request.client_address
		<< "): "s << what
		<< " => "s << status.code() << '.';
	
	return this->phony_response(worker, request, status);
}


template<class HostType>
inline
std::unique_ptr<::server::protocol::http::response>
server::host::file<HostType>::handle_error(const worker &worker,
										   const ::server::protocol::http::request &request,
										   const std::exception &e,
										   const ::server::protocol::http::status &status) const
{
	return this->handle_error(worker, request, e.what(), status);
}
