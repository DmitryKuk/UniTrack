// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/modules/file_handlers/template_pages_only.h>

#include <string>

#include <server/host/file.h>
#include <host/module.h>

using namespace std::literals;
using namespace std::placeholders;


namespace {


host::module<server::host::file<host::file_handlers::template_pages_only>> module{
	"template_pages_only"s,
	[](const nlohmann::json &host_config, ::logic::global_instance &logic)
	{
		return std::make_shared<server::host::file<host::file_handlers::template_pages_only>>(
			host_config,
			host::file_handlers::template_pages_only{logic}
		);
	}
};


};	// namespace



// static
std::shared_ptr<templatizer::page>
host::file_handlers::template_pages_only::load_page(const boost::filesystem::path &path)
{
	return std::make_shared<templatizer::page>(path);
}
