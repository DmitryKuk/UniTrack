// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logger/level.h>

#include <vector>

using namespace std::literals;


namespace {


const std::vector<std::pair<std::string, std::string>> levels_data{
	{
		// Base levels
		std::make_pair("info        "s, "\033[32m"s				),	// 0, green
		std::make_pair("warning     "s, "\033[33m"s				),	// 1, yellow
		std::make_pair("error       "s, "\033[31m"s				),	// 2, red
		std::make_pair("critical    "s, "\033[4;31m"s			),	// 3, red underlined
		std::make_pair("other       "s, "\033[4;32m"s			),	// 4, green underlined
		
		// Security levels
		std::make_pair("sec_info    "s, "\033[32m\033[47m"s		),	// 5, green on white
		std::make_pair("sec_warning "s, "\033[33m\033[47m"s		),	// 6, yellow on white
		std::make_pair("sec_error   "s, "\033[31m\033[47m"s		),	// 7, red on white
		std::make_pair("sec_critical"s, "\033[4;31m\033[47m"s	),	// 8, red underlined on white
		std::make_pair("sec_other   "s, "\033[4;32m\033[47m"s	)	// 9, green underlined on white
	}
};


const std::pair<std::string, std::string> level_unknown =
		std::make_pair("unknown     "s, "\033[36m"s				);	// -, light-blue


const std::string color_reset = "\033[0m"s;


};	// namespace


const std::pair<std::string, std::string> &
logger::level_data(logger::level level) noexcept
{
	auto level_val = static_cast<unsigned int>(level);
	if (level_val < levels_data.size())
		return levels_data[level_val];
	return level_unknown;
}


std::ostream &
logger::print_level_str(std::ostream &stream, logger::level level, bool colorize_output) noexcept
{
	const auto &data = logger::level_data(level);
	
	try {
		if (colorize_output) stream << data.second;
		stream << data.first;
		if (colorize_output) stream << color_reset;
		stream << "  "s;
	} catch (...) {}	// Ignore ostream exceptions (if it was enabled)
	
	return stream;
}
