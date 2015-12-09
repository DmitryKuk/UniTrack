// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_HOST_FILE_HANDLERS_FILES_ONLY_H
#define SERVER_HOST_FILE_HANDLERS_FILES_ONLY_H

#include <memory>

#include <boost/filesystem/path.hpp>

#include <base/mapped_file.h>
#include <base/file_cache.h>
#include <server/types.h>
#include <server/protocol/http.h>


namespace server {


class worker;


namespace host {
namespace file_handlers {


class files_only
{
public:
	class response:
		public ::server::protocol::http::response
	{
	public:
		inline response(std::shared_ptr<const ::base::mapped_file> &&mapped_file_ptr,
						const ::server::protocol::http::status &status,
						::server::protocol::http::version version = ::server::protocol::http::version::v_1_1);
	private:
		friend class files_only;
		
		// Data
		std::shared_ptr<const ::base::mapped_file> mapped_file_ptr_;
	};	// class response
	
	
	template<class FileHost>
	std::unique_ptr<::server::protocol::http::response>
	operator()(const FileHost &host,
			   const worker &worker,
			   const ::server::protocol::http::request &request,
			   const ::boost::filesystem::path &path) const;
private:
	static std::shared_ptr<::base::mapped_file> load_file(const boost::filesystem::path &path);
	
	
	// Data
	mutable ::base::file_cache<
		::base::mapped_file,
		std::shared_ptr<::base::mapped_file> (*)(const boost::filesystem::path &)
	> cache_{
		load_file
	};
};	// class files_only


};	// namespace file_handlers
};	// namespace host
};	// namespace server


#include <server/host/file_handlers/files_only.hpp>

#endif	// SERVER_HOST_FILE_HANDLERS_FILES_ONLY_H
