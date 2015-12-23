// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/host/file_handlers/files_only.h>

#include <tuple>
#include <utility>

#include <system_/utils.h>
#include <system_/exceptions.h>


// static
std::shared_ptr<::server::host::file_handlers::files_only::file_and_mime_pair>
server::host::file_handlers::files_only::load_file(const boost::filesystem::path &path)
{
	std::string mime_type;
	try {
		mime_type = system_::get_file_mime(path.string());
	} catch (const system_::unknown_error &e) {
		mime_type.clear();
	}
	
	
	return std::make_shared<::server::host::file_handlers::files_only::file_and_mime_pair>(
		std::piecewise_construct,
		std::forward_as_tuple(path, boost::interprocess::read_only, MAP_SHARED),
		std::forward_as_tuple(mime_type)
	);
}
