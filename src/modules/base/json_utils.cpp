// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <base/json_utils.h>

#include <fstream>
#include <stdexcept>


nlohmann::json
base::json_utils::json_from_file(const boost::filesystem::path &path)
{
	std::ifstream stream(path.c_str());
	if (stream.bad() || stream.fail())
		throw std::logic_error{"Can't open config file: " + path.string()};
	
	nlohmann::json json;
	json << stream;
	return json;
}
