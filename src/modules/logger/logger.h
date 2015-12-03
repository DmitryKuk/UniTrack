// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGGER_LOGGER_H
#define LOGGER_LOGGER_H

#include <sstream>

#include <logger/level.h>


namespace logger {


class global_instance
{
public:
	global_instance(const std::string &log_name);
	global_instance(std::string &&log_name) noexcept;
	~global_instance();
	
	
	// Non-copy/-move constructible/assignable. Use ptrs.
	global_instance(const global_instance &other) = delete;
	global_instance(global_instance &&other) = delete;
	
	global_instance & operator=(const global_instance &other) = delete;
	global_instance & operator=(global_instance &&other) = delete;
	
	
	// Returns current log name
	inline const std::string & log_name() const noexcept;
private:
	void start_log() noexcept;
	
	
	// Data
	std::string log_name_;
};	// class global_instance



template<class Level>
class log_stream:
	public std::ostringstream
{
public:
	inline log_stream(const Level &level) noexcept;
	~log_stream();
	
	log_stream(log_stream &&other) = default;
	log_stream & operator=(log_stream &&other) = default;
	
	// Non-copy constructible/assignable
	log_stream(const log_stream &other) = delete;
	log_stream & operator=(const log_stream &other) = delete;
private:
	// Data
	const Level *level_ptr_;
};	// class log_stream



// Global functions
template<class Level>
inline log_stream<Level> stream(const Level &level) noexcept;


template<int Level>
inline void log(const level::regular<Level> &, const std::string &data) noexcept;

template<int Level>
inline void log(const level::security<Level> &, const std::string &data) noexcept;


};	// namespace logger


#include <logger/logger.hpp>

#endif	// LOGGER_LOGGER_H
