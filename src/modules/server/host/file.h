// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_HOST_FILE_H
#define SERVER_HOST_FILE_H

#include <vector>
#include <string>
#include <regex>

#include <boost/filesystem/path.hpp>

#include <server/host/base.h>


namespace server {
namespace host {


template<class HostType>
class file:
	public ::server::host::base
{
public:
	// May be useful, if several allow_regexes specified.
	enum class allow_match_mode
	{
		any,	// Path matches any of allow_regexes (and doesn't match any of deny_regexes).
		all		// Path matches all of allow_regexes (and doesn't match any of deny_regexes).
	};	// enum class allow_match_mode
	
	
	
	file(const nlohmann::json &config,
		 HostType &&handler = std::move(HostType()));
	
	file(const nlohmann::json &config,
		 const HostType &handler);
	
	
	// Non-copy/-move constructible/assignable. Use ptrs.
	template<class HostType1>
	file(const file<HostType1> &other) = delete;
	
	template<class HostType1>
	file(file<HostType1> &&other) = delete;
	
	template<class HostType1>
	file<HostType> & operator=(const file<HostType1> &other) = delete;
	
	template<class HostType1>
	file<HostType> & operator=(file<HostType1> &&other) = delete;
	
	
	virtual
	std::unique_ptr<::server::protocol::http::response>
	response(const worker &worker,
			 const ::server::protocol::http::request &request) const override;
protected:
	// Validators
	inline void validate_method(::server::protocol::http::method method) const;
	void validate_path(const std::string &path) const;
	
	
	// Error handlers
	std::unique_ptr<::server::protocol::http::response>
	handle_error(const worker &worker,
				 const ::server::protocol::http::request &request,
				 const char *what,
				 const ::server::protocol::http::status &status) const;
	
	inline
	std::unique_ptr<::server::protocol::http::response>
	handle_error(const worker &worker,
				 const ::server::protocol::http::request &request,
				 const std::exception &e,
				 const ::server::protocol::http::status &status) const;
	
	
	// Config parser
	void parse_config(const nlohmann::json &config);
	
	
	
	// Parameters
	boost::filesystem::path root_;						// Required
	
	// Set to "index.html" for standard behavior. Empty value denies searching, if directory path requested
	boost::filesystem::path default_index_file_ = "";	// Optional
	
	std::vector<std::regex>
		deny_regexes_ =									// Optional
			{
				std::regex(".*\\.\\./.*"),	// Don't allow "../" sequences!
				std::regex(".*/\\..*")		// Don't allow hidden directories and files
			},
		allow_regexes_ =								// Optional
			{
				// Don't allow anything by default
			};
	
	allow_match_mode mode_ = allow_match_mode::all;		// Optional
private:
	// Data
	HostType handler_;
};	// class file


};	// namespace host
};	// namespace server


#include <server/host/file.hpp>

#endif	// SERVER_HOST_FILE_H
