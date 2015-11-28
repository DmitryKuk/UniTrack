// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logic/global_instance.h>

#include <base/json_utils.h>

#include <logic/exceptions.h>


// class logic::mongo_parameters
logic::global_instance::mongo_parameters::mongo_parameters(const nlohmann::json &config)
{
	if (!base::json_utils::extract(config, this->uri, "uri"))
		throw logic::parameters_init_error{"Required key: \"uri\" missed (in mongo parameters)"};
}



// class logic::parameters
logic::global_instance::parameters::parameters(const nlohmann::json &config)
{
	try {
		this->mongo = logic::global_instance::mongo_parameters{base::json_utils::at(config, "mongo")};
	} catch (const std::out_of_range &) {
		throw logic::parameters_init_error{"Required key: \"mongo\" missed"};
	}
}



// class logic::global_instance
logic::global_instance::global_instance(logger::logger &logger,
										const logic::global_instance::parameters &parameters,
										const mongo::client::Options &options):
	logger::enable_logger{logger},
	
	mongo_client_global_instance_{options},
	
	parameters_{parameters}
{
	if (!this->mongo_client_global_instance_.initialized())
		throw logic::global_instance_init_error{this->mongo_client_global_instance_.status().toString()};
	
	
	// Connection to MongoDB
	{
		std::string error_message;
		auto cs = mongo::ConnectionString::parse(this->parameters_.mongo.uri, error_message);
		if (!cs.isValid())
			throw logic::mongodb_incorrect_uri{error_message};
		
		this->connection_ptr_ = std::unique_ptr<mongo::DBClientBase>{cs.connect(error_message)};
		if (this->connection_ptr_) {
			this->logger().stream(logger::level::info)
				<< "Logic: Connected to MongoDB.";
		} else {
			throw logic::mongodb_connection_error{error_message};
		}
	}
}


logic::page_model
logic::global_instance::generate(const server::protocol::http::request &request,
								 const boost::filesystem::path &path) const
{
	auto stream = this->logger().stream(logger::level::info);
	stream << "Here! " << request.path;
	
	
	logic::page_model model;
	
	model.emplace("USERNAME", "Dmitry Kukovinets");
	model.emplace("UNIVERSITY", "STANKIN");
	model.emplace("DEPARTMENT", "Inteh");
	model.emplace("TAGS", "Student");
	
	return model;
}
