// Author: Dmitry Kukovinets (d1021976@gmail.com)


// Same as request.process_stream(stream)
inline
std::istream &
server::protocol::http::operator>>(std::istream &stream, ::server::protocol::http::request &request)
{
	request.process_stream(stream);
	return stream;
}
