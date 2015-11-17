// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef BASE_STRINGS_CACHE_H
#define BASE_STRINGS_CACHE_H

#include <string>
#include <deque>


namespace base {


class strings_cache
{
public:
	explicit strings_cache() = default;
	
	// Non-copy/move constructable/assignable
	strings_cache(const strings_cache &other) = delete;
	strings_cache(strings_cache &&other) = delete;
	
	strings_cache & operator=(const strings_cache &other) = delete;
	strings_cache & operator=(strings_cache &&other) = delete;
	
	
	// Returns cached copy of given string
	inline const std::string & cache(const std::string &str);
	inline const std::string & cache(std::string &&str);
private:
	std::deque<std::string> strings_;
};	// class strings_cache


};	// namespace base


#include <base/strings_cache.hpp>

#endif	// BASE_STRINGS_CACHE_H
