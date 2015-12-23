// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <base/mapped_file_exceptions.h>

#include <server/host/base.h>
#include <server/host/exceptions.h>
#include <server/worker.h>


inline
server::host::file_handlers::files_only::response::response(
	std::shared_ptr<const std::pair<::base::mapped_file, std::string>> &&fm_pair_ptr,
	const ::server::protocol::http::status &status,
	::server::protocol::http::version version
):
	::server::protocol::http::response{status, version},
	
	fm_pair_ptr_{std::move(fm_pair_ptr)}
{}


template<class FileHost>
std::unique_ptr<::server::protocol::http::response>
server::host::file_handlers::files_only::operator()(const FileHost &host,
													const ::server::worker &worker,
													const ::server::protocol::http::request &request,
													const boost::filesystem::path &path) const
{
	using namespace boost::interprocess;
	
	
	try {
		bool need_body = true;
		
		switch (request.method) {
			case ::server::protocol::http::method::GET:
				need_body = true;
				break;
			case ::server::protocol::http::method::HEAD:
				need_body = false;
				break;
			default:
				throw ::server::host::method_not_allowed{::server::protocol::http::method_to_str(request.method)};
				break;
		}
		
		
		auto response_ptr = std::make_unique<::server::host::file_handlers::files_only::response>(
			this->cache_.at(path),
			::server::protocol::http::status::ok,
			request.version
		);
		
		
		::server::host::base::add_server_name(worker, *response_ptr);	// Server name
		
		// Adding Content-Type header
		if (!response_ptr->fm_pair_ptr_->second.empty())
			response_ptr->add_header(::server::protocol::http::header::content_type,
									 response_ptr->fm_pair_ptr_->second);
		
		
		const void *file_content = response_ptr->fm_pair_ptr_->first.data();
		size_t file_size = response_ptr->fm_pair_ptr_->first.size();
		
		response_ptr->add_header(::server::protocol::http::header::content_length, std::to_string(file_size));
		
		if (need_body)
			response_ptr->add_body(::base::buffer(file_content, file_size));
		
		
		return std::move(response_ptr);
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
