// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/host/file_handlers/files_only.h>


// static
std::shared_ptr<::base::mapped_file>
server::host::file_handlers::files_only::load_file(const boost::filesystem::path &path)
{
	return std::make_shared<::base::mapped_file>(
		path,
		boost::interprocess::read_only,
		MAP_SHARED
	);
}
