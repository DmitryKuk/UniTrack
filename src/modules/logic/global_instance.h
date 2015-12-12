// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGIC_GLOBAL_INSTANCE_H
#define LOGIC_GLOBAL_INSTANCE_H

#include <string>
#include <memory>

#include <json.hpp>

#include <mongo/client/init.h>
#include <mongo/client/options.h>
#include <mongo/client/dbclient.h>
#include <mongo/client/dbclientcursor.h>


namespace logic {


using cursor_ptr_type = std::unique_ptr<mongo::DBClientCursor>;


class global_instance
{
public:
	global_instance(const nlohmann::json &config,
					const mongo::client::Options &options = mongo::client::Options());
	
	
	inline mongo::DBClientBase & connection();
	
	inline const std::string & collection_users() const noexcept;
	inline const std::string & collection_sessions() const noexcept;
protected:
	// Parameters
	std::string mongo_uri_;					// Required
	
	std::string	mongo_collection_users_,	// Required
				mongo_collection_sessions_;	// Required
private:
	// Data
	mongo::client::GlobalInstance mongo_client_global_instance_;
	std::unique_ptr<mongo::DBClientBase> connection_ptr_;
};	// class global_instance



class enable_global_instance_ref
{
public:
	inline enable_global_instance_ref(global_instance &logic_gi) noexcept;
protected:
	inline ::logic::global_instance & logic_gi() const noexcept;
private:
	// Data
	global_instance &logic_gi_;
};	// class enable_global_instance_ref


};	// namespace logic


#include <logic/global_instance.hpp>

#endif	// LOGIC_GLOBAL_INSTANCE_H
