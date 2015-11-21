// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <string>
#include <regex>


inline
host::file::files_and_template_pages::files_and_template_pages(
	logic::global_instance &logic_global_instance,
	const host::file::files_and_template_pages::parameters &parameters
):
	template_pages_only(logic_global_instance),
	
	parameters_(parameters)
{}


template<class FileHost>
server::protocol::http::response::ptr_type
host::file::files_and_template_pages::operator()(const FileHost &host,
												 const server::worker &worker,
												 server::protocol::http::request::ptr_type request_ptr,
												 const boost::filesystem::path &path)
{
	using behavior = host::file::files_and_template_pages::parameters::behavior;
	
	
	// Checking type of requested file resource
	bool is_template_page = (this->parameters_.default_behavior == behavior::template_pages);
	
	if (std::regex_match(path.string(), this->parameters_.change_behavior_regex))
		is_template_page = !is_template_page;
	
	
	// Dispatching work to one of handlers
	if (is_template_page)
		return this->host::file::template_pages_only::operator()(host, worker, std::move(request_ptr), path);
	else
		return this->server::host::file::files_only::operator()(host, worker, std::move(request_ptr), path);
}
