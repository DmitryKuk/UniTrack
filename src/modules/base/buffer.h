// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef BASE_BUFFER_H
#define BASE_BUFFER_H

#include <vector>
#include <functional>

#include <boost/asio/buffer.hpp>


namespace base {


// Send buffers
typedef boost::asio::const_buffer		send_buffer_type;
typedef std::vector<send_buffer_type>	send_buffers_type;


using boost::asio::buffer;
using boost::asio::buffer_cast;
using boost::asio::buffer_size;


class send_buffers_insert_functor
{
public:
	inline send_buffers_insert_functor(base::send_buffers_type &buffers);
	
	send_buffers_insert_functor(const send_buffers_insert_functor &other) = default;
	send_buffers_insert_functor(send_buffers_insert_functor &&other) = default;
	
	send_buffers_insert_functor & operator=(const send_buffers_insert_functor &other) = default;
	send_buffers_insert_functor & operator=(send_buffers_insert_functor &&other) = default;
	
	
	template<class... Args>
	size_t operator()(const Args &... args) const;
private:
	base::send_buffers_type *buffers_ptr_;
};	// class send_buffers_insert_functor


};	// namespace base


#include <base/buffer.hpp>

#endif	// BASE_BUFFER_H
