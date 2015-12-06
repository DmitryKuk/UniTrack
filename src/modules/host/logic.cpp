// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/logic.h>

#include <server/worker.h>


host::logic::logic(const nlohmann::json &config,
				   ::logic::global_instance &logic):
	server::host::base{config},
	
	logic_{logic}
{}


// virtual
std::unique_ptr<server::protocol::http::response>
host::logic::response(const server::worker &worker,
					  const server::protocol::http::request &request) const
{
	return this->server::host::base::response(worker, request);
}
