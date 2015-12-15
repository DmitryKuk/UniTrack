// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGIC_BASE_H
#define LOGIC_BASE_H

#include <string>
#include <utility>

#include <mongo/client/redef_macros.h>
#include <mongo/bson/bsonobj.h>
#include <mongo/bson/bsonobjbuilder.h>
#include <mongo/bson/bsonelement.h>

#include <logic/global_instance.h>


namespace logic {


class base:
	public ::logic::enable_global_instance_ref
{
public:
	// Constructor
	using ::logic::enable_global_instance_ref::enable_global_instance_ref;
	
	
	// Starts a new session for user by id.
	// Returns pair of user id and session id or throws.
	std::pair<std::string, std::string>
	start_session_for_id(const std::string &user_id,
						 const std::string &user_password) const;
	
	
	// Starts a new session for user by login and password.
	// Returns pair of user id and session id or throws.
	std::pair<std::string, std::string>
	start_session_for_login(const std::string &user_login,
							const std::string &user_password) const;
	
	
	// Starts a new session for user by email and password.
	// Returns pair of user id and session id or throws.
	std::pair<std::string, std::string>
	start_session_for_email(const std::string &user_email,
							const std::string &user_password) const;
	
	
	// Continues user session.
	// Returns correct user id or throws.
	std::string continue_session(const std::string &session_id) const;
	
	
	// Finishes user session.
	void finish_session(const std::string &session_id) const;
private:
	// Starts a new session for user by cursor.
	// Returns pair of user id and session id or throws.
	std::pair<std::string, std::string>
	start_session_for_obj(const mongo::BSONObj &user_obj,
						  const std::string &user_password) const;
};	// class base


};	// namespace logic


#endif	// LOGIC_BASE_H
