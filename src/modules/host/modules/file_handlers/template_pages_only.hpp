// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <base/buffer.h>
#include <server/host/base.h>


// class host::file_handlers::template_pages_only::response
inline
host::file_handlers::template_pages_only::response::response(
	std::shared_ptr<const templatizer::page> &&page_ptr,
	logic::page_model &&model,
	const server::protocol::http::status &status,
	server::protocol::http::version version
):
	server::protocol::http::response{status, version},
	
	page_ptr_{std::move(page_ptr)},
	page_model_{std::move(model)}
{}



// class host::file_handlers::template_pages_only
inline
host::file_handlers::template_pages_only::template_pages_only(logic::page_model_generator &logic):
	logic_{logic}
{}


template<class FileHost>
std::unique_ptr<server::protocol::http::response>
host::file_handlers::template_pages_only::operator()(const FileHost &host,
													 const server::worker &worker,
													 const server::protocol::http::request &request,
													 const boost::filesystem::path &path) const
{
	using namespace std::literals;
	
	
	// Generating page model, loading template page, if need, and creating response
	auto response_ptr = std::make_unique<host::file_handlers::template_pages_only::response>(
		this->cache_.at(path),
		this->logic_.page_model(request, path),
		server::protocol::http::status::ok,
		request.version
	);
	
	
	// Server name
	server::host::base::add_server_name(worker, *response_ptr);
	
	
	// Generating content
	size_t content_len = response_ptr->page_ptr_->generate(response_ptr->buffers(),
														   *response_ptr,
														   response_ptr->page_model_);
	
	// Content-Length
	response_ptr->add_header(server::protocol::http::header::content_length, std::to_string(content_len));
	
	
	return std::move(response_ptr);
}