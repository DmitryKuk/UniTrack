// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/modules/file_handlers/template_pages_only.h>


// NOTE: This host can NOT be a module! It needs page model generator, so this host should be helper
// for some logic host, which provides page model generator.


// static
std::shared_ptr<templatizer::page>
host::file_handlers::template_pages_only::load_page(const boost::filesystem::path &path)
{
	return std::make_shared<templatizer::page>(path);
}
