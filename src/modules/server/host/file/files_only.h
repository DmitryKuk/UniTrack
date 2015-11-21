// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_HOST_FILE_FILES_ONLY_H
#define SERVER_HOST_FILE_FILES_ONLY_H

#include <string>

#include <base/mapped_file.h>
#include <base/buffer.h>
#include <server/types.h>
#include <server/worker.h>
#include <server/protocol/http.h>


namespace server {
namespace host {


class files_only
{
public:
	class response:
		public server::protocol::http::response
	{
	public:
		inline response(base::mapped_file &&mapped_file,
						const server::protocol::http::status &status,
						server::protocol::http::version version = server::protocol::http::version::v_1_1);
	private:
		// Data
		base::mapped_file mapped_file_;
	};	// class response
	
	
	template<class FileHost>
	server::protocol::http::response::ptr_type
	operator()(const FileHost &host,
			   const server::worker &worker,
			   server::protocol::http::request::ptr_type request_ptr,
			   const boost::filesystem::path &path);
};	// class files_only


};	// namespace host
};	// namespace server


#include <server/host/file/files_only.hpp>

#endif	// SERVER_HOST_FILE_FILES_ONLY_H
