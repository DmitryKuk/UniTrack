// Author: Dmitry Kukovinets (d1021976@gmail.com)


template<class T>
inline
logger::record_object &
logger::record_object::operator<<(T data) noexcept
{
	try {
		this->stream_ << data;
	} catch (...) {}
	return *this;
}


template<>
inline
logger::record_object &
logger::record_object::operator<<(const std::string &data) noexcept
{
	try {
		this->stream_ << std::regex_replace(data, this->regex_r_, this->replace_r_by_);
	} catch (...) {}
	return *this;
}


template<>
inline
logger::record_object &
logger::record_object::operator<<(char const * const data) noexcept
{
	try {
		this->stream_ << std::regex_replace(data, this->regex_r_, this->replace_r_by_);
	} catch (...) {}
	return *this;
}


template<>
inline
logger::record_object &
logger::record_object::operator<<(char data) noexcept
{
	if (data != '\r')
		try {
			this->stream_ << data;
		} catch (...) {}
	return *this;
}
