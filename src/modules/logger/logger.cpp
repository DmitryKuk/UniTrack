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
	} catch (...) {}
	
	this->flush();
}


logger::stream<::logger::logger>
logger::logger::stream(::logger::level level, bool add_level_prefix) noexcept
{
	return {*this, level, add_level_prefix};
}


void
logger::logger::log_raw(::logger::level /* level */, const std::string &data) noexcept
{
	try {
		this->log_stream_ << data << std::endl;
	} catch (...) {}
}


void
logger::logger::flush() noexcept
{
	try {
		this->log_stream_.flush();
	} catch (...) {}
}
