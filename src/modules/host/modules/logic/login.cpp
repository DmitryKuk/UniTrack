// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/modules/logic/login.h>

#include <tuple>

#include <server/worker.h>
#include <host/module.h>
#include <logic/exceptions.h>

using namespace std::literals;


namespace {


host::module<host::logic::login> module{
	"logic/login"s,
	[](const nlohmann::json &host_config, ::logic::global_instance &logic)
	{
		return std::make_shared<host::logic::login>(host_config, logic);
	}
};


};	// namespace



host::logic::login::login(const nlohmann::json &config, ::logic::global_instance &logic):
	base_logic{logic},
	base_host{config}
{}


// virtual
std::unique_ptr<server::protocol::http::response>
host::logic::login::response(const server::worker &worker, server::protocol::http::request &request) const
{
	using namespace server::protocol::http;
	
	
	// This host can process only POST requests
	if (request.method != method::POST)
		return nullptr;	// Soft redirect
	
	// User have session id => don't need login him
	if (request.cookies.count("sid"s)) {
		static const std::string response_body = "{\"status\":\"already_logged_in\"}"s;
		return this->response_with_json_body(worker, request, status::ok, response_body, false);
	}
	
	
	const auto handle_error =
		[&](const auto &log_message, const std::string &response_body)
		{
			this->log_error(worker, request, log_message, status::ok);
			return this->response_with_json_body(worker, request, status::ok, response_body, false);
		};
	
	
	static const std::string bad_request_body = "{\"status\":\"bad_request\"}"s;
	
	
	// Login users by POST request only
	std::string data{request.body.data(), request.body.size()};
	if (data.size() != request.body.size())
		return handle_error("Non-string-convertible data in login request body"s, bad_request_body);
	
	
	// Parsing login form
	::logic::login::form form;
	bool parsed = server::protocol::http::decode_uri_args(
		data,
		[&](std::string &&key, std::string &&value) { form.emplace(std::move(key), std::move(value)); },
		[&](const std::string &) {}
	);
	
	if (!parsed)
		return handle_error("Can\'t decode login request body"s, bad_request_body);
	
	
	// Processing login form
	try {
		std::string user_ref, session_cookie;
		std::tie(user_ref, session_cookie) = this->login_user(form);
		
		std::string response_body = "{\"status\":\"ok\",\"ref\":\""s + user_ref + "\"}"s;
		auto response_ptr = this->response_with_json_body(worker, request, status::ok, std::move(response_body));
		response_ptr->add_header(header::set_cookie, session_cookie);
		
		return std::move(response_ptr);
	} catch (const ::logic::password_not_match &e) {
		static const std::string response_body = "{\"status\":\"incorrect_logpass\"}"s;
		return handle_error(e.what(), response_body);
	} catch (const ::logic::user_not_found &e) {
		static const std::string response_body = "{\"status\":\"incorrect_logpass\"}"s;
		return handle_error(e.what(), response_body);
	} catch (const std::exception &e) {
		static const std::string response_body = "{\"status\":\"unknown_error\"}"s;
		return handle_error(e.what(), response_body);
	}
}
