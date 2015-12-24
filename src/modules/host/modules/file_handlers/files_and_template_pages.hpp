// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <string>

#include <server/worker.h>


template<class FileHost>
std::unique_ptr<server::protocol::http::response>
host::file_handlers::files_and_template_pages::operator()(const FileHost &host,
														  const server::worker &worker,
														  const server::protocol::http::request &request,
														  const boost::filesystem::path &path) const
{
	// Checking type of requested file resource
	bool is_template_page = (this->default_behavior_ == behavior::template_pages);
	
	if (std::regex_match(path.string(), this->change_behavior_regex_))
		is_template_page = !is_template_page;
	
	
	// Dispatching work to one of handlers
	if (is_template_page)
		return this->host::file_handlers::template_pages_only::operator()(host, worker, request, path);
	else
		return this->server::host::file_handlers::files_only::operator()(host, worker, request, path);
}


inline
void
host::file_handlers::files_and_template_pages::clear_cache() noexcept
{
	this->server::host::file_handlers::files_only::clear_cache();
	this->host::file_handlers::template_pages_only::clear_cache();
}
