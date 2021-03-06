// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/protocol/http.h>

#include <vector>
#include <unordered_map>
#include <regex>
#include <cctype>


const server::port_type
	server::protocol::http::default_port					= 80;


const std::string
	server::protocol::http::str::default_port				= std::to_string(server::protocol::http::default_port),
	server::protocol::http::str::space						= " ",
	server::protocol::http::str::newline					= "\r\n",
	server::protocol::http::str::HTTP						= "HTTP",
	server::protocol::http::str::slash						= "/",
	server::protocol::http::str::header_separator			= ": ";

const std::string
	server::protocol::http::str::header_body_separator		= "\r\n\r\n";


const std::string
	server::protocol::http::header::content_length			= "Content-Length",
	server::protocol::http::header::content_type			= "Content-Type",
	server::protocol::http::header::server					= "Server",
	server::protocol::http::header::allow					= "Allow",
	server::protocol::http::header::connection				= "Connection",
	server::protocol::http::header::host					= "Host",
	server::protocol::http::header::cookie					= "Cookie",
	server::protocol::http::header::set_cookie				= "Set-Cookie",
	server::protocol::http::header::location				= "Location";


const std::string
	server::protocol::http::content_type::text_plain		= "text/plain",
	server::protocol::http::content_type::text_html			= "text/html",
	server::protocol::http::content_type::application_json	= "application/json";



namespace {


const std::string
	method_unknown	= "unknown",	// 0
	
	method_GET		= "GET",		// 1
	method_HEAD		= "HEAD",		// 2
	method_POST		= "POST",		// 3
	method_DELETE	= "DELETE";	// 4

const std::string
	version_unknown	= "unknown",	// 0
	
	version_v_1_1	= "1.1";		// 1


};	// namespace


const std::string &
server::protocol::http::method_to_str(server::protocol::http::method method) noexcept
{
	static const std::vector<const std::string *> method_str_vector{
		{
			&method_unknown,
			
			&method_GET,
			&method_HEAD,
			&method_POST,
			&method_DELETE
		}
	};
	
	
	size_t method_val = static_cast<size_t>(method);
	if (method_val >= method_str_vector.size())
		return method_unknown;
	return *method_str_vector[method_val];
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
	static const std::vector<const std::string *> version_str_vector{
		{
			&version_unknown,
			
			&version_v_1_1
		}
	};
	
	
	size_t version_val = static_cast<size_t>(version);
	if (version_val >= version_str_vector.size())
		return version_unknown;
	return *version_str_vector[version_val];
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
