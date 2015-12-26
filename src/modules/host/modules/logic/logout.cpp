// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/modules/logic/logout.h>

#include <tuple>

#include <server/worker.h>
#include <host/module.h>
#include <logic/exceptions.h>

using namespace std::literals;


namespace {


host::module<host::logic::logout> module{
	"logic/logout"s,
	[](const nlohmann::json &host_config, ::logic::global_instance &logic)
	{
		return std::make_shared<host::logic::logout>(host_config, logic);
	}
};


};	// namespace



host::logic::logout::logout(const nlohmann::json &config, ::logic::global_instance &logic):
	base_logic{logic},
	base_host{config}
{}


// virtual
std::unique_ptr<server::protocol::http::response>
host::logic::logout::response(const server::worker &worker, server::protocol::http::request &request) const
{
	using namespace server::protocol::http;
	
	
	// This host can process only POST requests
	if (request.method != method::POST)
		return nullptr;	// Soft redirect
	
	auto sid_it = request.cookies.find("sid"s);
	
	// User have not session id => don't need logout him
	if (sid_it == request.cookies.end()) {
		static const std::string not_logged_in_body	= "{\"status\":\"not_logged_in\"}"s;
		return this->response_with_json_body(worker, request, status::ok, not_logged_in_body, false);
	}
	
	
	static const std::string ok_body			= "{\"status\":\"ok\"}"s;
	static const std::string bad_request_body	= "{\"status\":\"bad_request\"}"s;
	static const std::string unknown_error_body	= "{\"status\":\"unknown_error\"}"s;
	
	const std::string *response_body_ptr = nullptr;
	try {
		this->logout_user(sid_it->second);
		
		response_body_ptr = &ok_body;
	} catch (const ::logic::session_error &e) {
		response_body_ptr = &bad_request_body;
	} catch (const std::exception &e) {
		response_body_ptr = &unknown_error_body;
	}
	
	auto response_ptr = this->response_with_json_body(worker, request, status::ok, *response_body_ptr, false);
	response_ptr->reset_cookie("sid"s);
	return std::move(response_ptr);
}
