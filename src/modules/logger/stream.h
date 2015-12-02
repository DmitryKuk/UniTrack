// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGGER_STREAM_H
#define LOGGER_STREAM_H

#include <sstream>
#include <string>
#include <regex>

#include <logger/level.h>
#include <logger/enable_logger_ref_impl.h>


namespace logger {


template<class Logger>
class stream:
	public std::ostringstream,
	protected logger::enable_logger_ref_impl<Logger>
{
public:
	stream(Logger &logger,
				  level level,
				  bool add_level_prefix = true) noexcept;
	~stream();
	
	stream(stream &&other) noexcept;
	stream & operator=(stream &&other) noexcept;
	
	// Non-copy constructible/assignable
	stream(const stream &other) = delete;
	stream & operator=(const stream &other) = delete;
private:
	// Data
	level level_;
};	// class stream


};	// logger


#include <logger/stream.hpp>

#endif	// LOGGER_STREAM_H
