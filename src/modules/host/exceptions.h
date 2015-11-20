// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef HOST_EXCEPTIONS_H
#define HOST_EXCEPTIONS_H

#include <string>
#include <stdexcept>

#include <server/host/exceptions.h>


namespace host {


using server::host::error;


class incorrect_config: public error
{
public:
	explicit incorrect_config(const std::string &message);
};


};	// namespace host


#endif	// HOST_EXCEPTIONS_H
