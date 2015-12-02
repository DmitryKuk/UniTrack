// Author: Dmitry Kukovinets (d1021976@gmail.com)


// Returns reference to error host object
inline
const ::server::host::base &
server::host::manager::error_host() const noexcept
{
	return this->error_host_;
}
