// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logger/record_object.h>

#include <ctime>

#include <logger/logger.h>


// static
const std::regex logger::record_object::regex_r_{"\r", std::regex::optimize};
const std::string logger::record_object::replace_r_by_{""};


logger::record_object::record_object(::logger::logger &logger, ::logger::level level) noexcept:
	logger_ptr_{&logger},
	
	level_{level}
{
	// Printing timestamp in format: DD.MM.YYYY hh:mm:ss
	std::time_t raw_time;
	std::time(&raw_time);
	
	auto *time_info = std::localtime(&raw_time);
	char time_str[23];
	std::strftime(time_str, 23, " %d.%m.%Y %H:%M:%S  ", time_info);
	
	*this << time_str;
}


logger::record_object::record_object(::logger::record_object &&other) noexcept:
	logger_ptr_{other.logger_ptr_},
	
	level_{other.level_},
	stream_{std::move(other.stream_)}
{
	other.logger_ptr_ = nullptr;
}


logger::record_object::~record_object()
{
	if (this->logger_ptr_)
		this->logger_ptr_->log_raw(this->level_, this->stream_.str());
}


logger::record_object &
logger::record_object::operator=(::logger::record_object &&other) noexcept
{
	this->logger_ptr_ = other.logger_ptr_;
	this->level_ = other.level_;
	try {
		this->stream_ = std::move(other.stream_);	// This should not throw
	} catch (...) {}
	
	other.logger_ptr_ = nullptr;
	
	return *this;
}
