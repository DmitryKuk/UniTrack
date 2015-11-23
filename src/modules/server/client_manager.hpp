// Author: Dmitry Kukovinets (d1021976@gmail.com)


inline
const boost::asio::ip::address &
server::client_manager::client_address() const noexcept
{
	return this->client_address_;
}


inline
server::port_type
server::client_manager::server_port() const noexcept
{
	return this->server_port_;
}


inline
void
server::client_manager::handle_error(const server::protocol::http::request &request,
									 const std::exception &e,
									 const server::protocol::http::status &status)
{
	this->handle_error(request, e.what(), status);
}
