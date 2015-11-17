// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_PROTOCOL_EXCEPTIONS_H
#define SERVER_PROTOCOL_EXCEPTIONS_H

#include <string>
#include <exception>


namespace server {
namespace protocol {


class error: public std::logic_error
{
public:
	explicit error(const std::string &what_arg);
};


};	// namespace protocol
};	// namespace server


#endif	// SERVER_PROTOCOL_EXCEPTIONS_H
