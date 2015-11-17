// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/host_http_exceptions.h>


server::host_http_error::host_http_error(const std::string &what_arg):
	std::logic_error(what_arg)
{}


server::headers_has_content_length::headers_has_content_length():
	server::host_http_error("Headers for phony page must NOT contain header \"Content-Length\"")
{}


server::duplicate_header::duplicate_header(const std::string &name):
	server::host_http_error("Duplicate header: \"" + name + '\"')
{}


server::host_not_found::host_not_found(const std::string &name):
	server::host_http_error("Host \"" + name + "\" not found")
{}


server::denied_uri::denied_uri(const std::string &uri):
	server::host_http_error("Requested denied URI: \"" + uri + '\"')
{}


server::method_not_allowed::method_not_allowed(const std::string &method_name):
	server::host_http_error("Method not allowed: \"" + method_name + '\"')
{}


server::path_forbidden::path_forbidden(const std::string &path):
	server::host_http_error("Path forbidden: \"" + path + '\"')
{}


server::path_not_found::path_not_found(const std::string &path):
	server::host_http_error("Path not found: \"" + path + '\"')
{}


server::incorrect_uri::incorrect_uri(const std::string &uri):
	server::host_http_error("Incorrect URI: \"" + uri + '\"')
{}
