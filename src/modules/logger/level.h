// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGGER_LEVEL_H
#define LOGGER_LEVEL_H

#include <ostream>
#include <string>
#include <utility>


namespace logger {


enum class level
{
	// Base levels
	info			= 0,
	warning			= 1,
	error			= 2,
	critical		= 3,
	other			= 4,
	
	// Security levels
	sec_info		= 5,
	sec_warning		= 6,
	sec_error		= 7,
	sec_critical	= 8,
	sec_other		= 9
};


const std::pair<std::string, std::string> & level_data(level level) noexcept;

std::ostream & print_level_str(std::ostream &stream, level level, bool colorize_output) noexcept;


};	// logger


#endif	// LOGGER_LEVEL_H
