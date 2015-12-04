// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_EXCEPTIONS_H
#define SERVER_EXCEPTIONS_H

#include <string>
#include <stdexcept>


namespace server {


class error: public std::logic_error
{
public:
	explicit error(const std::string &what_arg);
};


};	// namespace server


#endif	// SERVER_EXCEPTIONS_H
