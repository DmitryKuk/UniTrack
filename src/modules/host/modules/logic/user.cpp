// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/modules/logic/user.h>

#include <server/worker.h>
#include <host/module.h>

using namespace std::literals;
using namespace std::placeholders;


namespace {


host::module<host::logic::user> module{
	"logic/user"s,
	[](const nlohmann::json &host_config, ::logic::global_instance &logic)
	{
		return std::make_shared<host::logic::user>(
			host_config,
			logic
		);
	}
};


};	// namespace



host::logic::user::user(const nlohmann::json &config,
						::logic::global_instance &logic):
	::logic::user{logic},
	server::host::file<host::file_handlers::files_and_template_pages>{
		config,
		{config, *this}
	}
{}


// virtual
std::unique_ptr<server::protocol::http::response>
host::logic::user::response(const server::worker &worker,
							server::protocol::http::request &request) const
{
	return this->server::host::base::response(worker, request);
}
