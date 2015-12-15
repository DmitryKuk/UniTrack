// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef BASE_TIME_H
#define BASE_TIME_H

#include <chrono>


namespace base {


// Return current time in milliseconds
inline std::time_t utc_time() noexcept;


};	// namespace base


#include <base/time.hpp>

#endif	// BASE_TIME_H
