// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <base/time.h>


std::time_t
base::utc_time() noexcept
{
	std::time_t t = std::time(nullptr);
	std::tm tm;
	return std::mktime(::gmtime_r(&t, &tm));
}
