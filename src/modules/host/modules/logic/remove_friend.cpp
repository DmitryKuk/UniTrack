// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/modules/logic/remove_friend.h>

#include <tuple>

#include <server/worker.h>
#include <host/module.h>
#include <host/path_utils.h>
#include <logic/exceptions.h>

using namespace std::literals;


namespace {


host::module<host::logic::remove_friend> module{
	"logic/remove_friend"s,
	[](const nlohmann::json &host_config, ::logic::global_instance &logic)
	{
		return std::make_shared<host::logic::remove_friend>(host_config, logic);
	}
};


};	// namespace



host::logic::remove_friend::remove_friend(const nlohmann::json &config, ::logic::global_instance &logic):
	base_logic{logic},
	base_host{config}
{}


// virtual
std::unique_ptr<server::protocol::http::response>
host::logic::remove_friend::response(const server::worker &worker, server::protocol::http::request &request) const
{
	using namespace server::protocol::http;
	
	
	static const std::string bad_request_body = "{\"status\":\"bad_request\"}"s;
	
	
	// Validation
	if (request.method != method::POST)
		return this->response_with_json_body(worker, request, status::ok, bad_request_body, false);
	
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
	
	
	std::string friend_ref;
	try {
		std::string trailing_path;
		std::tie(friend_ref, trailing_path) = host::split_first_section(request.path);
		
		if (!trailing_path.empty() && trailing_path != "/"s)
			throw std::logic_error{""s};
	} catch (const std::logic_error &) {	// Incorrect path
		return this->response_with_json_body(worker, request, status::ok, bad_request_body, false);
	}
	
	
	try {
		static const std::string response_body = "{\"status\":\"ok\"}"s;
		
		std::string session_cookie = this->remove_friend_(friend_ref, request.cookies.at("sid"s));
		
		auto response_ptr = this->response_with_json_body(worker, request, status::ok, response_body, false);
		if (!session_cookie.empty())
			response_ptr->add_header(header::set_cookie, session_cookie);
		
		return std::move(response_ptr);
	} catch (const std::exception &e) {
		static const std::string response_body = "{\"status\":\"unknown_error\"}"s;
		return handle_error(e.what(), response_body);
	}
}
