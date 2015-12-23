// Author: Dmitry Kukovinets (d1021976@gmail.com)


// Returns status of execution.
inline
int
application::status() const noexcept
{
	return this->status_;
}


inline
logic::global_instance &
application::logic()
{
	return *this->logic_ptr_;
}
