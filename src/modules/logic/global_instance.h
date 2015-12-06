// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGIC_GLOBAL_INSTANCE_H
#define LOGIC_GLOBAL_INSTANCE_H

#include <memory>

#include <boost/filesystem/path.hpp>

#include <json.hpp>

#include <mongo/client/init.h>
#include <mongo/client/options.h>
#include <mongo/client/dbclient.h>

#include <server/types.h>
#include <server/protocol/http/request.h>
#include <logic/page_model.h>


namespace logic {


class global_instance
{
public:
	global_instance(const nlohmann::json &config,
					const mongo::client::Options &options = mongo::client::Options());
	
	
	logic::page_model page_model(const server::protocol::http::request &request,
								 const boost::filesystem::path &path) const;
protected:
	// Parameters
	std::string mongo_uri_;	// Required
private:
	// Data
	mongo::client::GlobalInstance mongo_client_global_instance_;
	std::unique_ptr<mongo::DBClientBase> connection_ptr_;
};	// class global_instance


};	// namespace logic


#endif	// LOGIC_GLOBAL_INSTANCE_H
