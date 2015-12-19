// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef BASE_TIME_H
#define BASE_TIME_H

#include <chrono>
#include <string>


namespace base {


// Returns current time in milliseconds
inline std::time_t utc_time() noexcept;

// Returns time as a string in GMT format
std::string time_for_cookie(std::time_t milliseconds);


};	// namespace base


#include <base/time.hpp>

#endif	// BASE_TIME_H
