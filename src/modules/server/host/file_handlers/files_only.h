// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_HOST_FILE_HANDLERS_FILES_ONLY_H
#define SERVER_HOST_FILE_HANDLERS_FILES_ONLY_H

#include <memory>
#include <string>

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
		response(std::shared_ptr<const std::pair<::base::mapped_file, std::string>> &&fm_pair_ptr,
				 const worker &worker,
				 const ::server::protocol::http::request &request,
				 const ::server::protocol::http::status &status = ::server::protocol::http::status::ok);
	private:
		// Data
		std::shared_ptr<const std::pair<::base::mapped_file, std::string>> fm_pair_ptr_;
	};	// class response
	
	
	
	template<class FileHost>
	std::unique_ptr<::server::protocol::http::response>
	operator()(const FileHost &host,
			   const worker &worker,
			   const ::server::protocol::http::request &request,
			   const ::boost::filesystem::path &path) const;
	
	
	inline void clear_cache() noexcept;
private:
	using file_and_mime_pair = std::pair<::base::mapped_file, std::string>;
	
	
	static std::shared_ptr<file_and_mime_pair> load_file(const boost::filesystem::path &path);
	
	
	// Data
	mutable ::base::file_cache<
		file_and_mime_pair,
		std::shared_ptr<file_and_mime_pair> (*)(const boost::filesystem::path &path)
	> cache_{
		load_file
	};
};	// class files_only


};	// namespace file_handlers
};	// namespace host
};	// namespace server


#include <server/host/file_handlers/files_only.hpp>

#endif	// SERVER_HOST_FILE_HANDLERS_FILES_ONLY_H
