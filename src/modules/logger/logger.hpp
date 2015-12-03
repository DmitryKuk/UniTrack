// Author: Dmitry Kukovinets (d1021976@gmail.com)


// class logger::global_instance
// Returns current log name
inline
const std::string &
logger::global_instance::log_name() const noexcept
{
	return this->log_name_;
}



// class logger::log_stream
template<class Level>
inline
logger::log_stream<Level>::log_stream(const Level &level) noexcept:
	level_ptr_{&level}
{}


template<class Level>
logger::log_stream<Level>::~log_stream()
{
	auto str = this->str();
	if (!str.empty())
		logger::log(*this->level_ptr_, str);
}



// Global functions
template<class Level>
inline
logger::log_stream<Level>
logger::stream(const Level &level) noexcept
{
	return {level};
}


template<int Level>
inline
void
logger::log(const logger::level::regular<Level> &, const std::string &data) noexcept
{
	::syslog(logger::level::regular<Level>::value, "%s", data.c_str());
}


template<int Level>
inline
void
logger::log(const logger::level::security<Level> &, const std::string &data) noexcept
{
	::syslog(logger::level::security<Level>::value, "<Security>: %s", data.c_str());
}
