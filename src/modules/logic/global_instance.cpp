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
	
	if (!base::json_utils::extract(*mongo_config_ptr, this->mongo_collection_users_, "collection_users"s))
		throw logic::global_instance_init_error{"Required key: \"collection_users\" missed (in mongo parameters)"s};
	
	if (!base::json_utils::extract(*mongo_config_ptr, this->mongo_collection_sessions_, "collection_sessions"s))
		throw logic::global_instance_init_error{"Required key: \"collection_sessions\" missed (in mongo parameters)"s};
	
	// Optional parameters
	base::json_utils::extract(*mongo_config_ptr, this->session_lifetime_, "session_lifetime"s);
	base::json_utils::extract(*mongo_config_ptr, this->session_forget_time_, "session_forget_time"s);
	
	base::json_utils::extract(*mongo_config_ptr, this->session_create_attempts_, "session_create_attempts"s);
	
	
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



std::string
logic::generate_session_id(const std::string &user_id, std::random_device &rd)
{
	return logic::generate_random_hash<CryptoPP::SHA3_512>(user_id, rd);
}
