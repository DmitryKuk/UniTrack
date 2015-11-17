// Author: Dmitry Kukovinets (d1021976@gmail.com)


inline
const boost::asio::ip::address &
server::client_manager::client_address() const noexcept
{
	return this->client_address_;
}


inline
server::port_t
server::client_manager::server_port() const noexcept
{
	return this->server_port_;
}


inline
bool
server::client_manager::keep_alive() const noexcept
{
	return this->keep_alive_;
}


inline
void
server::client_manager::keep_alive(bool status) noexcept
{
	this->keep_alive_ = status;
}


// protected
inline
void
server::client_manager::handle_error(server::client_manager::request_data_ptr_t request_data_ptr,
									 const std::exception &e,
									 const server::http::status &status,
									 bool exit,
									 bool send_phony,
									 std::unordered_map<std::string, std::string> &&headers)
{
	this->handle_error(request_data_ptr, e.what(), status, exit, send_phony, std::move(headers));
}
