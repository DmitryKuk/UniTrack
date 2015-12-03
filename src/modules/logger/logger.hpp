// Author: Dmitry Kukovinets (d1021976@gmail.com)


// class logger::logger
// Returns current log name
inline
const std::string &
logger::logger::log_name() const noexcept
{
	return this->log_name_;
}


template<class Level>
inline
logger::stream<Level>
logger::logger::stream(const Level &level) noexcept
{
	return {*this, level};
}


template<int Level>
inline
void
logger::logger::log_raw(const ::logger::level::regular<Level> &, const std::string &data) noexcept
{
	::syslog(::logger::level::regular<Level>::value, "%s", data.c_str());
}


template<int Level>
inline
void
logger::logger::log_raw(const ::logger::level::security<Level> &, const std::string &data) noexcept
{
	::syslog(::logger::level::security<Level>::value, "<Security>: %s", data.c_str());
}



// class logger::enable_logger
inline
logger::enable_logger::enable_logger(const std::string &log_name):
	logger_{log_name}
{}


inline
logger::enable_logger::enable_logger(std::string &&log_name) noexcept:
	logger_{std::move(log_name)}
{}


inline
logger::logger &
logger::enable_logger::logger() const noexcept
{
	return this->logger_;
}



// class logger::enable_logger_ref
inline
logger::enable_logger_ref::enable_logger_ref(::logger::logger &logger) noexcept:
	logger_ptr_{&logger}
{}


inline
logger::logger &
logger::enable_logger_ref::logger() const noexcept
{
	return *this->logger_ptr_;
}



// class logger::stream
template<class Level>
inline
logger::stream<Level>::stream(::logger::logger &logger, const Level &level) noexcept:
	::logger::enable_logger_ref{logger},
	
	level_ptr_{&level}
{}


template<class Level>
logger::stream<Level>::~stream()
{
	auto str = this->str();
	if (!str.empty())
		this->logger().log_raw(*this->level_ptr_, str);
}
