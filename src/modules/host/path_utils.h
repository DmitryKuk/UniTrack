// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef HOST_PATH_UTILS_H
#define HOST_PATH_UTILS_H

#include <string>
#include <utility>


namespace host {


// Returns first section and trailing path or throws std::logic_error.
// Example: "/user/photo/123?args=..." => ("user", "/photo/123?args=...")
std::pair<std::string, std::string> split_first_section(const std::string &path);


};	// namespace host


#endif	// HOST_PATH_UTILS_H
