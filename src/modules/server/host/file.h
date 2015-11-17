// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_HOST_FILE_H
#define SERVER_HOST_FILE_H

#include <boost/filesystem/path.hpp>

#include <base/buffer.h>

#include <logger/logger.h>

#include <server/types.h>
#include <server/host/base.h>
#include <server/protocol/http.h>


namespace server {
namespace host {


// Requirements to the class HostType:
// 	- may have class inside:
// 		class cache (even it is empty), that have a default constructor
// 		
// 		Then you can leave CacheType template parameter as is. If not (for example, HostType
// 		is pointer to function or lambda), try to set CacheType to server::host_cache. But then
// 		HostType object will be unable to use own cache.
// 	- must have non-static method:
// 		
// 		std::pair<base::send_buffers_t, base::send_buffers_t>
// 		operator()(const file_host<HostType, CacheType> &host,	// Host, that is handler's owner
// 				   CacheType &cache);							// Cache that the handler can use
// 		
// 		Return value: first if headers buffers, second is content buffers.
// 		Headers buffers must contain Content-Length, if need.
// 		This method can throws: server::path_forbidden or server::path_not_found.
// 		In this case status will be 403 Forbidden and 404 Not Found.
// 		Other exceptions will be processed with status 500 Internal Server Error.
// 		
// 		This method can use cache pointed by cache_ptr and must return buffers
// 		ready to socket.async_send(). All data is in that cache.
// 
// See also:
// - base/buffer.h and server/types.h for base::send_buffers_t and server::file_host_cache<>::ptr_t

// Requirements to class CacheType:
// 	- must be inheritor of server::host_cache
// 	- must have typedef inside: ptr_t


// NOTE: If you don't know what is these, and what should you do, simple files-only host example:
// 		server::parameters params;
// 		// Set parameters with: params.smth = smth_val;
// 		server::file_host<server::files_only> host(logger, params);


template<class HostType>
class file_host:
	public server::host::base
{
public:
	struct only_parameters
	{
		// May be useful, if several allow_regexes specified.
		enum class allow_match_mode
		{
			any,	// Uri matches any of allow_regexes (and doesn't match any of deny_regexes).
			all		// Uri matches all of allow_regexes (and doesn't match any of deny_regexes).
		};
		
		
		boost::filesystem::path root;			// Required
		
		std::vector<std::regex>
			deny_regexes =						// Optional
				{
					std::regex(".*\\.\\./.*"),	// Don't allow "../" sequences!
					std::regex(".*/\\..*")		// Don't allow hidden directories and files
				},
			allow_regexes =						// Optional
				{
					// Don't allow anything by default
				};
		
		allow_match_mode mode					// Optional
			= allow_match_mode::all;
		
		
		explicit only_parameters() = default;
		explicit only_parameters(const nlohmann::json &config);
	};	// struct only_parameters
	
	
	struct parameters:
		public server::host::base::parameters,
		public only_parameters
	{
		using only_parameters::allow_match_mode;
		
		
		explicit parameters() = default;
		
		explicit parameters(const nlohmann::json &config):
			server::host::base::parameters(config),
			only_parameters(config)
		{}
	};	// struct parameters
	
	
	
	file_host(logger::logger &logger,
			  const parameters &parameters,
			  HostType &&handler = std::move(HostType()));
	
	file_host(logger::logger &logger,
			  const parameters &parameters,
			  const HostType &handler);
	
	
	// Non-copy/-move constructable/assignable. Use ptrs.
	template<class HostType1, class CacheType1>
	file_host(const file_host<HostType1, CacheType1> &other) = delete;
	
	template<class HostType1, class CacheType1>
	file_host(file_host<HostType1, CacheType1> &&other) = delete;
	
	template<class HostType1, class CacheType1>
	file_host<HostType, CacheType> &
	operator=(const file_host<HostType1, CacheType1> &other) = delete;
	
	template<class HostType1, class CacheType1>
	file_host<HostType, CacheType> &
	operator=(file_host<HostType1, CacheType1> &&other) = delete;
	
	
	// Prepares a correct response to the client.
	// Returns pair<vector<buffer>, shared_ptr<cache>> ready to socket.async_send().
	// WARNING: first field of result does NOT contain data, only references. Second field
	// contains data need to be sent, so save the given shared_ptr anywhere during all sending!
	virtual
	server::protocol::http::response::ptr_type
	response(server::protocol::http::request::ptr_type request_ptr) override;
	
	
	// URI parsing
	bool parse_uri(const std::string &uri, server::host_cache &cache);
protected:
	bool validate_path(const std::string &path) const noexcept;
	bool validate_args(const server::uri_arguments_map_t &args_map,
					   const server::uri_arguments_set_t &args_set) const noexcept;
	bool validate_method(server::http::method method) const noexcept;
	
	
	server::response_data_t
	log_and_phony_response(const std::string &message,
						   server::http::version version,
						   server::file_host<HostType, CacheType>::cache_shared_ptr_t cache_ptr,
						   const server::http::status &status);
	
	
	server::response_data_t
	handle_filesystem_error(const boost::filesystem::filesystem_error &e,
							server::http::version version,
							server::file_host<HostType, CacheType>::cache_shared_ptr_t cache_ptr,
							const server::http::status &status);
private:
	server::response_data_t
	response(server::file_host<HostType, CacheType>::cache_shared_ptr_t &&cache_ptr,
			 server::http::method method,
			 server::http::version version,
			 server::headers_t &&request_headers);
	
	
	// Data
	only_parameters parameters_;
	
	HostType handler_;
};	// class file_host


};	// namespace host
};	// namespace server


#include <server/file_host.hpp>

#endif	// SERVER_HOST_FILE_H
