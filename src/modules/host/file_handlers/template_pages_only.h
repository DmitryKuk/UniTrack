// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef HOST_FILE_TEMPLATE_PAGES_ONLY_H
#define HOST_FILE_TEMPLATE_PAGES_ONLY_H

#include <memory>

#include <boost/filesystem/path.hpp>

#include <base/file_cache.h>
#include <server/protocol/http.h>
#include <templatizer/page.h>
#include <logic/page_model.h>
#include <logic/global_instance.h>


namespace server {


class worker;


};	// namespace server


namespace host {
namespace file_handlers {


class template_pages_only
{
public:
	class response:
		public server::protocol::http::response
	{
	public:
		inline response(std::shared_ptr<const templatizer::page> &&page_ptr,
						logic::page_model &&model,
						const server::protocol::http::status &status,
						server::protocol::http::version version = server::protocol::http::version::v_1_1);
	private:
		friend class template_pages_only;
		
		// Data
		std::shared_ptr<const templatizer::page> page_ptr_;
		
		logic::page_model page_model_;
	};	// class response
	
	
	
	inline template_pages_only(logic::global_instance &logic);
	
	
	template<class FileHost>
	std::unique_ptr<server::protocol::http::response>
	operator()(const FileHost &host,
			   const server::worker &worker,
			   const server::protocol::http::request &request,
			   const boost::filesystem::path &path) const;
private:
	static std::shared_ptr<templatizer::page> load_page(const boost::filesystem::path &path);
	
	
	// Data
	mutable ::base::file_cache<
		templatizer::page,
		std::shared_ptr<templatizer::page> (*)(const boost::filesystem::path &)
	> cache_{
		load_page
	};
	
	logic::global_instance &logic_;
};	// template_pages_only


};	// namespace file_handlers
};	// namespace host


#include <host/file_handlers/template_pages_only.hpp>

#endif	// HOST_FILE_TEMPLATE_PAGES_ONLY_H
