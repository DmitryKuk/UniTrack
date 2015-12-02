// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGGER_ENABLE_LOGGER_IMPL_H
#define LOGGER_ENABLE_LOGGER_IMPL_H


namespace logger {


template<class Logger>
class enable_logger_impl
{
public:
	template<class... LoggerArgs>
	inline enable_logger_impl(LoggerArgs &&... logger_args) noexcept;
	
	enable_logger_impl(const enable_logger_impl &other) = default;
	enable_logger_impl(enable_logger_impl &&other) = default;
	
	enable_logger_impl & operator=(const enable_logger_impl &other) = default;
	enable_logger_impl & operator=(enable_logger_impl &&other) = default;
	
	
	// Returns reference to logger object
	inline Logger & logger() noexcept;
	
	// Resets logger object. After this call this object can't be used for logging!
	inline void reset_logger() noexcept;
	
	// Returns true, if this object is correct for logging. Otherwise returns false
	inline bool check_logger() const noexcept;
private:
	Logger logger_;
};	// class enable_logger_impl


};	// namespace logger


#include <logger/enable_logger_impl.hpp>

#endif	// LOGGER_ENABLE_LOGGER_IMPL_H
