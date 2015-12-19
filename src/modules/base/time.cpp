// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <base/time.h>

#include <ctime>


// Returns time as a string in GMT format
std::string
base::time_for_cookie(std::time_t milliseconds)
{
	std::time_t seconds = milliseconds / 1000;
	
	std::tm time_now;
	char str[80];
	strftime(str, 80, "%a, %d-%b-%Y %H:%M:%S GMT", ::gmtime_r(&seconds, &time_now));
	
	return str;
}
