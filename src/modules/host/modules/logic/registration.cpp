// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/modules/logic/registration.h>

#include <server/worker.h>
#include <host/module.h>

using namespace std::literals;


namespace {


host::module<host::logic::registration> module{
	"logic/registration"s,
	[](const nlohmann::json &host_config, ::logic::global_instance &logic)
	{
		return std::make_shared<host::logic::registration>(
			host_config,
			logic
		);
	}
};


};	// namespace



host::logic::registration::registration(const nlohmann::json &config,
										::logic::global_instance &logic):
	::logic::registration{logic},
	file_host{config, {config, *this}}
{}


// virtual
std::unique_ptr<server::protocol::http::response>
host::logic::registration::response(const server::worker &worker,
									server::protocol::http::request &request) const
{
	if (request.method == server::protocol::http::method::POST) {
		this->register_user(std::string{request.body.data(), request.body.size()});
		return this->phony_response(worker, request, server::protocol::http::status::ok);
	}
	
	return this->file_host::response(worker, request);
}


// private
void
host::logic::registration::validate_request_body(const std::vector<char> &body) const
{
	
}
