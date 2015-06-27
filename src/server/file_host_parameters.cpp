// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/file_host_parameters.h>

#include <string>
#include <functional>

#include <boost/filesystem.hpp>

#include <base/json_utils.h>

#include <server/host_exceptions.h>


server::file_host_only_parameters::file_host_only_parameters(const nlohmann::json &config):
	root(base::json_utils::get<decltype(this->root)>(config, "root"))
{
	if (!boost::filesystem::exists(this->root))
		throw server::path_not_found(this->root.string());
	this->root = std::move(boost::filesystem::canonical(this->root, "/"));
	
	
	// Allow mode
	{
		const auto mode = base::json_utils::get<std::string>(config, "allow_match_mode");
		if (mode == "any") {
			this->mode = server::file_host_parameters::allow_match_mode::any;
		} else if (mode == "all") {
			this->mode = server::file_host_parameters::allow_match_mode::all;
		} else {
			throw std::logic_error("Incorrect config");
		}
	}
	
	
	// Optional parameters
	{
		static const auto fill_regexes = [](auto &regex_vector, const auto &regexes)
		{
			if (!regexes.empty()) {
				regex_vector.clear();
				for (const std::string &regex: regexes)
					regex_vector.emplace_back(regex);
			}
		};
		
		
		try {
			fill_regexes(this->allow_regexes, base::json_utils::at(config, "allow_regexes"));
		} catch (const std::out_of_range &) {}
		
		
		try {
			fill_regexes(this->deny_regexes, base::json_utils::at(config, "deny_regexes"));
		} catch (const std::out_of_range &) {}
	}
}
