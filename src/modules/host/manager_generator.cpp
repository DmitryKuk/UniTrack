// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/manager_generator.h>

#include <string>

#include <base/json_utils.h>
#include <server/host/base.h>
#include <server/host/file.h>
#include <server/host/file_handlers/files_only.h>
#include <host/file_handlers/template_pages_only.h>
#include <host/file_handlers/files_and_template_pages.h>
#include <host/logic.h>

using namespace std::literals;


namespace {


std::shared_ptr<server::host::base>
phony_host(const nlohmann::json &host_config,
		   logic::global_instance & /* logic */)
{
	return std::make_shared<server::host::base>(
		host_config
	);
}


std::shared_ptr<server::host::base>
files_only_host(const nlohmann::json &host_config,
				logic::global_instance & /* logic */)
{
	return std::make_shared<server::host::file<server::host::file_handlers::files_only>>(
		host_config
	);
}


std::shared_ptr<server::host::base>
template_pages_only_host(const nlohmann::json &host_config,
						 logic::global_instance &logic)
{
	return std::make_shared<server::host::file<host::file_handlers::template_pages_only>>(
		host_config,
		host::file_handlers::template_pages_only{logic}
	);
}


std::shared_ptr<server::host::base>
files_and_template_pages_host(const nlohmann::json &host_config,
							  logic::global_instance &logic)
{
	return std::make_shared<server::host::file<host::file_handlers::files_and_template_pages>>(
		host_config,
		host::file_handlers::files_and_template_pages{
			host_config,
			logic
		}
	);
}


std::shared_ptr<server::host::base>
logic_host(const nlohmann::json &host_config,
		   logic::global_instance &logic)
{
	return std::make_shared<host::logic>(
		host_config,
		logic
	);
}



using host_generator_type =
	std::shared_ptr<server::host::base>
	(*)(const nlohmann::json &,
		logic::global_instance &);


using hg_pair = std::pair<std::string, host_generator_type>;


const std::unordered_map<std::string, host_generator_type> host_generators_map =
{
	hg_pair{"phony"s,						phony_host					 },
	hg_pair{"files_only"s,					files_only_host				 },
	hg_pair{"template_pages_only"s,			template_pages_only_host	 },
	hg_pair{"files_and_template_pages"s,	files_and_template_pages_host},
	hg_pair{"logic"s,						logic_host					 }
};


class host_manager_with_logic:
	public server::host::manager,
	private logic::global_instance
{
public:
	host_manager_with_logic(const nlohmann::json &logic_config):
		logic::global_instance{logic_config}
	{}
	
	
	logic::global_instance & logic() noexcept
	{
		return static_cast<logic::global_instance &>(*this);
	}
};	// class host_manager_with_logic


};	// namespace



host::manager_generator::manager_generator(const nlohmann::json &hosts_config,
										   const nlohmann::json &logic_config):
	hosts_config_{hosts_config},
	logic_config_{logic_config}
{}


std::unique_ptr<server::host::manager>
host::manager_generator::operator()() const
{
	auto host_manager_ptr = std::make_unique<host_manager_with_logic>(this->logic_config_);
	
	for (const auto &host_config: this->hosts_config_) {
		// Getting type of new host
		std::string type;
		if (!base::json_utils::extract(host_config, type, "type"s))
			throw server::host::incorrect_config{"Required key: \"type\""s};
		
		// Searching for generator of host of given type
		auto host_generator_it = host_generators_map.find(type);
		if (host_generator_it == host_generators_map.end())
			throw server::host::incorrect_config{"Unknown host type: \""s + type
												 + "\", correct values are: \"phony\", \"files_only\", "
												   "\"template_pages_only\", \"files_and_template_pages\""s};
		
		// Generating new host
		auto host_ptr = (host_generator_it->second)(
			host_config,
			host_manager_ptr->logic()
		);
		
		// And adding it
		host_manager_ptr->add_host(host_ptr);
	}
	
	return std::move(host_manager_ptr);
};
