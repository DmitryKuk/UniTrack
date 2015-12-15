// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/modules/logic/registration.h>

#include <tuple>

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
	file_host{config}
{}


// virtual
std::unique_ptr<server::protocol::http::response>
host::logic::registration::response(const server::worker &worker,
									server::protocol::http::request &request) const
{
	if (request.method != server::protocol::http::method::POST)
		return this->file_host::response(worker, request);
	
	
	// Register new users by POST request only
	std::string data{request.body.data(), request.body.size()};
	if (data.size() != request.body.size())
		return this->handle_error(worker,
								  request,
								  "Non-string-convertible data in registration request body"s,
								  server::protocol::http::status::bad_request);
	
	
	::logic::registration::form form;
	
	bool parsed = server::protocol::http::decode_uri_args(
		data,
		[&](std::string &&key, std::string &&value) { form.emplace(std::move(key), std::move(value)); },
		[&](const std::string &) {}
	);
	
	if (!parsed)
		return this->handle_error(worker,
								  request,
								  "Can\'t decode registration request body"s,
								  server::protocol::http::status::bad_request);
	
	try {
		std::string user_ref, session_id;
		std::tie(user_ref, session_id) = this->register_user(form);
		
		auto response_ptr = this->redirect_response(worker, request, "/user/"s + user_ref);
		response_ptr->add_header(server::protocol::http::header::set_cookie, "sid="s + session_id);
		
		return response_ptr;
	} catch (const std::exception &e) {
		return this->handle_error(worker, request, e.what(), server::protocol::http::status::bad_request);
	}
}
