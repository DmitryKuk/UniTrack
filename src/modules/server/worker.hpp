// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/server.h>


boost::asio::io_service &
server::worker::io_service() noexcept
{
	return this->io_service_;
}


inline
::server::host::manager &
server::worker::host_manager() noexcept
{
	return this->server_.host_manager();
}
