// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/protocol/http/http.h>

#include <unordered_map>
#include <regex>
#include <cctype>

#include <server/protocol/http/exceptions.h>

#include <server/protocol/http/statuses.hpp>


const server::port_t
	server::protocol::http::default_port			= 80;


const std::string
	server::protocol::http::str::default_port		= std::to_string(server::protocol::http::default_port),
	server::protocol::http::str::space				= " ",
	server::protocol::http::str::newline			= "\r\n",
	server::protocol::http::str::HTTP				= "HTTP",
	server::protocol::http::str::slash				= "/",
	server::protocol::http::str::header_separator	= ": ";


const std::string
	server::protocol::http::header::content_length	= "Content-Length",
	server::protocol::http::header::server			= "Server",
	server::protocol::http::header::allow			= "Allow",
	server::protocol::http::header::connection		= "Connection",
	server::protocol::http::header::host			= "Host";


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
	static const std::unordered_map<server::protocol::http::method, std::string> method_to_str_map{
		{ server::protocol::http::method::GET,		method_GET     },
		{ server::protocol::http::method::HEAD,		method_HEAD    },
		{ server::protocol::http::method::POST,		method_POST    },
		{ server::protocol::http::method::DELETE,	method_DELETE  },
		{ server::protocol::http::method::unknown,	method_unknown }
	};
	
	
	try {
		return method_to_str_map.at(method);
	} catch (const std::out_of_range &) {
		return method_unknown;
	}
}


server::protocol::http::method
server::protocol::http::str_to_method(const std::string &str) noexcept
{
	static const std::unordered_map<std::string, server::protocol::http::method> str_to_method_map{
		{ method_GET,		server::protocol::http::method::GET     },
		{ method_HEAD,		server::protocol::http::method::HEAD    },
		{ method_POST,		server::protocol::http::method::POST    },
		{ method_DELETE,	server::protocol::http::method::DELETE  },
		{ method_unknown,	server::protocol::http::method::unknown }
	};
	
	
	try {
		return str_to_method_map.at(str);
	} catch (const std::out_of_range &) {
		return server::protocol::http::method::unknown;
	}
}


const std::string &
server::protocol::http::version_to_str(server::protocol::http::version version) noexcept
{
	static const std::unordered_map<server::protocol::http::version, std::string> version_to_str_map{
		{ server::protocol::http::version::v_1_1,	version_v_1_1   },
		{ server::protocol::http::version::unknown,	version_unknown }
	};
	
	
	auto it = version_to_str_map.find(version);
	if (it == version_to_str_map.end()) return version_unknown;
	return *(it->second);
}


server::protocol::http::version
server::protocol::http::str_to_version_str(const std::string &str) noexcept
{
	static const std::unordered_map<std::string, server::protocol::http::method> str_to_version_map{
		{ version_v_1_1,	server::protocol::http::version::v_1_1   },
		{ version_unknown,	server::protocol::http::version::unknown }
	};
	
	
	try {
		return str_to_version_map.at(str);
	} catch (const std::out_of_range &) {
		return server::protocol::http::version::unknown;
	}
}


server::protocol::http::status::status(unsigned int code, std::string description) noexcept:
	code_(code),
	code_str_(std::move(std::to_string(code_))),
	description_(std::move(description))
{}
