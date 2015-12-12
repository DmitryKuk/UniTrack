// Author: Dmitry Kukovinets (d1021976@gmail.com)


inline
server::protocol::http::response::response(const ::server::protocol::http::status &status,
										   ::server::protocol::http::version version):
	::server::protocol::response{1}	// Reserved for headers
{
	using namespace ::server::protocol::http;
	
	this->headers_ += str::HTTP;
	this->headers_ += str::slash;
	this->headers_ += version_to_str(version);
	this->headers_ += status.start_string();	// Contains "\r\n"
	
	this->update_headers_buffer();
}


template<class String1, class String2>
inline
void
server::protocol::http::response::add_header(String1 &&name, String2 &&value)
{
	using namespace ::server::protocol::http;
	
	this->prepare_update_headers_buffer();
	
	this->headers_ += std::forward<String1>(name);
	this->headers_ += str::header_separator;
	this->headers_ += std::forward<String2>(value);
	this->headers_ += str::newline;
	
	this->update_headers_buffer();
}


inline
void
server::protocol::http::response::add_body(const base::send_buffer_type &buffer)
{
	this->buffers().push_back(buffer);
}


// private
inline
void
server::protocol::http::response::prepare_update_headers_buffer() noexcept
{
	this->headers_.erase(this->headers_.size() - 2);	// Erase old last "\r\n"
}


inline
void
server::protocol::http::response::update_headers_buffer() noexcept
{
	this->headers_ += str::newline;
	this->buffers()[0] = base::buffer(this->headers_);
}
