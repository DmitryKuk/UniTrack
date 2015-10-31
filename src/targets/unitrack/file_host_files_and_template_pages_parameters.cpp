// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <file_host_files_and_template_pages_parameters.h>

#include <base/json_utils.h>


files_and_template_pages_parameters::files_and_template_pages_parameters(
	const nlohmann::json &config)
{
	// Optional parameters
	{
		std::string str;
		
		if (base::json_utils::extract(config, str, "default_behavior")) {
			if (str == "files") {
				this->default_behavior
					= files_and_template_pages_parameters::behavior::files;
			} else if (str == "template_pages") {
				this->default_behavior
					= files_and_template_pages_parameters::behavior::template_pages;
			} else {
				throw std::logic_error("Incorrect config");
			}
		}
	}
	
	
	{
		std::string str;
		
		if (base::json_utils::extract(config, str, "change_behavior_regex"))
			this->change_behavior_regex = std::move(str);
	}
}
