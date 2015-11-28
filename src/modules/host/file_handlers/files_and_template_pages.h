// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef HOST_FILE_FILES_AND_TEMPLATE_PAGES_H
#define HOST_FILE_FILES_AND_TEMPLATE_PAGES_H

#include <memory>
#include <regex>

#include <boost/filesystem/path.hpp>

#include <json.hpp>

#include <server/host/file_handlers/files_only.h>
#include <server/protocol/http.h>
#include <logic/global_instance.h>
#include <host/file_handlers/template_pages_only.h>


namespace server {


class worker;


};	// namespace server


namespace host {
namespace file_handlers {


class files_and_template_pages:
	private server::host::file_handlers::files_only,
	private host::file_handlers::template_pages_only
{
public:
	struct parameters
	{
		enum class behavior {
			template_pages,
			files
		};	// enum class behavior
		
		
		// By default: interpret all files as plain files...
		behavior default_behavior = behavior::files;				// Optional
		
		// ...but change behavior for .html files.
		std::regex change_behavior_regex = std::regex(".*\\.html");	// Optional
		
		
		explicit parameters() = default;
		explicit parameters(const nlohmann::json &config);
	};	// struct parameters
	
	
	
	inline files_and_template_pages(logic::global_instance &logic,
									const parameters &parameters);
	
	
	template<class FileHost>
	std::unique_ptr<server::protocol::http::response>
	operator()(const FileHost &host,
			   const server::worker &worker,
			   const server::protocol::http::request &request,
			   const boost::filesystem::path &path);
protected:
	parameters parameters_;
};	// files_and_template_pages


};	// namespace file_handlers
};	// namespace host


#include <host/file_handlers/files_and_template_pages.hpp>

#endif	// HOST_FILE_FILES_AND_TEMPLATE_PAGES_H
