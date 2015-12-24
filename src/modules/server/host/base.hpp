// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logger/logger.h>

#include <memory>


// class server::host::base
inline
const std::string &
server::host::base::name() const noexcept
{
	return this->name_;
}


// Prepares a response with body.
// If empty content_type string given, Content-Type header will not be added.
template<class String>
std::unique_ptr<::server::protocol::http::response>
server::host::base::response_with_body(const ::server::worker &worker,
									   const ::server::protocol::http::request &request,
									   const ::server::protocol::http::status &status,
									   String &&response_body,
									   bool need_cache,
									   const std::string &content_type) const
{
	using namespace ::server::protocol::http;
	
	auto response_ptr = std::make_unique<::server::protocol::http::response>(status, request.version);
	::server::host::base::add_server_name(worker, *response_ptr);
	
	::base::send_buffer_type buffer;
	if (need_cache)	buffer = ::base::buffer(response_ptr->cache(std::forward<String>(response_body)));
	else			buffer = ::base::buffer(std::forward<String>(response_body));
	
	// Content-Type header
	if (!content_type.empty())
		response_ptr->add_header(header::content_type, content_type);
	
	response_ptr->add_header(header::content_length, std::to_string(::base::buffer_size(buffer)));
	
	if (request.method != method::HEAD)
		response_ptr->add_body(std::move(buffer));
	
	return std::move(response_ptr);
}


// Prepares a response with html body.
template<class String>
inline
std::unique_ptr<::server::protocol::http::response>
server::host::base::response_with_html_body(const ::server::worker &worker,
											const ::server::protocol::http::request &request,
											const ::server::protocol::http::status &status,
											String &&response_body,
											bool need_cache) const
{
	return this->response_with_body(worker, request, status,
									std::forward<String>(response_body), need_cache,
									::server::protocol::http::content_type::text_html);
}


// Prepares a response with json body.
template<class String>
inline
std::unique_ptr<::server::protocol::http::response>
server::host::base::response_with_json_body(const ::server::worker &worker,
											const ::server::protocol::http::request &request,
											const ::server::protocol::http::status &status,
											String &&response_body,
											bool need_cache) const
{
	return this->response_with_body(worker, request, status,
									std::forward<String>(response_body), need_cache,
									::server::protocol::http::content_type::application_json);
}


template<class WhatArgs>
inline
void
server::host::base::log_error(const ::server::worker &worker,
							  const ::server::protocol::http::request &request,
							  const WhatArgs &what) const
{
	using namespace std::literals;
	logger::stream(logger::level::error)
		<< "Client: "s << request.client_address
		<< ": Host: \""s << this->name()
		<< "\": "s << what << '.';
}


template<class WhatArgs>
inline
void
server::host::base::log_error(const ::server::worker &worker,
							  const ::server::protocol::http::request &request,
							  const WhatArgs &what,
							  const ::server::protocol::http::status &status) const
{
	using namespace std::literals;
	logger::stream(logger::level::error)
		<< "Client: "s << request.client_address
		<< ": Host: \""s << this->name()
		<< "\": "s << what
		<< " => "s << status.code() << '.';
}


inline
std::unique_ptr<::server::protocol::http::response>
server::host::base::handle_error(const ::server::worker &worker,
								 const ::server::protocol::http::request &request,
								 const std::exception &e,
								 const ::server::protocol::http::status &status) const
{
	return this->handle_error(worker, request, e.what(), status);
}
