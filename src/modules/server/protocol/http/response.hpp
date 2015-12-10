// Author: Dmitry Kukovinets (d1021976@gmail.com)


inline
server::protocol::http::response::response(const ::server::protocol::http::status &status,
										   ::server::protocol::http::version version):
	::server::protocol::response{1}	// Reserved for headers
{
	this->add_start_string(status, version);
}


inline
server::protocol::http::response::response():
	::server::protocol::response{1}	// Reserved for headers
{}


inline
void
server::protocol::http::response::add_start_string(const ::server::protocol::http::status &status,
												   ::server::protocol::http::version version)
{
	using namespace ::server::protocol::http;
	
	this->headers_ += str::HTTP;
	this->headers_ += str::slash;
	this->headers_ += version_to_str(version);
	this->headers_ += status.start_string();	// Contains "\r\n"
}


template<class String1, class String2>
inline
void
server::protocol::http::response::add_header(String1 &&name, String2 &&value)
{
	using namespace ::server::protocol::http;
	
	this->headers_ += std::forward<String1>(name);
	this->headers_ += str::header_separator;
	this->headers_ += std::forward<String2>(value);
	this->headers_ += str::newline;
}


template<class Headers>
void
server::protocol::http::response::add_headers(const Headers &headers)
{
	for (const auto &header: headers)
		this->add_header(header.first, header.second);
}


inline
void
server::protocol::http::response::add_body(const base::send_buffer_type &buffer)
{
	this->buffers().push_back(buffer);
}


inline
void
server::protocol::http::response::finish()
{
	using namespace ::server::protocol::http;
	
	this->headers_ += str::newline;
	this->buffers()[0] = base::buffer(this->headers_);
}
