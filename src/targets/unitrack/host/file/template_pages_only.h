// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef HOST_FILE_TEMPLATE_PAGES_ONLY_H
#define HOST_FILE_TEMPLATE_PAGES_ONLY_H

#include <string>
#include <memory>
#include <unordered_map>

#include <boost/filesystem/path.hpp>

#include <base/buffer.h>
#include <templatizer/page.h>
#include <logic/page_model.h>
#include <logic/global_instance.h>


namespace host {
namespace file {


class template_pages_only
{
public:
	class response:
		public server::protocol::http::response
	{
	public:
		inline response(const templatizer::page &page,
						logic::page_model &&model,
						const server::protocol::http::status &status,
						server::protocol::http::version version = v_1_1);
	private:
		// Data
		const templatizer::page &page_;
		
		logic::page_model page_model_;
		
		friend class template_pages_only;
	};	// class response
	
	
	inline template_pages_only(logic::global_instance &logic_global_instance);
	
	
	template<class FileHost>
	server::protocol::http::response::ptr_type
	operator()(const FileHost &host,
			   server::protocol::http::request::ptr_type request_ptr,
			   const boost::filesystem::path &path);
private:
	class pages_cache
	{
	public:
		const templatizer::page & at(const boost::filesystem::path &path);
	private:
		std::unordered_map<boost::filesystem::path, std::unique_ptr<templatizer::page>> cache_;
	};	// class pages_cache
	
	
	pages_cache pages_cache_;
	
	logic::global_instance &logic_global_instance_;
};	// template_pages_only


};	// namespace file
};	// namespace host


#include <host/file/template_pages_only.hpp>

#endif	// HOST_FILE_TEMPLATE_PAGES_ONLY_H
