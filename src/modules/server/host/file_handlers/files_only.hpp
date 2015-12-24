// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <base/mapped_file_exceptions.h>
#include <server/host/base.h>
#include <server/host/exceptions.h>
#include <server/worker.h>


template<class FileHost>
std::unique_ptr<::server::protocol::http::response>
server::host::file_handlers::files_only::operator()(const FileHost &host,
													const ::server::worker &worker,
													const ::server::protocol::http::request &request,
													const boost::filesystem::path &path) const
{
	using namespace boost::interprocess;
	using namespace ::server::protocol::http;
	
	
	if (request.method != method::GET && request.method != method::HEAD)
		throw ::server::host::method_not_allowed{method_to_str(request.method)};
	
	
	try {
		return std::make_unique<::server::host::file_handlers::files_only::response>(
			this->cache_.at(path),
			worker,
			request
		);
	}
	
	// File mapping error (impossible -- path checked by server::host::file<files_only>::response method)
	catch (const ::base::path_not_found &e) {
		throw ::server::host::path_not_found{path.string()};
	}
	
	// Interprocess error
	catch (const interprocess_exception &e) {
		using namespace std::literals;
		
		switch (e.get_error_code()) {
			case not_such_file_or_directory: case not_found_error:	// Impossible too
				throw ::server::host::path_not_found{path.string()};
			default:
				throw ::server::host::path_forbidden{path.string(), "file_mapping: "s + e.what()};
		}
	}
}


inline
void
server::host::file_handlers::files_only::clear_cache() noexcept
{
	this->cache_.clear();
}
