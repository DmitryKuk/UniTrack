// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <string>

#include <server/worker.h>


inline
host::file_handlers::files_and_template_pages::files_and_template_pages(
	logic::global_instance &logic,
	const host::file_handlers::files_and_template_pages::parameters &parameters
):
	template_pages_only{logic},
	
	parameters_{parameters}
{}


template<class FileHost>
std::unique_ptr<server::protocol::http::response>
host::file_handlers::files_and_template_pages::operator()(const FileHost &host,
												 const server::worker &worker,
												 const server::protocol::http::request &request,
												 const boost::filesystem::path &path) const
{
	using behavior = host::file_handlers::files_and_template_pages::parameters::behavior;
	
	
	// Checking type of requested file resource
	bool is_template_page = (this->parameters_.default_behavior == behavior::template_pages);
	
	if (std::regex_match(path.string(), this->parameters_.change_behavior_regex))
		is_template_page = !is_template_page;
	
	
	// Dispatching work to one of handlers
	if (is_template_page)
		return this->host::file_handlers::template_pages_only::operator()(host, worker, request, path);
	else
		return this->server::host::file_handlers::files_only::operator()(host, worker, request, path);
}
