// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logger/logger.h>


// class server::host::base
inline
const std::string &
server::host::base::name() const noexcept
{
	return this->name_;
}


inline
std::unique_ptr<::server::protocol::http::response>
server::host::base::handle_error(const worker &worker,
								 const ::server::protocol::http::request &request,
								 const std::exception &e,
								 const ::server::protocol::http::status &status) const
{
	return this->handle_error(worker, request, e.what(), status);
}
