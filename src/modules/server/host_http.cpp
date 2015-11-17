// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/host_http.h>

#include <chrono>
#include <mutex>
#include <numeric>

#include <server/host_exceptions.h>


namespace {


const std::string host_without_name = "";


};	// namespace


// static
std::unique_ptr<server::host> server::host_http::error_host_;


server::host_http::host(logger::logger &logger,
				   const server::host_http_parameters &parameters):
	logger::enable_logger(logger),
	
	host_http_parameters_(parameters),
	
	server_name_generator_(std::chrono::system_clock::now().time_since_epoch().count())
{}


// Returns true, if host can process requests on specified port, or false otherwise.
bool
server::host_http::port_allowed(server::port_t port) const noexcept
{
	if (this->host_http_parameters_.ports.find(port) == this->host_http_parameters_.ports.end())
		return false;
	return true;
}


// Returns host name as string (random!)
const std::string &
server::host_http::server_name() const noexcept
{
	if (this->host_http_parameters_.server_names.empty())
		return ::host_without_name;
	
	size_t index = this->server_name_generator_() % this->host_http_parameters_.server_names.size();
	return this->host_http_parameters_.server_names[index];
}


// Prepares a correct response to the client.
// NOTE: By default -- phony "404 Not Found". Redefine this function in child classes.
// virtual
std::unique_ptr<server::protocol::http::response>
response(std::unique_ptr<server::protocol::http::request> &&request)
{
	return this->phony_response(request, server::http::status::not_found);
}


// Prepares a phony response to the client.
// WARNING: Remember to save anywhere status too (standard statuses are already saved)!
template<class Headers>
std::unique_ptr<server::protocol::http::response>
phony_response(std::unique_ptr<server::protocol::http::request> &&request,
			   const server::protocol::http::status &status)
{
	using namespace server::protocol::http;
	using base::buffer;
	
	
	auto response = std::make_unique<server::protocol::http::response>(status, request->version);
	
	
	// Server name
	auto server_name = this->server_name();
	response->add_header(header_server, server_name);
	
	
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
	
	
	// Status strings
	const std::string &code_str    = status.code_str(),
					  $description = status.description();
	
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
		
		content_len_ptr = &response.cache(std::to_string(content_len));
	}
	
	// Adding content length header
	response->add_header(header::content_length, *content_len_ptr);
	response->finish_headers();
	
	// Adding body
	for (const auto body_element: body)
		response.add_body(*body_element);
	
	return response;
}


// Returns reference to error host object, creating it, if does not exist.
// If it didn't exist, it will be binded to logger of object's, whoose method was called.
server::host_http &
server::host_http::error_host() const
{
	return server::host_http::error_host(this->logger());
}


// Returns reference to error host object, creating it, if does not exist.
// If it didn't exist, it will be binded to logger.
// static
server::host_http &
server::host_http::error_host(logger::logger &logger)
{
	if (server::host_http::error_host_ == nullptr)
		server::host_http::create_error_host(logger);
	return *server::host_http::error_host_;
}


// Creates error_host if it does not exist. You may call it once from server, if you want.
// static
void
server::host_http::create_error_host(logger::logger &logger)
{
	static std::mutex m;
	
	std::unique_lock<std::mutex> lock(m);
	if (server::host_http::error_host_ == nullptr)
		server::host_http::error_host_ = std::make_unique<server::host>(logger, server::host_http_parameters());
}
