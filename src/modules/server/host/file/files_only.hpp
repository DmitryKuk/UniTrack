// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <base/mapped_file_exceptions.h>

#include <server/host/base.h>
#include <server/host/exceptions.h>
#include <server/worker.h>


inline
server::host::file::files_only::response::response(::base::mapped_file &&mapped_file,
												   const ::server::protocol::http::status &status,
												   ::server::protocol::http::version version):
	::server::protocol::http::response(status, version),
	
	mapped_file_(std::move(mapped_file))
{}


template<class FileHost>
std::shared_ptr<::server::protocol::http::response>
server::host::file::files_only::operator()(const FileHost &host,
										   const ::server::worker &worker,
										   const ::server::protocol::http::request &request,
										   const boost::filesystem::path &path)
{
	using namespace boost::interprocess;
	
	
	try {
		bool need_body = false;
		
		switch (request.method) {
			case ::server::protocol::http::method::GET:
				need_body = true;
				break;
			case ::server::protocol::http::method::HEAD:
				need_body = false;
				break;
			default:
				throw ::server::host::method_not_allowed(::server::protocol::http::method_to_str(request.method));
		}
		
		
		auto response_ptr = std::make_shared<::server::host::file::files_only::response>(
			::base::mapped_file(path, read_only, MAP_SHARED),
			::server::protocol::http::status::ok,
			request.version
		);
		
		
		::server::host::base::add_server_name(worker, *response_ptr);	// Server name
		
		
		const void *file_content = response_ptr->mapped_file_.data();
		size_t file_size = response_ptr->mapped_file_.size();
		
		auto &content_len_str = response_ptr->cache(std::to_string(file_size));
		
		response_ptr->add_header(::server::protocol::http::header::content_length, content_len_str);
		response_ptr->finish_headers();
		
		if (need_body)
			response_ptr->add_body(file_content, file_size);
		
		
		return response_ptr;
	}
	
	// File mapping error (impossible -- path checked by server::host::file<files_only>::response method)
	catch (const ::base::path_not_found &e) {
		throw ::server::host::path_not_found(path.string());
	}
	
	// Interprocess error
	catch (const interprocess_exception &e) {
		using namespace std::literals;
		
		switch (e.get_error_code()) {
			case not_such_file_or_directory: case not_found_error:	// Impossible too
				throw ::server::host::path_not_found(path.string());
			default:
				throw ::server::host::path_forbidden(path.string(), "file_mapping: "s + e.what());
		}
	}
}
