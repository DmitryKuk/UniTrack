// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_TYPES_H
#define SERVER_TYPES_H

#include <memory>
#include <unordered_set>

#include <boost/asio/ip/tcp.hpp>


namespace server {


// Sockets
typedef std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr_type;


// Ports
typedef unsigned short					port_type;
typedef std::unordered_set<port_type>	port_set_type;


};	// namespace server


#endif	// SERVER_TYPES_H
