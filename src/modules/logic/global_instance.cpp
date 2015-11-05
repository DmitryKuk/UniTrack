// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logic/global_instance.h>

#include <logic/logic_exceptions.h>


logic::global_instance::global_instance(logger::logger &logger,
										const mongo::client::Options &options):
	logger::enable_logger(logger),
	
	mongo_client_global_instance_(options)
{
	if (this->mongo_client_global_instance_.initialized()) {
		logger.stream(logger::level::info)
			<< "Logic: Global instance initialized.";
	} else {
		logger.stream(logger::level::critical)
			<< "Logic: Can\'t initialize global instance: "
			<< this->mongo_client_global_instance_.status().toString();
		
		throw logic::global_instance_init_error();
	}
}
