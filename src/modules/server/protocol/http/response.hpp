// Author: Dmitry Kukovinets (d1021976@gmail.com)


inline
server::protocol::http::response::response(const ::server::protocol::http::status &status,
										   ::server::protocol::http::version version)
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
	using namespace ::server::protocol::http;
	
	this->buffers.push_back(buffer(str::newline));
}


inline
void
server::protocol::http::response::add_body(const base::send_buffer_type &buffer)
{
	this->buffers.push_back(buffer);
}


// Returns index of send buffer, that points to value of last added header
// WARNING: Call this method immidiately after add_header or add_headers called!
//          Otherwise, trash value will be returned!
inline
size_t
server::protocol::http::response::header_value_index() const noexcept
{
	return this->buffers.size() - 2;	// Index of buffer, that points to last header's value
}
