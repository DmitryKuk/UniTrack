// Author: Dmitry Kukovinets (d1021976@gmail.com)


inline
server::protocol::request::request(const boost::asio::ip::address &client_address):
	client_address(client_address)
{}
