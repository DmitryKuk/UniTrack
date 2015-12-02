// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGGER_LOGGER_H
#define LOGGER_LOGGER_H

#include <iostream>

#include <logger/level.h>
#include <logger/stream.h>
#include <logger/enable_logger_impl.h>
#include <logger/enable_logger_ref_impl.h>


namespace logger {


class logger
{
public:
	logger(std::ostream &log_stream, bool colorize_output = false) noexcept;
	~logger();
	
	
	// Non-copy/-move constructible/assignable. Use ptrs.
	logger(const logger &other) = delete;
	logger(logger &&other) = delete;
	
	logger & operator=(const logger &other) = delete;
	logger & operator=(logger &&other) = delete;
	
	
	inline bool colorize_output() const noexcept;
	
	
	stream<logger> stream(level level, bool add_level_prefix = true) noexcept;
	
	void log_raw(level level_, const std::string &data) noexcept;
	
	void flush() noexcept;
private:
	std::ostream &log_stream_;
	
	bool colorize_output_;
};	// class logger


typedef ::logger::enable_logger_impl<logger> enable_logger;
typedef ::logger::enable_logger_ref_impl<logger> enable_logger_ref;


};	// namespace logger


#include <logger/logger.hpp>

#endif	// LOGGER_LOGGER_H
