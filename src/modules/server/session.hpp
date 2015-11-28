// Author: Dmitry Kukovinets (d1021976@gmail.com)


inline
const boost::asio::ip::address &
server::session::client_address() const noexcept
{
	return this->client_address_;
}


inline
server::port_type
server::session::server_port() const noexcept
{
	return this->server_port_;
}


inline
void
server::session::handle_error(const std::exception &e, const ::server::protocol::http::status &status)
{
	this->handle_error(e.what(), status);
}
