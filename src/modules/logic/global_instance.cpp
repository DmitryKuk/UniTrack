// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logic/global_instance.h>

#include <string>

#include <logger/logger.h>
#include <base/json_utils.h>
#include <logic/exceptions.h>

using namespace std::literals;


logic::global_instance::global_instance(const nlohmann::json &config,
										const mongo::client::Options &options):
	mongo_client_global_instance_{options}
{
	if (!this->mongo_client_global_instance_.initialized())
		throw logic::global_instance_init_error{this->mongo_client_global_instance_.status().toString()};
	
	
	// Parsing config
	const nlohmann::json *mongo_config_ptr = nullptr;
	try {
		mongo_config_ptr = &base::json_utils::at(config, "mongo"s);
	} catch (const std::out_of_range &) {
		throw logic::global_instance_init_error{"Required key: \"mongo\" missed"s};
	}
	
	if (!base::json_utils::extract(*mongo_config_ptr, this->mongo_uri_, "uri"s))
		throw logic::global_instance_init_error{"Required key: \"uri\" missed (in mongo parameters)"s};
	
	
	// Connection to MongoDB
	{
		std::string error_message;
		auto cs = mongo::ConnectionString::parse(this->mongo_uri_, error_message);
		if (!cs.isValid())
			throw logic::mongodb_incorrect_uri{error_message};
		
		this->connection_ptr_ = std::unique_ptr<mongo::DBClientBase>{cs.connect(error_message)};
		if (this->connection_ptr_) {
			logger::log(logger::level::info, "Logic: Connected to MongoDB."s);
		} else {
			throw logic::mongodb_connection_error{error_message};
		}
	}
}


logic::page_model
logic::global_instance::page_model(const server::protocol::http::request &request,
								   const boost::filesystem::path &path) const
{
	auto &&stream = logger::stream(logger::level::info);
	stream << "Here! " << request.path;
	
	
	logic::page_model model;
	
	model.emplace("USERNAME", "Dmitry Kukovinets");
	model.emplace("UNIVERSITY", "STANKIN");
	model.emplace("DEPARTMENT", "Inteh");
	model.emplace("TAGS", "Student");
	
	return model;
}
