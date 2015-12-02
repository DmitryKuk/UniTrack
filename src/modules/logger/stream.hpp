// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <ctime>


template<class Logger>
logger::stream<Logger>::stream(Logger &logger,
							   logger::level level,
							   bool add_level_prefix) noexcept:
	logger::enable_logger_ref_impl<Logger>{logger},
	
	level_{level}
{
	if (add_level_prefix)
		logger::print_level_str(*this, this->level_, this->logger().colorize_output());
	
	
	// Printing timestamp in format: DD.MM.YYYY hh:mm:ss
	std::time_t raw_time;
	std::time(&raw_time);
	
	auto *time_info = std::localtime(&raw_time);
	char time_str[22];
	std::strftime(time_str, 22, "%d.%m.%Y %H:%M:%S  ", time_info);
	
	*this << time_str;
}


template<class Logger>
logger::stream<Logger>::stream(logger::stream<Logger> &&other) noexcept:
	logger::enable_logger_ref_impl<Logger>{std::forward<logger::enable_logger_ref_impl<Logger>>(other)},
	
	level_{other.level_}
{
	other.reset_logger();
	
	try {
		this->std::ostringstream::operator=(std::forward<std::ostringstream>(other));	// This should not throw
	} catch (...) {}
}


template<class Logger>
logger::stream<Logger>::~stream()
{
	if (this->check_logger())
		this->logger().log_raw(this->level_, this->str());
}


template<class Logger>
logger::stream<Logger> &
logger::stream<Logger>::operator=(logger::stream<Logger> &&other) noexcept
{
	this->logger::enable_logger_ref_impl<Logger>::operator=(
		std::forward<logger::enable_logger_ref_impl<Logger>>(other)
	);
	
	other.reset_logger();
	
	this->level_ = other.level_;
	
	try {
		this->std::ostringstream::operator=(std::forward<std::ostringstream>(other));	// This should not throw
	} catch (...) {}
	
	return *this;
}
