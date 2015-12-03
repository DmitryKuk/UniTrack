// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef TEMPLATIZER_EXCEPTIONS_H
#define TEMPLATIZER_EXCEPTIONS_H

#include <string>
#include <stdexcept>


namespace templatizer {


class error: public std::logic_error
{
public:
	explicit error(const std::string &what_arg);
};


};	// namespace templatizer


#endif	// TEMPLATIZER_EXCEPTIONS_H
