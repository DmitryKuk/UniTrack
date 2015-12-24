// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef HOST_MODULES_FILE_HANDLERS_TEMPLATE_PAGES_ONLY_H
#define HOST_MODULES_FILE_HANDLERS_TEMPLATE_PAGES_ONLY_H

#include <memory>

#include <boost/filesystem/path.hpp>

#include <base/file_cache.h>
#include <server/protocol/http.h>
#include <templatizer/page.h>
#include <logic/page_model.h>
#include <logic/page_model_generator.h>


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
		response(std::shared_ptr<const std::pair<templatizer::page, std::string>> &&tm_pair_ptr,
				 logic::page_model &&model,
				 const server::worker &worker,
				 const server::protocol::http::request &request,
				 const server::protocol::http::status &status = server::protocol::http::status::ok);
	private:
		// Data
		std::shared_ptr<const std::pair<templatizer::page, std::string>> tm_pair_ptr_;
		
		logic::page_model page_model_;
	};	// class response
	
	
	
	inline template_pages_only(logic::page_model_generator &logic);
	
	
	template<class FileHost>
	inline
	std::unique_ptr<server::protocol::http::response>
	operator()(const FileHost &host,
			   const server::worker &worker,
			   const server::protocol::http::request &request,
			   const boost::filesystem::path &path) const;
	
	
	inline void clear_cache() noexcept;
private:
	using template_page_and_mime_pair = std::pair<templatizer::page, std::string>;
	
	
	static std::shared_ptr<template_page_and_mime_pair> load_page(const boost::filesystem::path &path);
	
	
	// Data
	mutable ::base::file_cache<
		template_page_and_mime_pair,
		std::shared_ptr<template_page_and_mime_pair> (*)(const boost::filesystem::path &)
	> cache_{
		load_page
	};
	
	logic::page_model_generator &logic_;
};	// template_pages_only


};	// namespace file_handlers
};	// namespace host


#include <host/modules/file_handlers/template_pages_only.hpp>

#endif	// HOST_MODULES_FILE_HANDLERS_TEMPLATE_PAGES_ONLY_H
