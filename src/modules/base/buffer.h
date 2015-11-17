// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef BASE_BUFFER_H
#define BASE_BUFFER_H

#include <vector>
#include <iterator>

#include <boost/asio/buffer.hpp>
#include <boost/asio/streambuf.hpp>


namespace base {


// Send buffers
typedef boost::asio::const_buffer						send_buffer_type;
typedef std::vector<send_buffer_type>					send_buffers_type;
typedef std::back_insert_iterator<send_buffers_type>	send_buffers_insert_iterator_type;


using boost::asio::buffer;
using boost::asio::buffer_cast;
using boost::asio::buffer_size;

using boost::asio::streambuf;


};	// namespace base


#endif	// BASE_BUFFER_H
