// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGIC_GLOBAL_INSTANCE_H
#define LOGIC_GLOBAL_INSTANCE_H

#include <memory>

#include <boost/filesystem/path.hpp>

#include <json.hpp>

#include <mongo/client/init.h>
#include <mongo/client/options.h>
#include <mongo/client/dbclient.h>

#include <logger/logger.h>
#include <server/types.h>
#include <server/protocol/http/request.h>
#include <logic/page_model.h>


namespace logic {


class global_instance:
	protected logger::enable_logger_ref
{
public:
	struct mongo_parameters
	{
		std::string uri;
		
		
		explicit mongo_parameters() = default;
		explicit mongo_parameters(const nlohmann::json &config);
		
		mongo_parameters(const mongo_parameters &other) = default;
		mongo_parameters(mongo_parameters &&other) = default;
		
		mongo_parameters & operator=(const mongo_parameters &other) = default;
		mongo_parameters & operator=(mongo_parameters &&other) = default;
	};	// struct mongo_parameters
	
	
	struct parameters
	{
		mongo_parameters mongo;
		
		
		explicit parameters() = default;
		explicit parameters(const nlohmann::json &config);
		
		parameters(const parameters &other) = default;
		parameters(parameters &&other) = default;
		
		parameters & operator=(const parameters &other) = default;
		parameters & operator=(parameters &&other) = default;
	};	// struct parameters
	
	
	
	explicit global_instance(logger::logger &logger,
							 const parameters &parameters,
							 const mongo::client::Options &options = mongo::client::Options());
	
	
	logic::page_model generate(const server::protocol::http::request &request,
							   const boost::filesystem::path &path) const;
private:
	mongo::client::GlobalInstance mongo_client_global_instance_;
	std::unique_ptr<mongo::DBClientBase> connection_ptr_;
	
	parameters parameters_;
};	// class global_instance


};	// namespace logic


#endif	// LOGIC_GLOBAL_INSTANCE_H
