// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/file/files_and_template_pages.h>

#include <base/json_utils.h>


host::file::files_and_template_pages::parameters::parameters(const nlohmann::json &config)
{
	// Optional parameters
	{
		using behavior = host::file::files_and_template_pages::parameters::behavior;
		
		
		std::string str;
		
		if (base::json_utils::extract(config, str, "default_behavior")) {
			if (str == "files") {
				this->default_behavior = behavior::files;
			} else if (str == "template_pages") {
				this->default_behavior = behavior::template_pages;
			} else {
				throw std::logic_error("Incorrect config");
			}
		}
	}
	
	
	{
		std::string str;
		
		if (base::json_utils::extract(config, str, "change_behavior_regex"))
			this->change_behavior_regex = str;
	}
}
