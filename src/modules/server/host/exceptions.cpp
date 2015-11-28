// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/host/exceptions.h>


server::host::error::error(const std::string &what_arg):
	std::logic_error(what_arg)
{}


server::host::incorrect_config::incorrect_config(const std::string &what_arg):
	server::host::error("Incorrect host config: " + what_arg)
{}


server::host::host_not_found::host_not_found(const std::string &name):
	server::host::error("Host not found: \"" + name + '\"')
{}


server::host::port_not_allowed::port_not_allowed(const std::string &host, ::server::port_type port):
	server::host::error("Port not allowed: " + std::to_string(port) + "(host: \"" + host + "\")")
{}


server::host::method_not_allowed::method_not_allowed(const std::string &method_name):
	server::host::error("Method not allowed: \"" + method_name + '\"')
{}


server::host::path_forbidden::path_forbidden(const std::string &path):
	server::host::error("Path forbidden: \"" + path + '\"')
{}

server::host::path_forbidden::path_forbidden(const std::string &path, const std::string &message):
	server::host::error("Path forbidden: \"" + path + "\": " + message)
{}


server::host::path_not_found::path_not_found(const std::string &path):
	server::host::error("Path not found: \"" + path + '\"')
{}


server::host::path_is_directory::path_is_directory(const std::string &path):
	server::host::error("Path is directory: \"" + path + '\"')
{}


server::host::incorrect_uri::incorrect_uri(const std::string &uri):
	server::host::error("Incorrect URI: \"" + uri + '\"')
{}
