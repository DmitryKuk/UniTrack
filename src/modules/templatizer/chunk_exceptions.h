// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef TEMPLATIZER_CHUNK_EXCEPTIONS_H
#define TEMPLATIZER_CHUNK_EXCEPTIONS_H

#include <string>

#include <templatizer/exceptions.h>


namespace templatizer {


class chunk_error: public templatizer::error
{
public:
	explicit chunk_error(const std::string &chunk_cmd, const std::string &what_arg);
};


};	// namespace templatizer


#endif // TEMPLATIZER_CHUNK_EXCEPTIONS_H
