// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGGER_LOGGER_H
#define LOGGER_LOGGER_H

#include <sstream>

#include <logger/level.h>


namespace logger {


template<class Level>
class stream;


class logger
{
public:
	logger(const std::string &log_name);
	logger(std::string &&log_name) noexcept;
	~logger();
	
	
	// Non-copy/-move constructible/assignable. Use ptrs.
	logger(const logger &other) = delete;
	logger(logger &&other) = delete;
	
	logger & operator=(const logger &other) = delete;
	logger & operator=(logger &&other) = delete;
	
	
	// Returns current log name
	inline const std::string & log_name() const noexcept;
	
	
	template<class Level>
	inline stream<Level> stream(const Level &level) noexcept;
	
	
	template<int Level>
	inline void log_raw(const level::regular<Level> &, const std::string &data) noexcept;
	
	template<int Level>
	inline void log_raw(const level::security<Level> &, const std::string &data) noexcept;
private:
	void start_log() noexcept;
	
	
	// Data
	std::string log_name_;
};	// class logger



class enable_logger
{
public:
	inline enable_logger(const std::string &log_name);
	inline enable_logger(std::string &&log_name) noexcept;
	
	enable_logger(const enable_logger &other) = delete;
	enable_logger(enable_logger &&other) = delete;
	
	enable_logger & operator=(const enable_logger &other) = delete;
	enable_logger & operator=(enable_logger &&other) = delete;
	
	
	// Returns reference to logger object
	inline class logger & logger() const noexcept;
private:
	// Data
	mutable class logger logger_;
};	// class enable_logger



class enable_logger_ref
{
public:
	inline enable_logger_ref(class logger &logger) noexcept;
	
	
	enable_logger_ref(const enable_logger_ref &other) = default;
	enable_logger_ref(enable_logger_ref &&other) = default;
	
	enable_logger_ref & operator=(const enable_logger_ref &other) = default;
	enable_logger_ref & operator=(enable_logger_ref &&other) = default;
	
	
	// Returns reference to logger object
	inline class logger & logger() const noexcept;
private:
	// Data
	class logger *logger_ptr_;
};	// class enable_logger_ref



template<class Level>
class stream:
	public std::ostringstream,
	protected enable_logger_ref
{
public:
	inline stream(class logger &logger, const Level &level) noexcept;
	~stream();
	
	stream(stream &&other) = default;
	stream & operator=(stream &&other) = default;
	
	// Non-copy constructible/assignable
	stream(const stream &other) = delete;
	stream & operator=(const stream &other) = delete;
private:
	// Data
	const Level *level_ptr_;
};	// class stream


};	// namespace logger


#include <logger/logger.hpp>

#endif	// LOGGER_LOGGER_H
