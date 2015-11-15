// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGIC_GLOBAL_INSTANCE_H
#define LOGIC_GLOBAL_INSTANCE_H

#include <memory>

#include <mongo/client/init.h>
#include <mongo/client/options.h>
#include <mongo/client/dbclient.h>

#include <logger/logger.h>
#include <server/types.h>

#include <logic/global_instance_parameters.h>
#include <logic/page_model.h>


namespace logic {


class global_instance:
	protected logger::enable_logger
{
public:
	explicit global_instance(logger::logger &logger,
							 const logic::global_instance_parameters &parameters,
							 const mongo::client::Options &options = mongo::client::Options());
	
	
	logic::page_model generate(const server::host_cache &cache) const;
private:
	mongo::client::GlobalInstance mongo_client_global_instance_;
	std::unique_ptr<mongo::DBClientBase> connection_ptr_;
	
	logic::global_instance_parameters parameters_;
};	// class global_instance


};	// namespace logic


#endif	// LOGIC_GLOBAL_INSTANCE_H
