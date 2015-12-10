// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/host/base.h>

#include <mutex>
#include <numeric>

#include <base/json_utils.h>
#include <server/host/exceptions.h>
#include <server/worker.h>

using namespace std::literals;


server::host::base::base(const nlohmann::json &config):
	name_{::base::json_utils::get<decltype(this->name_)>(config, "name"s)},
	ports_{::base::json_utils::get<decltype(this->ports_)>(config, "ports"s)}
{}


// Returns true, if host can process requests on specified port, or false otherwise.
bool
server::host::base::port_allowed(::server::port_type port) const noexcept
{
	if (this->ports_.count(port) > 0)
		return true;
	return false;
}


// Prepares a correct response to the client.
// NOTE: By default -- phony "404 Not Found". Redefine this function in child classes.
// virtual
std::unique_ptr<::server::protocol::http::response>
server::host::base::response(const ::server::worker &worker,
							 ::server::protocol::http::request &request) const
{
	return this->phony_response(worker, request, ::server::protocol::http::status::not_found);
}


// Prepares a phony response to the client.
// WARNING: Remember to save anywhere status too (standard statuses are already saved)!
std::unique_ptr<::server::protocol::http::response>
server::host::base::phony_response(const ::server::worker &worker,
								   const ::server::protocol::http::request &request,
								   const ::server::protocol::http::status &status) const
{
	using namespace ::server::protocol::http;
	
	
	// Response
	auto response_ptr = std::make_unique<::server::protocol::http::response>(status, request.version);
	
	
	// Server name (even, if it is empty)
	const auto &server_name = *(::server::host::base::add_server_name(worker, *response_ptr).first);
	
	// Adding body
	if (request.method != ::server::protocol::http::method::HEAD) {
		std::string status_str = status.code_str() + str::space + status.description();
		
		// Page body
		std::string body =
			"<html>"
			"<head>"
				"<meta charset=\"utf-8\">"
				"<title>"s
				+ status_str +
				"</title>"
			"</head>"
			"<body>"
				"<h1>"s
				+ status_str +
				"</h1>"
				"<hr width=\"100%\">"
				"<p>"s
				+ server_name +
				"</p>"
			"</body>"
			"</html>"s;
		
		response_ptr->add_header(header::content_length, std::to_string(body.size()));
		response_ptr->add_body(::base::buffer(body));
	}
	
	response_ptr->finish();
	
	return response_ptr;
}


// Gets server name from worker and adds it to headers in response.
// Returns pointer to used server name string and true, if name was added.
// Otherwise, returns pointer to empty string and false (if returned server name is empty).
// static
std::pair<const std::string *, bool>
server::host::base::add_server_name(const ::server::worker &worker, ::server::protocol::http::response &response)
{
	const std::string &server_name = worker.server_name();
	if (server_name.empty())
		return {&server_name, false};
	
	response.add_header(::server::protocol::http::header::server, server_name);
	return {&server_name, true};
}
