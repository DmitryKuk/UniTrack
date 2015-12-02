// Author: Dmitry Kukovinets (d1021976@gmail.com)


template<class Logger>
inline
logger::enable_logger_ref_impl<Logger>::enable_logger_ref_impl(Logger &logger) noexcept:
	logger_ptr_{&logger}
{}


// Returns reference to logger object
template<class Logger>
inline
Logger &
logger::enable_logger_ref_impl<Logger>::logger() const noexcept
{
	return *this->logger_ptr_;
}


// Resets logger object. After this call this object can't be used for logging!
template<class Logger>
inline
void
logger::enable_logger_ref_impl<Logger>::reset_logger() noexcept
{
	this->logger_ptr_ = nullptr;
}


// Returns true, if this object is correct for logging. Otherwise returns false
template<class Logger>
inline
bool
logger::enable_logger_ref_impl<Logger>::check_logger() const noexcept
{
	return (this->logger_ptr_ == nullptr)? false: true;
}
