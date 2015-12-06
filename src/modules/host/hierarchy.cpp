// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/hierarchy.h>

#include <base/json_utils.h>
#include <server/worker.h>


host::hierarchy::hierarchy(const nlohmann::json &config,
						   ::logic::global_instance &logic):
	server::host::base{config},
	
	logic_{logic}
{}


// virtual
std::unique_ptr<server::protocol::http::response>
host::hierarchy::response(const server::worker &worker,
						  const server::protocol::http::request &request) const
{
	return this->server::host::base::response(worker, request);
}
