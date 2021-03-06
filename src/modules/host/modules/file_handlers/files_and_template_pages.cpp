// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/modules/file_handlers/files_and_template_pages.h>

#include <string>

#include <base/json_utils.h>
#include <host/exceptions.h>

using namespace std::literals;


// NOTE: This host can NOT be a module! It needs page model generator, so this host should be helper
// for some logic host, which provides page model generator.


host::file_handlers::files_and_template_pages::files_and_template_pages(const nlohmann::json &config,
																		logic::page_model_generator &logic):
	host::file_handlers::template_pages_only{logic}
{
	// Optional parameters
	{
		std::string str;
		
		if (base::json_utils::extract(config, str, "default_behavior"s)) {
			if (str == "files"s)
				this->default_behavior_ = behavior::files;
			else if (str == "template_pages"s)
				this->default_behavior_ = behavior::template_pages;
			else
				throw server::host::incorrect_config{"Incorrect default_behavior: \""s + str
													 + "\", correct values are: \"files\", \"template_pages\""s};
		}
	}
	
	
	{
		std::string str;
		
		if (base::json_utils::extract(config, str, "change_behavior_regex"s))
			this->change_behavior_regex_ = str;
	}
}
