// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/modules/phony.h>

#include <functional>

#include <host/module.h>

using namespace std::literals;
using namespace std::placeholders;


namespace {


host::module<host::phony> module{
	"phony"s,
	[](const nlohmann::json &host_config, ::logic::global_instance & /* logic */)
	{
		return std::make_shared<host::phony>(host_config);
	}
};


};	// namespace
