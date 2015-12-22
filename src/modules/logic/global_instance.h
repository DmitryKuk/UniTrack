// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGIC_GLOBAL_INSTANCE_H
#define LOGIC_GLOBAL_INSTANCE_H

#include <string>
#include <memory>
#include <random>
#include <ctime>

#include <json.hpp>

#include <mongo/client/init.h>
#include <mongo/client/options.h>
#include <mongo/client/dbclient.h>
#include <mongo/client/dbclientcursor.h>

#include <logic/random_hash.h>


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
	
	inline std::time_t session_lifetime() const noexcept;
	inline std::time_t session_restart_after() const noexcept;
	inline std::time_t session_forget_after() const noexcept;
	
	inline unsigned int session_id_create_attempts() const noexcept;
	inline unsigned int user_ref_create_attempts() const noexcept;
	
	
	inline std::string generate_session_id(const std::string &user_id);
	inline std::string generate_user_ref(const std::string &user_id);
	
	inline std::random_device & random_device();
protected:
	static constexpr std::time_t one_month		= 30 * 24 * 60 * 60 * 1000ull;	// Constant, in milliseconds
	
	
	// Parameters
	std::string		mongo_uri_;										// Required
	
	std::string		mongo_collection_users_,						// Required
					mongo_collection_sessions_;						// Required
	
	// In milliseconds!
	std::time_t		session_lifetime_			= one_month *  1,	// Optional (by default: 1 month)
					session_restart_after_		= one_month /  2,	// Optional (by default: 2 weeks)
					session_forget_after_		= one_month * 12;	// Optional (by default: 1 year)
	
	unsigned int	session_id_create_attempts_	= 3;				// Optional
	unsigned int	user_ref_create_attempts_	= 5;				// Optional
	
	
	// Data
	std::random_device random_device_;
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



std::string
generate_session_id(const std::string &user_id, std::random_device &rd);


std::string
generate_user_ref(const std::string &user_id, std::random_device &rd);


};	// namespace logic


#include <logic/global_instance.hpp>

#endif	// LOGIC_GLOBAL_INSTANCE_H
