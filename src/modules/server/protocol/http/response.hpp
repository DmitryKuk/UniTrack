// Author: Dmitry Kukovinets (d1021976@gmail.com)


inline
server::protocol::http::response::response(const server::protocol::http::status &status,
										   server::protocol::http::version version):
{
	this->add_start_string(status, version);
}


template<class Headers>
void
server::protocol::http::response::add_headers(const Headers &headers)
{
	this->buffers.reserve(buffers.size() + 4 * headers.size());
	
	for (const auto &header: headers)
		this->add_header(header.first, header.second);
}


inline
void
server::protocol::http::response::finish_headers()
{
	using base::buffer;
	using namespace server::protocol::http;
	
	this->buffers.push_back(buffer(str::newline));
}


template<class T>
inline
void
server::protocol::http::response::add_body(const T &data)
{
	using base::buffer;
	
	this->add_body(buffer(data));
}


template<>
inline
void
server::protocol::http::response::add_body(const base::send_buffer_type &buffer)
{
	this->buffers.push_back(buffer);
}
