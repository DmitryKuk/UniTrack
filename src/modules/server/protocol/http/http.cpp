// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/protocol/http/http.h>

#include <unordered_map>
#include <regex>
#include <cctype>

#include <server/protocol/http/exceptions.h>

#include <server/protocol/http/statuses.hpp>


const server::port_type
	server::protocol::http::default_port				= 80;


const std::string
	server::protocol::http::str::default_port			= std::to_string(server::protocol::http::default_port),
	server::protocol::http::str::space					= " ",
	server::protocol::http::str::newline				= "\r\n",
	server::protocol::http::str::HTTP					= "HTTP",
	server::protocol::http::str::slash					= "/",
	server::protocol::http::str::header_separator		= ": ";

const std::string
	server::protocol::http::str::header_body_separator	= "\r\n\r\n";


const std::string
	server::protocol::http::header::content_length		= "Content-Length",
	server::protocol::http::header::server				= "Server",
	server::protocol::http::header::allow				= "Allow",
	server::protocol::http::header::connection			= "Connection",
	server::protocol::http::header::host				= "Host";


namespace {

const std::string
	method_GET("GET"),
	method_HEAD("HEAD"),
	method_POST("POST"),
	method_DELETE("DELETE"),
	method_unknown("unknown");

const std::string
	version_v_1_1("1.1"),
	version_unknown("unknown");

};	// namespace


const std::string &
server::protocol::http::method_to_str(server::protocol::http::method method) noexcept
{
	typedef std::pair<server::protocol::http::method, std::string> pair;
	
	static const std::unordered_map<server::protocol::http::method, std::string> method_to_str_map{
		{
			pair{server::protocol::http::method::GET,		method_GET    },
			pair{server::protocol::http::method::HEAD,		method_HEAD   },
			pair{server::protocol::http::method::POST,		method_POST   },
			pair{server::protocol::http::method::DELETE,	method_DELETE },
			pair{server::protocol::http::method::unknown,	method_unknown}
		}
	};
	
	
	auto it = method_to_str_map.find(method);
	if (it == method_to_str_map.end())
		return method_unknown;
	return it->second;
}


server::protocol::http::method
server::protocol::http::str_to_method(const std::string &str) noexcept
{
	typedef std::pair<std::string, server::protocol::http::method> pair;
	
	static const std::unordered_map<std::string, server::protocol::http::method> str_to_method_map{
		{
			pair{method_GET,		server::protocol::http::method::GET    },
			pair{method_HEAD,		server::protocol::http::method::HEAD   },
			pair{method_POST,		server::protocol::http::method::POST   },
			pair{method_DELETE,		server::protocol::http::method::DELETE },
			pair{method_unknown,	server::protocol::http::method::unknown}
		}
	};
	
	
	auto it = str_to_method_map.find(str);
	if (it == str_to_method_map.end())
		return server::protocol::http::method::unknown;
	return it->second;
}


const std::string &
server::protocol::http::version_to_str(server::protocol::http::version version) noexcept
{
	typedef std::pair<server::protocol::http::version, std::string> pair;
	
	static const std::unordered_map<server::protocol::http::version, std::string> version_to_str_map{
		{
			pair{server::protocol::http::version::v_1_1,	version_v_1_1  },
			pair{server::protocol::http::version::unknown,	version_unknown}
		}
	};
	
	
	auto it = version_to_str_map.find(version);
	if (it == version_to_str_map.end())
		return version_unknown;
	return it->second;
}


server::protocol::http::version
server::protocol::http::str_to_version(const std::string &str) noexcept
{
	typedef std::pair<std::string, server::protocol::http::version> pair;
	
	static const std::unordered_map<std::string, server::protocol::http::version> str_to_version_map{
		{
			pair{version_v_1_1,		server::protocol::http::version::v_1_1  },
			pair{version_unknown,	server::protocol::http::version::unknown}
		}
	};
	
	
	auto it = str_to_version_map.find(str);
	if (it == str_to_version_map.end())
		return server::protocol::http::version::unknown;
	return it->second;
}


server::protocol::http::status::status(unsigned int code, std::string description) noexcept:
	code_(code),
	code_str_(std::move(std::to_string(code_))),
	description_(std::move(description))
{}
