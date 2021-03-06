// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef HOST_MODULES_FILE_HANDLERS_FILES_AND_TEMPLATE_PAGES_H
#define HOST_MODULES_FILE_HANDLERS_FILES_AND_TEMPLATE_PAGES_H

#include <memory>
#include <regex>

#include <boost/filesystem/path.hpp>

#include <json.hpp>

#include <server/protocol/http.h>
#include <logic/page_model_generator.h>
#include <host/modules/file_handlers/files_only.h>
#include <host/modules/file_handlers/template_pages_only.h>


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
	enum class behavior {
		template_pages,
		files
	};	// enum class behavior
	
	
	
	files_and_template_pages(const nlohmann::json &config,
							 logic::page_model_generator &logic);
	
	
	template<class FileHost>
	std::unique_ptr<server::protocol::http::response>
	operator()(const FileHost &host,
			   const server::worker &worker,
			   const server::protocol::http::request &request,
			   const boost::filesystem::path &path) const;
	
	
	inline void clear_cache() noexcept;
protected:
	// Parameters
	// By default: interpret all files as plain files...
	behavior default_behavior_ = behavior::files;	// Optional
	
	// ...but change behavior for .html files.
	std::regex change_behavior_regex_{".*\\.html", std::regex::optimize};	// Optional
};	// files_and_template_pages


};	// namespace file_handlers
};	// namespace host


#include <host/modules/file_handlers/files_and_template_pages.hpp>

#endif	// HOST_MODULES_FILE_HANDLERS_FILES_AND_TEMPLATE_PAGES_H
