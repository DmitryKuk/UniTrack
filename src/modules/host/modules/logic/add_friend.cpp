// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/modules/logic/add_friend.h>

#include <tuple>

#include <server/worker.h>
#include <host/module.h>
#include <logic/exceptions.h>

using namespace std::literals;


namespace {


host::module<host::logic::add_friend> module{
	"logic/add_friend"s,
	[](const nlohmann::json &host_config, ::logic::global_instance &logic)
	{
		return std::make_shared<host::logic::add_friend>(host_config, logic);
	}
};


};	// namespace



host::logic::add_friend::add_friend(const nlohmann::json &config, ::logic::global_instance &logic):
	base_logic{logic},
	base_host{config}
{}


// virtual
std::unique_ptr<server::protocol::http::response>
host::logic::add_friend::response(const server::worker &worker, server::protocol::http::request &request) const
{
	using namespace server::protocol::http;
	
	static const std::string bad_request_body = "{\"status\":\"bad_request\"}"s;
	
	static const std::string status_ok = "{\"status\":\"ok\"}"s;
	
	
	// User have not session id => ignore him
	// This host can process only POST requests
	if (request.method != method::POST || request.cookies.count("sid"s) == 0)
		return this->redirect_response(worker, request, "/"s);	// Hard redirect
	
	
	const auto handle_error =
		[&](const auto &log_message, const std::string &response_body)
		{
			this->log_error(worker, request, log_message, status::ok);
			return this->response_with_json_body(worker, request, status::ok, response_body, false);
		};
	
	
	std::string user_ref, section;
	try {
		std::string trailling_path;
		const auto fix_request_path =
			[&]
			{
				if (trailling_path.empty() || trailling_path[0] != '/') {
					request.path = '/';
					request.path += trailling_path;
				} else {
					request.path = std::move(trailling_path);
				}
			};
		
		
		std::tie(user_ref, trailling_path) = host::split_first_section(request.path);
		fix_request_path();
		
		std::tie(section,  trailling_path) = host::split_first_section(request.path);
		fix_request_path();
	} catch (const std::logic_error &) {	// Incorrect path
		return this->redirect_response(worker, request, "/"s);	// Hard redirect
	}
	
	
	// Processing add_friend form
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
	} catch (const std::exception &e) {
		static const std::string response_body = "{\"status\":\"unknown_error\"}"s;
		return handle_error(e.what(), response_body);
	}
}
