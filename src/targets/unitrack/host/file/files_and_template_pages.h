// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef HOST_FILE_FILES_AND_TEMPLATE_PAGES_H
#define HOST_FILE_FILES_AND_TEMPLATE_PAGES_H

#include <regex>

#include <json.hpp>

#include <base/buffer.h>
#include <server/host/file/files_only.h>
#include <logic/global_instance.h>

#include <host/file/template_pages_only.h>


namespace host {
namespace file {


class files_and_template_pages:
	private server::host::file::files_only,
	private host::file::template_pages_only
{
public:
	struct parameters
	{
		enum class behavior {
			template_pages,
			files
		};
		
		
		// By default: interpret all files as plain files...
		behavior default_behavior = behavior::files;				// Optional
		
		// ...but change behavior for .html files.
		std::regex change_behavior_regex = std::regex(".*\\.html");	// Optional
		
		
		explicit parameters() = default;
		explicit parameters(const nlohmann::json &config);
	};	// struct parameters
	
	
	
	inline files_and_template_pages(logic::global_instance &logic_global_instance,
									const parameters &parameters);
	
	
	template<class FileHost>
	server::protocol::http::response::ptr_type
	operator()(const FileHost &host,
			   server::protocol::http::request::ptr_type request_ptr,
			   const boost::filesystem::path &path);
private:
	parameters parameters_;
};	// files_and_template_pages


};	// namespace file
};	// namespace host


#include <host/file/files_and_template_pages.hpp>

#endif	// HOST_FILE_FILES_AND_TEMPLATE_PAGES_H
