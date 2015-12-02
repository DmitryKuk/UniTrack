// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <functional>


template<class Logger>
template<class... LoggerArgs>
inline
logger::enable_logger_impl<Logger>::enable_logger_impl(LoggerArgs &&... logger_args) noexcept:
	logger_{std::forward<LoggerArgs>(logger_args)...}
{}


// Returns reference to logger object
template<class Logger>
inline
Logger &
logger::enable_logger_impl<Logger>::logger() noexcept
{
	return this->logger_;
}


// Resets logger object. After this call this object can't be used for logging!
template<class Logger>
inline
void
logger::enable_logger_impl<Logger>::reset_logger() noexcept
{}


// Returns true, if this object is correct for logging. Otherwise returns false
template<class Logger>
inline
bool
logger::enable_logger_impl<Logger>::check_logger() const noexcept
{
	return true;
}
