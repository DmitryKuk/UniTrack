// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/host/exceptions.h>


server::host::error::error(const std::string &what_arg):
	std::logic_error(what_arg)
{}


server::host::headers_has_content_length::headers_has_content_length():
	server::host::error("Headers for phony page must NOT contain header \"Content-Length\"")
{}


server::host::duplicate_header::duplicate_header(const std::string &name):
	server::host::error("Duplicate header: \"" + name + '\"')
{}


server::host::host_not_found::host_not_found(const std::string &name):
	server::host::error("Host \"" + name + "\" not found")
{}


server::host::denied_uri::denied_uri(const std::string &uri):
	server::host::error("Requested denied URI: \"" + uri + '\"')
{}


server::host::method_not_allowed::method_not_allowed(const std::string &method_name):
	server::host::error("Method not allowed: \"" + method_name + '\"')
{}


server::host::path_forbidden::path_forbidden(const std::string &path):
	server::host::error("Path forbidden: \"" + path + '\"')
{}


server::host::path_not_found::path_not_found(const std::string &path):
	server::host::error("Path not found: \"" + path + '\"')
{}


server::host::incorrect_uri::incorrect_uri(const std::string &uri):
	server::host::error("Incorrect URI: \"" + uri + '\"')
{}
