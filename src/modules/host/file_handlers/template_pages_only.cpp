// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/file_handlers/template_pages_only.h>


// static
std::shared_ptr<templatizer::page>
host::file_handlers::template_pages_only::load_page(const boost::filesystem::path &path)
{
	return std::make_shared<templatizer::page>(path);
}
