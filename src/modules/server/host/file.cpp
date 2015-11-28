// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/host/file.h>

#include <boost/filesystem.hpp>

#include <base/json_utils.h>
#include <server/host/exceptions.h>


// struct server::host::file_only_parameters
server::host::file_only_parameters::file_only_parameters(const nlohmann::json &config):
	root{::base::json_utils::get<decltype(this->root)>(config, "root")}
{
	if (!boost::filesystem::exists(this->root))
		throw ::server::host::path_not_found{this->root.string()};
	this->root = boost::filesystem::canonical(this->root, "/");
	
	
	// Allow mode
	{
		const auto mode = ::base::json_utils::get<std::string>(config, "allow_match_mode");
		if (mode == "any") {
			this->mode = file_parameters::allow_match_mode::any;
		} else if (mode == "all") {
			this->mode = file_parameters::allow_match_mode::all;
		} else {
			throw server::host::incorrect_config{"Incorrect allow_match_mode: \"" + mode
												 + "\", correct values are: \"any\", \"all\""};
		}
	}
	
	
	// Optional parameters
	{
		static const auto fill_regexes =
			[](auto &regex_vector, const auto &regexes)
			{
				regex_vector.clear();
				for (const std::string &regex: regexes)
					regex_vector.emplace_back(regex);
			};
		
		
		try {
			fill_regexes(this->allow_regexes, ::base::json_utils::at(config, "allow_regexes"));
		} catch (const std::out_of_range &) {}
		
		try {
			fill_regexes(this->deny_regexes, ::base::json_utils::at(config, "deny_regexes"));
		} catch (const std::out_of_range &) {}
	}
}
