// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_HOST_FILE_H
#define SERVER_HOST_FILE_H

#include <boost/filesystem/path.hpp>

#include <base/buffer.h>

#include <logger/logger.h>

#include <server/types.h>
#include <server/host/base.h>
#include <server/protocol/http.h>
#include <server/worker.h>


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
// 		operator()(const file<HostType> &host,	// Host, that is handler's owner
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
// 		server::file<server::files_only> host(logger, params);


template<class HostType>
class file:
	public server::host::base
{
public:
	struct current_parameters
	{
		// May be useful, if several allow_regexes specified.
		enum class allow_match_mode
		{
			any,	// Path matches any of allow_regexes (and doesn't match any of deny_regexes).
			all		// Path matches all of allow_regexes (and doesn't match any of deny_regexes).
		};	// enum class allow_match_mode
		
		
		boost::filesystem::path root;						// Required
		
		// Set to "index.html" for standard behavior. Empty value denies searching, if directory path requested
		boost::filesystem::path default_index_file = "";	// Optional
		
		std::vector<std::regex>
			deny_regexes =									// Optional
				{
					std::regex(".*\\.\\./.*"),	// Don't allow "../" sequences!
					std::regex(".*/\\..*")		// Don't allow hidden directories and files
				},
			allow_regexes =									// Optional
				{
					// Don't allow anything by default
				};
		
		allow_match_mode mode = allow_match_mode::all;		// Optional
		
		
		
		explicit current_parameters() = default;
		explicit current_parameters(const nlohmann::json &config);
	};	// struct current_parameters
	
	
	struct parameters:
		public server::host::base::parameters,
		public current_parameters
	{
		using current_parameters::allow_match_mode;
		
		
		explicit parameters() = default;
		
		explicit parameters(const nlohmann::json &config):
			server::host::base::parameters(config),
			current_parameters(config)
		{}
	};	// struct parameters
	
	
	
	file(logger::logger &logger,
			  const parameters &parameters,
			  HostType &&handler = std::move(HostType()));
	
	file(logger::logger &logger,
			  const parameters &parameters,
			  const HostType &handler);
	
	
	// Non-copy/-move constructable/assignable. Use ptrs.
	template<class HostType1>
	file(const file<HostType1> &other) = delete;
	
	template<class HostType1>
	file(file<HostType1> &&other) = delete;
	
	template<class HostType1>
	file<HostType> & operator=(const file<HostType1> &other) = delete;
	
	template<class HostType1>
	file<HostType> & operator=(file<HostType1> &&other) = delete;
	
	
	// Prepares a correct response to the client.
	// Returns pair<vector<buffer>, shared_ptr<cache>> ready to socket.async_send().
	// WARNING: first field of result does NOT contain data, only references. Second field
	// contains data need to be sent, so save the given shared_ptr anywhere during all sending!
	virtual
	server::protocol::http::response::ptr_type
	response(const server::worker &worker,
			 server::protocol::http::request::ptr_type request_ptr) override;
protected:
	// Validators
	inline
	void validate_method(server::http::method method) const;
	
	void validate_path(const std::string &path) const;
	
	
	// Error handlers
	server::protocol::http::response::ptr_type
	handle_error(server::protocol::http::request::ptr_type request_ptr,
				 const char *what,
				 const server::http::status &status);
	
	inline
	server::protocol::http::response::ptr_type
	handle_error(server::protocol::http::request::ptr_type request_ptr,
				 const std::exception &e,
				 const server::http::status &status);
	
	
	// Data
	current_parameters parameters_;
private:
	HostType handler_;
};	// class file


};	// namespace host
};	// namespace server


#include <server/host/file.hpp>

#endif	// SERVER_HOST_FILE_H
