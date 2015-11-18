// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/host/base/host.h>

#include <chrono>
#include <mutex>
#include <numeric>

#include <base/json_utils.h>
#include <server/host/exceptions.h>


namespace {


const std::string host_without_name = "";


};	// namespace


// class server::host::base::parameters
server::host::base::parameters::parameters(const nlohmann::json &config):
	name(base::json_utils::get<decltype(this->name)>(config, "name")),
	ports(base::json_utils::get<decltype(this->ports)>(config, "ports"))
{
	base::json_utils::extract(config, this->server_names, "server_names");
}


// class server::host::base
// static
std::unique_ptr<server::host::base> server::host::base::error_host_ptr_;


server::host::base::base(logger::logger &logger,
						 const server::host::base::parameters &parameters):
	logger::enable_logger(logger),
	
	parameters_(parameters),
	
	server_name_generator_(std::chrono::system_clock::now().time_since_epoch().count())
{}


// Returns true, if host can process requests on specified port, or false otherwise.
bool
server::host::base::port_allowed(server::port_type port) const noexcept
{
	if (this->parameters_.ports.find(port) == this->parameters_.ports.end())
		return false;
	return true;
}


// Returns host name as string (random!)
const std::string &
server::host::base::server_name() const noexcept
{
	if (this->parameters_.server_names.empty())
		return ::host_without_name;
	
	size_t index = this->server_name_generator_() % this->parameters_.server_names.size();
	return this->parameters_.server_names[index];
}


// Prepares a correct response to the client.
// NOTE: By default -- phony "404 Not Found". Redefine this function in child classes.
// virtual
server::protocol::http::response::ptr_type
server::host::base::response(server::protocol::http::request::ptr_type request_ptr)
{
	return this->phony_response(request_ptr, server::http::status::not_found);
}


// Prepares a phony response to the client.
// WARNING: Remember to save anywhere status too (standard statuses are already saved)!
template<class Headers>
server::protocol::http::response::ptr_type
phony_response(server::protocol::http::request::ptr_type request_ptr,
			   const server::protocol::http::status &status)
{
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
	auto response_ptr = std::make_shared<server::protocol::http::response>(status, request_ptr->version);
	
	
	// Server name
	auto server_name = this->server_name();
	response_ptr->add_header(server::protocol::http::header::server, server_name);
	
	
	// Status strings
	const std::string &code_str    = status.code_str(),
					  &description = status.description();
	
	std::vector<const std::string *> body = { &body_1, &code_str, &body_2, &description,
											  &body_3, &code_str, &body_4, &description,
											  &body_5, &server_name, &body_6 };
	
	// Calculating content length
	const std::string *content_len_ptr = nullptr;
	{
		size_t content_len = std::accumulate(
			std::begin(body), std::end(body), size_t{0},
			[](size_t current, const std::string &body_element) -> size_t
			{
				return current + body_element.size();
			}
		);
		
		content_len_ptr = &response_ptr->cache(std::to_string(content_len));
	}
	
	// Adding content length header
	response_ptr->add_header(server::protocol::http::header::content_length, *content_len_ptr);
	response_ptr->finish_headers();
	
	// Adding body
	for (const auto body_element: body)
		response_ptr->add_body(*body_element);
	
	return response_ptr;
}


// Returns reference to error host object, creating it, if does not exist.
// If it didn't exist, it will be binded to logger of object's, whoose method was called.
server::host::base &
server::host::base::error_host() const
{
	return server::host::base::error_host(this->logger());
}


// Returns reference to error host object, creating it, if does not exist.
// If it didn't exist, it will be binded to logger.
// static
server::host::base &
server::host::base::error_host(logger::logger &logger)
{
	if (server::host::base::error_host_ptr_ == nullptr)
		server::host::base::create_error_host(logger);
	return *server::host::base::error_host_ptr_;
}


// Creates error_host if it does not exist. You may call it once from server, if you want.
// static
void
server::host::base::create_error_host(logger::logger &logger)
{
	static std::mutex m;
	
	std::unique_lock<std::mutex> lock(m);
	if (server::host::base::error_host_ptr_ == nullptr)
		server::host::base::error_host_ptr_ = std::make_unique<server::host>(logger, server::host_http_parameters());
}
