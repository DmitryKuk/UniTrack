// Author: Dmitry Kukovinets (d1021976@gmail.com)


inline
unsigned int
server::protocol::http::status::code() const noexcept
{
	return this->code_;
}


inline
const std::string &
server::protocol::http::status::code_str() const noexcept
{
	return this->code_str_;
}


inline
const std::string &
server::protocol::http::status::description() const noexcept
{
	return this->description_;
}


inline
const std::string &
server::protocol::http::status::start_string() const noexcept
{
	return this->start_string_;
}
