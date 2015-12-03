// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/host/base.h>

#include <mutex>
#include <numeric>

#include <base/json_utils.h>
#include <server/host/exceptions.h>
#include <server/worker.h>

using namespace std::literals;


// class ::server::host::base::parameters
server::host::base::parameters::parameters(const nlohmann::json &config):
	name{::base::json_utils::get<decltype(this->name)>(config, "name"s)},
	ports{::base::json_utils::get<decltype(this->ports)>(config, "ports"s)}
{}


// class ::server::host::base
server::host::base::base(const ::server::host::base::parameters &parameters):
	parameters_{parameters}
{}


// Returns true, if host can process requests on specified port, or false otherwise.
bool
server::host::base::port_allowed(::server::port_type port) const noexcept
{
	if (this->parameters_.ports.count(port) > 0)
		return true;
	return false;
}


// Prepares a correct response to the client.
// NOTE: By default -- phony "404 Not Found". Redefine this function in child classes.
// virtual
std::unique_ptr<::server::protocol::http::response>
server::host::base::response(const ::server::worker &worker,
							 const ::server::protocol::http::request &request) const
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
	
	
	// Body elements
	static const std::string
		body_1  =
			"<html>"
			"<head>"
				"<meta charset=\"utf-8\">"
				"<title>",
				// status.code_str()
		&body_2 =
				str::space,
				// status.description()
		body_3  =
				"</title>"
			"</head>"
			"<body>"
				"<h1>",
				// status.code_str()
		&body_4 =
				str::space,
				// status.description()
		body_5  =
				"</h1>"
				"<hr width=\"100%\">"
				"<p>",
				// Server name
		body_6  =
				"</p>"
			"</body>"
			"</html>";
	
	
	// Response
	auto response_ptr = std::make_unique<::server::protocol::http::response>(status, request.version);
	
	
	// Server name (even, if it is empty)
	const auto &server_name = *(::server::host::base::add_server_name(worker, *response_ptr).first);
	
	
	// Status strings
	const std::string &code_str    = status.code_str(),
					  &description = status.description();
	
	std::vector<const std::string *> body_ptrs{{ &body_1, &code_str, &body_2, &description,
												 &body_3, &code_str, &body_4, &description,
												 &body_5, &server_name, &body_6 }};
	
	// Calculating content length
	const std::string *content_len_ptr = nullptr;
	{
		size_t content_len = std::accumulate(
			std::begin(body_ptrs), std::end(body_ptrs), size_t{0},
			[](size_t current, const std::string *body_element_ptr) -> size_t
			{
				return current + body_element_ptr->size();
			}
		);
		
		content_len_ptr = &response_ptr->cache(std::to_string(content_len));
	}
	
	// Adding content length header
	response_ptr->add_header(::server::protocol::http::header::content_length, *content_len_ptr);
	response_ptr->finish_headers();
	
	// Adding body
	for (const auto body_element_ptr: body_ptrs)
		response_ptr->add_body(*body_element_ptr);
	
	
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
