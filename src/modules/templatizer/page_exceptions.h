// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef TEMPLATIZER_PAGE_EXCEPTIONS_H
#define TEMPLATIZER_PAGE_EXCEPTIONS_H

#include <string>

#include <templatizer/exceptions.h>


namespace templatizer {


class page_error: public templatizer::error
{
public:
	explicit page_error(const std::string &what_arg);
};


class file_mapping_error: public page_error
{
public:
	explicit file_mapping_error(const std::string &file_name, const std::string &message);
};


class file_parsing_error: public page_error
{
public:
	explicit file_parsing_error(const std::string &file_name, const std::string &message);
};


class path_not_found: public page_error
{
public:
	explicit path_not_found(const std::string &path);
};


};	// namespace templatizer


#endif // TEMPLATIZER_PAGE_EXCEPTIONS_H
