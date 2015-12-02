// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_ACCEPTOR_H
#define SERVER_ACCEPTOR_H

#include <boost/asio/ip/tcp.hpp>

#include <logger/logger.h>
#include <server/types.h>


namespace server {


class worker;


class acceptor:
	protected boost::asio::ip::tcp::acceptor
{
public:
	acceptor(worker &worker, ::server::port_type port);
	~acceptor();
	
	acceptor(acceptor &&other) = default;
	acceptor & operator=(acceptor &&other) = default;
	
	acceptor(const acceptor &other) = delete;
	acceptor & operator=(const acceptor &other) = delete;
private:
	// Handlers
	// Add accept_handler to the io_service event loop
	void add_accept_handler();
	
	
	// Handles the accept event
	void accept_handler(const boost::system::error_code &err);
	
	
	// Data
	worker *worker_ptr_;
	
	::server::socket socket_;
};


};	// namespace server


#endif	// SERVER_ACCEPTOR_H
