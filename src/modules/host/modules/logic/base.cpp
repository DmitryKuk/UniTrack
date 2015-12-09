// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/modules/logic/base.h>

#include <server/worker.h>
#include <host/module.h>

using namespace std::literals;
using namespace std::placeholders;


namespace {


host::module<host::logic::base> module{
	"logic_base"s,
	[](const nlohmann::json &host_config, ::logic::global_instance &logic)
	{
		return std::make_shared<host::logic::base>(
			host_config,
			logic
		);
	}
};


};	// namespace



host::logic::base::base(const nlohmann::json &config,
						::logic::global_instance &logic):
	server::host::base{config},
	
	logic_{logic}
{}


// virtual
std::unique_ptr<server::protocol::http::response>
host::logic::base::response(const server::worker &worker,
							server::protocol::http::request &request) const
{
	return this->server::host::base::response(worker, request);
}
