// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_FILE_HOST_HANDLER_EXCEPTIONS_H
#define SERVER_FILE_HOST_HANDLER_EXCEPTIONS_H

#include <stdexcept>


namespace server {


class file_host_handler_error: public std::logic_error
{
public:
	explicit file_host_handler_error(const std::string &what_arg);
};


};	// namespace server


#endif // SERVER_FILE_HOST_HANDLER_EXCEPTIONS_H
