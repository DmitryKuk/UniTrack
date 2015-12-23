// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/modules/file_handlers/template_pages_only.h>

#include <tuple>
#include <utility>

#include <system_/utils.h>
#include <system_/exceptions.h>


// NOTE: This host can NOT be a module! It needs page model generator, so this host should be helper
// for some logic host, which provides page model generator.


// class host::file_handlers::template_pages_only::response
host::file_handlers::template_pages_only::response::response(
	std::shared_ptr<const std::pair<templatizer::page, std::string>> &&tm_pair_ptr,
	logic::page_model &&model,
	const server::worker &worker,
	const server::protocol::http::request &request,
	const server::protocol::http::status &status
):
	server::protocol::http::response{status, request.version},
	
	tm_pair_ptr_{std::move(tm_pair_ptr)},
	page_model_{std::move(model)}
{
	using namespace server::protocol::http;
	
	
	// Adding Server header
	server::host::base::add_server_name(worker, *this);
	
	
	// Generating content
	size_t content_len = this->tm_pair_ptr_->first.generate(this->buffers(), *this, this->page_model_);
	
	
	// Adding Content-Type header
	if (!this->tm_pair_ptr_->second.empty())
		this->add_header(header::content_type, this->tm_pair_ptr_->second);
	
	// Adding Content-Length header
	this->add_header(header::content_length, std::to_string(content_len));
	
	
	if (request.method == method::HEAD)
		this->buffers().resize(1);	// Remove all body buffers
}



// class host::file_handlers::template_pages_only
// static
std::shared_ptr<host::file_handlers::template_pages_only::template_page_and_mime_pair>
host::file_handlers::template_pages_only::load_page(const boost::filesystem::path &path)
{
	std::string mime_type;
	try {
		mime_type = system_::get_file_mime(path.string());
	} catch (const system_::unknown_error &e) {
		mime_type.clear();
	}
	
	
	return std::make_shared<host::file_handlers::template_pages_only::template_page_and_mime_pair>(
		std::piecewise_construct,
		std::forward_as_tuple(path),
		std::forward_as_tuple(mime_type)
	);
}
