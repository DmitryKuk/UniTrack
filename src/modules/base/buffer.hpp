// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <memory>


inline
base::send_buffers_insert_functor::send_buffers_insert_functor(base::send_buffers_type &buffers):
	buffers_ptr_(std::addressof(buffers))
{}


template<class... Args>
size_t
base::send_buffers_insert_functor::operator()(const Args &... args) const
{
	auto buffer = base::buffer(args...);
	this->buffers_ptr_->push_back(buffer);
	return base::buffer_size(buffer);
}
