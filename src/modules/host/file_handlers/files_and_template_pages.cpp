// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/file_handlers/files_and_template_pages.h>

#include <string>

#include <base/json_utils.h>
#include <host/exceptions.h>

using namespace std::literals;


host::file_handlers::files_and_template_pages::parameters::parameters(const nlohmann::json &config)
{
	// Optional parameters
	{
		using behavior = host::file_handlers::files_and_template_pages::parameters::behavior;
		
		
		std::string str;
		
		if (base::json_utils::extract(config, str, "default_behavior"s)) {
			if (str == "files"s)
				this->default_behavior = behavior::files;
			else if (str == "template_pages"s)
				this->default_behavior = behavior::template_pages;
			else
				throw server::host::incorrect_config{"Incorrect default_behavior: \""s + str
													 + "\", correct values are: \"files\", \"template_pages\""s};
		}
	}
	
	
	{
		std::string str;
		
		if (base::json_utils::extract(config, str, "change_behavior_regex"s))
			this->change_behavior_regex = str;
	}
}
