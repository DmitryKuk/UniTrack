// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/modules/file_handlers/files_only.h>

#include <string>

#include <server/host/file.h>
#include <host/module.h>

using namespace std::literals;
using namespace std::placeholders;


namespace {


host::module<server::host::file<host::file_handlers::files_only>> module{
	"files_only"s,
	[](const nlohmann::json &host_config, ::logic::global_instance & /* logic */)
	{
		return std::make_shared<server::host::file<host::file_handlers::files_only>>(host_config);
	}
};


};	// namespace
