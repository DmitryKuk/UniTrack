// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_SERVER_EXCEPTIONS_H
#define SERVER_SERVER_EXCEPTIONS_H

#include <string>
#include <stdexcept>


namespace server {


class server_error: public std::server_error
{
public:
	explicit server_error(const std::string &what_arg);
};


// Start string
class server_not_started: public server_error
{
public:
	explicit server_not_started();
};


};	// namespace server


#endif	// SERVER_SERVER_EXCEPTIONS_H
