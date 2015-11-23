// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_HOST_FILE_H
#define SERVER_HOST_FILE_H

#include <vector>
#include <regex>

#include <boost/filesystem/path.hpp>

#include <logger/logger.h>
#include <server/host/base.h>


namespace server {
namespace host {


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
	
	
	virtual
	std::shared_ptr<server::protocol::http::response>
	response(const worker &worker,
			 const server::protocol::http::request &request) override;
protected:
	// Validators
	inline
	void validate_method(server::http::method method) const;
	
	void validate_path(const std::string &path) const;
	
	
	// Error handlers
	std::shared_ptr<server::protocol::http::response>
	handle_error(const worker &worker,
				 const server::protocol::http::request &request,
				 const char *what,
				 const server::http::status &status);
	
	inline
	std::shared_ptr<server::protocol::http::response>
	handle_error(const worker &worker,
				 const server::protocol::http::request &request,
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
