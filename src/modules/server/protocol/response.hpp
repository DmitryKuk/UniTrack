// Author: Dmitry Kukovinets (d1021976@gmail.com)


// Constructs buffers_ with given size. Don't forget to fill!
inline
server::protocol::response::response(size_t buffers_size):
	buffers_{buffers_size}
{}


inline
const base::send_buffers_type &
server::protocol::response::buffers() const noexcept
{
	return this->buffers_;
}


inline
base::send_buffers_type &
server::protocol::response::buffers() noexcept
{
	return this->buffers_;
}
