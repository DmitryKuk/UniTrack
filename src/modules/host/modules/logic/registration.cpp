// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/modules/logic/registration.h>

#include <tuple>

#include <server/worker.h>
#include <host/module.h>
#include <logic/exceptions.h>

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
	base_host{config}
{}

#include <iostream>
// virtual
std::unique_ptr<server::protocol::http::response>
host::logic::registration::response(const server::worker &worker,
									server::protocol::http::request &request) const
{
	using namespace server::protocol::http;
	
	static const std::string bad_request_body = "{\"status\":\"bad_request\"}"s;
	
	
	// User have session id => don't need login him
	// This host can process only POST requests
	if (request.method != method::POST || request.cookies.count("sid"s))
		return this->redirect_response(worker, request, "/"s);	// Hard redirect
	
	
	const auto handle_error =
		[&](const auto &log_message, const std::string &response_body)
		{
			this->log_error(worker, request, log_message, status::ok);
			return this->response_with_json_body(worker, request, status::ok, response_body, false);
		};
	
	
	// Register new users by POST request only
	std::string data{request.body.data(), request.body.size()};
	if (data.size() != request.body.size())
		return handle_error("Non-string-convertible data in registration request body"s, bad_request_body);
	
	std::cerr << "Raw form: " << data << std::endl;
	// Parsing registration form
	::logic::registration::form form;
	bool parsed = server::protocol::http::decode_uri_args(
		data,
		[&](std::string &&key, std::string &&value) {
			std::cerr << "Parsed: " << key << ": " << value << std::endl;
			form.emplace(std::move(key), std::move(value));
		},
		[&](const std::string &key) {
			std::cerr << "Parsed: " << key << std::endl;
		}
	);
	
	if (!parsed)
		return handle_error("Can\'t decode registration request body"s, bad_request_body);
	std::cerr << "Form:";
	for (const auto &p: form)
		std::cerr << "  [" << p.first << ": " << p.second << "]";
	std::cerr << std::endl;
	
	// Processing registration form
	try {
		std::string user_ref, session_cookie;
		std::tie(user_ref, session_cookie) = this->register_user(form);
		
		std::string response_body = "{\"status\":\"ok\",\"ref\":\""s + user_ref + "\"}"s;
		auto response_ptr = this->response_with_json_body(worker, request, status::ok, std::move(response_body));
		response_ptr->add_header(header::set_cookie, session_cookie);
		
		return std::move(response_ptr);
	} catch (const ::logic::duplicate_user_found &e) {
		static const std::string response_body = "{\"status\":\"duplicate_user_found\"}"s;
		return handle_error(e.what(), response_body);
	} catch (const std::out_of_range &e) {
		std::cerr << "Out of range caught: " << e.what() << std::endl;
		static const std::string response_body = "{\"status\":\"unknown_error\"}"s;
		return handle_error(e.what(), response_body);
	} catch (const std::exception &e) {
		static const std::string response_body = "{\"status\":\"unknown_error\"}"s;
		return handle_error(e.what(), response_body);
	}
}
