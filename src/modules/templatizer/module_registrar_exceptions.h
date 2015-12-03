// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef TEMPLATIZER_MODULE_REGISTRAR_EXCEPTIONS_H
#define TEMPLATIZER_MODULE_REGISTRAR_EXCEPTIONS_H

#include <string>

#include <templatizer/exceptions.h>


namespace templatizer {


class module_registrar_error: public templatizer::error
{
public:
	explicit module_registrar_error(const std::string &what_arg);
};


class module_not_found: public module_registrar_error
{
public:
	explicit module_not_found(const std::string &module_name);
};


};	// namespace templatizer


#endif	// TEMPLATIZER_MODULE_REGISTRAR_EXCEPTIONS_H
