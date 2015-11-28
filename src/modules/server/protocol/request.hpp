// Author: Dmitry Kukovinets (d1021976@gmail.com)


inline
server::protocol::request::request(const boost::asio::ip::address &client_address):
	client_address(client_address)
{}


// Prepares request for new data from same client
// Resets buffer and keep_alive, saving client_address
inline
void
server::protocol::request::reset() noexcept
{
	this->keep_alive = false;
}
