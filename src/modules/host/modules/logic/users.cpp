// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/modules/logic/users.h>

#include <tuple>

#include <server/worker.h>
#include <host/module.h>
#include <host/path_utils.h>
#include <logic/exceptions.h>

using namespace std::literals;


namespace {


host::module<host::logic::users> module{
	"logic/users"s,
	[](const nlohmann::json &host_config, ::logic::global_instance &logic)
	{
		return std::make_shared<host::logic::users>(host_config, logic);
	}
};


};	// namespace



host::logic::users::users(const nlohmann::json &config, ::logic::global_instance &logic):
	base_logic{logic},
	base_host{config}
{}


// virtual
std::unique_ptr<server::protocol::http::response>
host::logic::users::response(const server::worker &worker, server::protocol::http::request &request) const
{
	using namespace server::protocol::http;
	
	
	if ((request.method != method::GET && request.method != method::HEAD)
		|| request.args_set.count("json"s) == 0) {
		if (request.args_set.count("json"s) == 0) {
			return nullptr;	// Soft redirect
		} else {
			static const std::string response_body = "{\"status\":\"bad_request\"}"s;
			return this->response_with_json_body(worker, request, status::ok, response_body, false);
		}
	}
	
	if (request.cookies.count("sid"s) == 0) {
		static const std::string response_body = "{\"status\":\"not_logged_in\"}"s;
		return this->response_with_json_body(worker, request, status::ok, response_body, false);
	}
	
	
	const auto handle_error =
		[&](const auto &log_message, const std::string &response_body)
		{
			this->log_error(worker, request, log_message, status::ok);
			return this->response_with_json_body(worker, request, status::ok, response_body, false);
		};
	
	
	try {
		std::string response_body, session_cookie;
		std::tie(response_body, session_cookie) = this->list_users(request.cookies.at("sid"s));
		
		auto response_ptr = this->response_with_json_body(worker, request, status::ok, std::move(response_body));
		if (!session_cookie.empty())
			response_ptr->add_header(header::set_cookie, session_cookie);
		
		return std::move(response_ptr);
	} catch (const std::exception &e) {
		static const std::string response_body = "{\"status\":\"unknown_error\"}"s;
		return handle_error(e.what(), response_body);
	}
}
