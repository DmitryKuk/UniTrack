// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SYSTEM__UTILS_H
#define SYSTEM__UTILS_H

#include <string>


namespace system_ {


// Returns given file mime type as a string or throws system_::unknown_error.
std::string get_file_mime(const std::string &path,
						  const std::string &command = "file -Ib");	// Using external file utility


};	// namespace system_


#endif	// SYSTEM__UTILS_H
