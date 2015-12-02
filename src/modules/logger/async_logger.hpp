// Author: Dmitry Kukovinets (d1021976@gmail.com)


inline
bool
logger::async_logger::colorize_output() const noexcept
{
	return this->parameters_.colorize_output;
}


inline
size_t
logger::async_logger::max_queue_size() const noexcept
{
	return this->parameters_.max_queue_size;
}
