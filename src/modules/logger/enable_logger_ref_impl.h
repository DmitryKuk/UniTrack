// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGGER_ENABLE_LOGGER_REF_IMPL_H
#define LOGGER_ENABLE_LOGGER_REF_IMPL_H


namespace logger {


template<class Logger>
class enable_logger_ref_impl
{
public:
	inline enable_logger_ref_impl(Logger &logger) noexcept;
	
	enable_logger_ref_impl(const enable_logger_ref_impl &other) = default;
	enable_logger_ref_impl(enable_logger_ref_impl &&other) = default;
	
	enable_logger_ref_impl & operator=(const enable_logger_ref_impl &other) = default;
	enable_logger_ref_impl & operator=(enable_logger_ref_impl &&other) = default;
	
	
	// Returns reference to logger object
	inline Logger & logger() const noexcept;
	
	// Resets logger object. After this call this object can't be used for logging!
	inline void reset_logger() noexcept;
	
	// Returns true, if this object is correct for logging. Otherwise returns false
	inline bool check_logger() const noexcept;
private:
	Logger *logger_ptr_;
};	// class enable_logger_ref_impl


};	// namespace logger


#include <logger/enable_logger_ref_impl.hpp>

#endif	// LOGGER_ENABLE_LOGGER_REF_IMPL_H
