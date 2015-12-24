// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/host/file_handlers/files_only.h>

#include <tuple>
#include <utility>

#include <system_/utils.h>
#include <system_/exceptions.h>


// class host::file_handlers::files_only::response
server::host::file_handlers::files_only::response::response(
	std::shared_ptr<const std::pair<::base::mapped_file, std::string>> &&fm_pair_ptr,
	const ::server::worker &worker,
	const ::server::protocol::http::request &request,
	const ::server::protocol::http::status &status
):
	::server::protocol::http::response{status, request.version},
	
	fm_pair_ptr_{std::move(fm_pair_ptr)}
{
	using namespace ::server::protocol::http;
	
	
	// Adding Server header
	::server::host::base::add_server_name(worker, *this);
	
	
	// Adding Content-Type header
	if (!this->fm_pair_ptr_->second.empty())
		this->add_header(header::content_type, this->fm_pair_ptr_->second);
	
	
	const void *file_content = this->fm_pair_ptr_->first.data();
	size_t file_size = this->fm_pair_ptr_->first.size();
	
	// Adding Content-Length header
	this->add_header(header::content_length, std::to_string(file_size));
	
	
	if (request.method != method::HEAD)
		this->add_body(::base::buffer(file_content, file_size));
}



// class host::file_handlers::files_only
// static
std::shared_ptr<::server::host::file_handlers::files_only::file_and_mime_pair>
server::host::file_handlers::files_only::load_file(const boost::filesystem::path &path)
{
	std::string mime_type;
	try {
		mime_type = system_::get_file_mime(path.string());
	} catch (const system_::unknown_error &e) {
		mime_type.clear();
	}
	
	
	return std::make_shared<::server::host::file_handlers::files_only::file_and_mime_pair>(
		std::piecewise_construct,
		std::forward_as_tuple(path, boost::interprocess::read_only, MAP_SHARED),
		std::forward_as_tuple(mime_type)
	);
}
