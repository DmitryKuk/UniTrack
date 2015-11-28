// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef HOST_EXCEPTIONS_H
#define HOST_EXCEPTIONS_H

#include <string>
#include <stdexcept>

#include <server/host/exceptions.h>


namespace host {


using server::host::error;


class template_pages_cache_error: public error
{
public:
	explicit template_pages_cache_error(const std::string &path);
};


};	// namespace host


#endif	// HOST_EXCEPTIONS_H
