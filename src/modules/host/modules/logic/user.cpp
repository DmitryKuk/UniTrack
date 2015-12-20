// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/modules/logic/user.h>

#include <tuple>
#include <functional>
#include <stdexcept>

#include <server/worker.h>
#include <host/module.h>
#include <logic/exceptions.h>
#include <host/path_utils.h>

using namespace std::literals;


namespace {


host::module<host::logic::user> module{
	"logic/user"s,
	[](const nlohmann::json &host_config, ::logic::global_instance &logic)
	{
		return std::make_shared<host::logic::user>(
			host_config,
			logic
		);
	}
};


};	// namespace


using sh_pair = std::pair<std::string, host::logic::user::section_handler_type>;

// static
const std::unordered_map<std::string, host::logic::user::section_handler_type>
	host::logic::user::section_handlers_{
		{
			sh_pair{""s, &host::logic::user::section_root}
		}
	};


host::logic::user::user(const nlohmann::json &config,
						::logic::global_instance &logic):
	::logic::user{logic},
	base_host{config}
{}


// virtual
std::unique_ptr<server::protocol::http::response>
host::logic::user::response(const server::worker &worker,
							server::protocol::http::request &request) const
{
	using namespace server::protocol::http;
	
	
	// User have not session id => don't show any page him!
	// This host can process only GET and HEAD requests with "?json" suffix in uri
	if ((request.method != method::GET && request.method != method::HEAD)
		|| request.cookies.count("sid"s) == 0
		|| request.args_set.count("json"s) == 0) {
		request.path = "/"s;
		return nullptr;	// Soft redirect
	}
	
	
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
	
	
	// Search for section handler
	auto it = section_handlers_.find(section);
	if (it == section_handlers_.end())
		return this->redirect_response(worker, request, "/"s);	// Hard redirect
	
	return std::bind<std::unique_ptr<server::protocol::http::response>>(std::ref(it->second), this,
																		std::ref(worker), std::ref(request),
																		std::ref(user_ref), std::ref(section))();
}


// Section handlers
std::unique_ptr<server::protocol::http::response>
host::logic::user::section_root(const server::worker &worker,
								server::protocol::http::request &request,
								const std::string &user_ref,
								const std::string &section) const
{
	using namespace server::protocol::http;
	
	
	const auto handle_error =
		[&](const auto &log_message, const std::string &response_body)
		{
			this->log_error(worker, request, log_message, status::ok);
			return this->response_with_body(worker, request, status::ok, response_body, false);
		};
	
	
	try {
		std::string response_body, session_cookie;
		std::tie(response_body, session_cookie) = this->user_info(user_ref, request.cookies.at("sid"s));
		
		auto response_ptr = this->response_with_body(worker, request, status::ok, std::move(response_body));
		if (!session_cookie.empty())
			response_ptr->add_header(header::set_cookie, session_cookie);
		
		return std::move(response_ptr);
	} catch (const ::logic::session_not_found &e) {
		auto response_ptr = this->redirect_response(worker, request, "/"s);
		response_ptr->add_header(header::set_cookie, "sid=; expires=Thu, 01 Jan 1970 00:00:01 GMT;"s);
		
		return std::move(response_ptr);
	} catch (const std::exception &e) {
		static const std::string response_body = "{\"status\":\"unknown_error\"}"s;
		return handle_error(e.what(), response_body);
	}
}