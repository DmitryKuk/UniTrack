// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <base/buffer.h>
#include <server/host/base.h>

#include <server/host/exceptions.h>


inline
host::file_handlers::template_pages_only::template_pages_only(logic::page_model_generator &logic):
	logic_{logic}
{}


template<class FileHost>
inline
std::unique_ptr<server::protocol::http::response>
host::file_handlers::template_pages_only::operator()(const FileHost &host,
													 const server::worker &worker,
													 const server::protocol::http::request &request,
													 const boost::filesystem::path &path) const
{
	using namespace server::protocol::http;
	
	
	if (request.method != method::GET && request.method != method::HEAD)
		throw server::host::method_not_allowed{method_to_str(request.method)};
	
	
	// Generating page model, loading template page, if need, and creating response
	return std::make_unique<host::file_handlers::template_pages_only::response>(
		this->cache_.at(path),
		this->logic_.page_model(request, path),
		worker,
		request
	);
}
