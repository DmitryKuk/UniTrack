// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGGER_LEVEL_H
#define LOGGER_LEVEL_H

#include <syslog.h>


namespace logger {
namespace level {


// Regular levels
template<int Level>
struct regular
{
	enum { value = Level };
};	// struct regular


constexpr regular<LOG_DEBUG  > debug		{};
constexpr regular<LOG_INFO   > info			{};
constexpr regular<LOG_WARNING> warning		{};
constexpr regular<LOG_ERR    > error		{};
constexpr regular<LOG_CRIT   > critical		{};
constexpr regular<LOG_NOTICE > other		{};



// Security levels
template<int Level>
struct security
{
	enum { value = Level };
};	// struct security


constexpr security<LOG_DEBUG  > sec_debug	{};
constexpr security<LOG_INFO   > sec_info	{};
constexpr security<LOG_WARNING> sec_warning	{};
constexpr security<LOG_ERR    > sec_error	{};
constexpr security<LOG_CRIT   > sec_critical{};
constexpr security<LOG_NOTICE > sec_other	{};


};	// namespace level
};	// logger


#endif	// LOGGER_LEVEL_H
