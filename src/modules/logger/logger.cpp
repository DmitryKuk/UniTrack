// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logger/logger.h>


logger::logger::logger(std::ostream &log_stream, bool colorize_output) noexcept:
	log_stream_{log_stream},
	
	colorize_output_{colorize_output}
{
	try {
		this->stream(level::info)
			<< "Log started...";
	} catch (...) {}
}


logger::logger::~logger()
{
	try {
		this->stream(level::info)
			<< "End of log.";
		
		this->log_stream_.flush();
	} catch (...) {}
}


logger::record_object
logger::logger::stream(::logger::level level) noexcept
{
	return ::logger::record_object{*this, level};
}


void
logger::logger::log_raw(::logger::level level, const std::string &data) noexcept
{
	const auto &level_str = ::logger::level_to_str(level);
	
	try {
		if (this->colorize_output_)
			this->log_stream_ << "\033[32m";
		
		this->log_stream_ << level_str;
		
		if (this->colorize_output_)
			this->log_stream_ << "\033[0m";
		
		this->log_stream_ << ' ' << data << std::endl;
	} catch (...) {}
}
