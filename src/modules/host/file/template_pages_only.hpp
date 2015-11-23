// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <base/buffer.h>
#include <server/host/base.h>


// class host::file::template_pages_only::response
inline
host::file::template_pages_only::response::response(const templatizer::page &page,
													logic::page_model &&model,
													const server::protocol::http::status &status,
													server::protocol::http::version version):
	server::protocol::http::response{status, version},
	
	page_{page},
	page_model_{std::move(model)}
{}



// class host::file::template_pages_only
inline
host::file::template_pages_only::template_pages_only(logic::global_instance &logic):
	logic_{logic}
{}


template<class FileHost>
std::shared_ptr<server::protocol::http::response>
host::file::template_pages_only::operator()(const FileHost &host,
											const server::worker &worker,
											const server::protocol::http::request &request,
											const boost::filesystem::path &path)
{
	using namespace std::literals;
	
	
	// Generating page model, loading template page, if need, and creating response
	auto response_ptr = std::make_shared<host::file::template_pages_only::response>(
		this->pages_cache_.at(path),
		this->logic_.generate(request, path),
		server::protocol::http::status::ok,
		request.version
	);
	
	
	// Adding headers
	server::host::base::add_server_name(worker, *response_ptr);						// Server name
	
	response_ptr->add_header(server::protocol::http::header::content_length, ""s);	// Content-Length
	size_t content_len_index = response_ptr->header_value_index();
	
	response_ptr->finish_headers();
	
	
	// Generating content
	size_t content_len = response_ptr->page_.generate(response_ptr->buffers,
													  *response_ptr,
													  response_ptr->page_model_);
	
	
	// Fix Content-Length header
	const auto &content_len_str = response_ptr->cache(std::to_string(content_len));
	response_ptr->buffers[content_len_index] = ::base::buffer(content_len_str);
	
	
	return response_ptr;
}
