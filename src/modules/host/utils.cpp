// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/utils.h>

#include <string>

#include <base/json_utils.h>
#include <server/host/base.h>
#include <server/host/file.h>
#include <server/host/file_handlers/files_only.h>
#include <host/file_handlers/template_pages_only.h>
#include <host/file_handlers/files_and_template_pages.h>
#include <host/logic.h>
#include <host/hierarchy.h>

using namespace std::literals;


namespace {


std::shared_ptr<server::host::base>
generate_phony_host(const nlohmann::json &host_config,
					logic::global_instance & /* logic */)
{
	return std::make_shared<server::host::base>(host_config);
}


std::shared_ptr<server::host::base>
generate_files_only_host(const nlohmann::json &host_config,
						 logic::global_instance & /* logic */)
{
	return std::make_shared<server::host::file<server::host::file_handlers::files_only>>(
		host_config
	);
}


std::shared_ptr<server::host::base>
generate_template_pages_only_host(const nlohmann::json &host_config,
								  logic::global_instance &logic)
{
	return std::make_shared<server::host::file<host::file_handlers::template_pages_only>>(
		host_config,
		host::file_handlers::template_pages_only{logic}
	);
}


std::shared_ptr<server::host::base>
generate_files_and_template_pages_host(const nlohmann::json &host_config,
									   logic::global_instance &logic)
{
	return std::make_shared<server::host::file<host::file_handlers::files_and_template_pages>>(
		host_config,
		host::file_handlers::files_and_template_pages{host_config, logic}
	);
}


std::shared_ptr<server::host::base>
generate_logic_host(const nlohmann::json &host_config,
					logic::global_instance &logic)
{
	return std::make_shared<host::logic>(host_config, logic);
}


std::shared_ptr<server::host::base>
generate_hierarchy_host(const nlohmann::json &host_config,
						logic::global_instance &logic)
{
	return std::make_shared<host::hierarchy>(host_config, logic);
}



using host_generator_type =
	std::shared_ptr<server::host::base>
	(*)(const nlohmann::json &,
		logic::global_instance &);


using hg_pair = std::pair<std::string, host_generator_type>;


const std::unordered_map<std::string, host_generator_type> host_generators_map =
	{
		hg_pair{"phony"s,						generate_phony_host						},
		hg_pair{"files_only"s,					generate_files_only_host				},
		hg_pair{"template_pages_only"s,			generate_template_pages_only_host		},
		hg_pair{"files_and_template_pages"s,	generate_files_and_template_pages_host	},
		hg_pair{"logic"s,						generate_logic_host						},
		hg_pair{"hierarchy"s,					generate_hierarchy_host					}
	};


};	// namespace



std::shared_ptr<server::host::base>
host::generate_host(const nlohmann::json &host_config,
					::logic::global_instance &logic)
{
	// Getting type of new host
	std::string type;
	if (!base::json_utils::extract(host_config, type, "type"s))
		throw server::host::incorrect_config{"Required key: \"type\" missed"s};
	
	
	// Searching for generator of host of given type
	auto host_generator_it = host_generators_map.find(type);
	if (host_generator_it != host_generators_map.end())
		return (host_generator_it->second)(host_config, logic);	// Generating and returning new host
	
	
	// Throwing exception with message
	std::string message = "Unknown host type: \""s + type + '\"';
	
	auto it = host_generators_map.begin(), end = host_generators_map.end();
	if (it != end) {
		message += ", correct values are: "s;
		
		for (size_t i = 0, size = host_generators_map.size(); it != end; ++it, ++i) {
			message += '\"';
			message += it->first;
			message += '\"';
			
			if (i < size - 1)
				message += ", "s;
		}
	}
	
	throw server::host::incorrect_config{message};
}
