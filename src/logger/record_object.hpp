// Author: Dmitry Kukovinets (d1021976@gmail.com)


template<class T>
inline
logger::record_object &
logger::record_object::operator<<(T data)
{
	this->stream_ << data;
	return *this;
}


template<>
inline
logger::record_object &
logger::record_object::operator<<(const std::string &data)
{
	this->stream_ << std::regex_replace(data, this->regex_r_, this->replace_r_by_);
	return *this;
}


template<>
inline
logger::record_object &
logger::record_object::operator<<(std::string &&data)
{
	this->stream_ << std::regex_replace(data, this->regex_r_, this->replace_r_by_);
	return *this;
}


template<>
inline
logger::record_object &
logger::record_object::operator<<(char const * const data)
{
	this->stream_ << std::regex_replace(data, this->regex_r_, this->replace_r_by_);
	return *this;
}


template<>
inline
logger::record_object &
logger::record_object::operator<<(char *data)
{
	this->stream_ << std::regex_replace(data, this->regex_r_, this->replace_r_by_);
	return *this;
}
