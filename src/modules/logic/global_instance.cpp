// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logic/global_instance.h>

#include <logic/logic_exceptions.h>


logic::global_instance::global_instance(logger::logger &logger,
										const logic::global_instance_parameters &parameters,
										const mongo::client::Options &options):
	logger::enable_logger(logger),
	
	mongo_client_global_instance_(options),
	
	parameters_(parameters)
{
	if (!this->mongo_client_global_instance_.initialized())
		throw logic::global_instance_init_error(
			this->mongo_client_global_instance_.status().toString()
		);
	
	
	{
		std::string error_message;
		auto cs = mongo::ConnectionString::parse(this->parameters_.mongo.uri, error_message);
		if (!cs.isValid()) throw logic::mongodb_incorrect_uri(error_message);
		
		this->connection_ptr_.reset(cs.connect(error_message));
		if (this->connection_ptr_) {
			this->logger().stream(logger::level::info)
				<< "Logic: Connected to MongoDB.";
		} else {
			throw logic::mongodb_connection_error(error_message);
		}
	}
}
