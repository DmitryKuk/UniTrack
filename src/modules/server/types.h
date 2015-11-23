// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_TYPES_H
#define SERVER_TYPES_H

#include <memory>
#include <unordered_set>

#include <boost/asio/ip/tcp.hpp>


namespace server {


// Sockets
using socket			= boost::asio::ip::tcp::socket;
using socket_ptr_type	= std::shared_ptr<socket>;


// Ports
using port_type			= unsigned short;
using port_set_type		= std::unordered_set<port_type>;


};	// namespace server


#endif	// SERVER_TYPES_H
