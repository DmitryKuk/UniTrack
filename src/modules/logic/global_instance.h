// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGIC_GLOBAL_INSTANCE_H
#define LOGIC_GLOBAL_INSTANCE_H

#include <mongo/client/init.h>
#include <mongo/client/options.h>

#include <logger/logger.h>


namespace logic {


class global_instance:
	protected logger::enable_logger
{
public:
	explicit global_instance(logger::logger &logger,
							 const mongo::client::Options &options = mongo::client::Options());
private:
	mongo::client::GlobalInstance mongo_client_global_instance_;
};	// class global_instance


};	// namespace logic


#endif	// LOGIC_GLOBAL_INSTANCE_H
