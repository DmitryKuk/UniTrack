// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logger/level.h>

#include <vector>


namespace {


const std::vector<std::string> level_strings{
	{
		// Base levels
		"info        ",	// 0
		"warning     ",	// 1
		"error       ",	// 2
		"critical    ",	// 3
		"other       ",	// 4
		
		// Security levels
		"sec_info    ",	// 5
		"sec_warning ",	// 6
		"sec_error   ",	// 7
		"sec_critical",	// 8
		"sec_other   "	// 9
	}
};


const std::string level_unknown
	=	"unknown     ";


};	// namespace


const std::string &
logger::level_to_str(logger::level level_) noexcept
{
	auto level_val = static_cast<unsigned int>(level_);
	if (level_val < level_strings.size())
		return level_strings[level_val];
	return level_unknown;
}
