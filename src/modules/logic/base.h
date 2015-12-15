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
	
	
	// Starts a new session for user by email and password.
	// Returns pair of user ref and session id or throws.
	std::pair<std::string, std::string>
	start_session_for_email(const std::string &user_email,
							const std::string &user_password) const;
	
	
	// Continues user session.
	// Returns pair of user ref and session id or throws.
	std::pair<std::string, std::string>
	continue_session(const std::string &session_id) const;
	
	
	// Finishes user session.
	void finish_session(const std::string &session_id) const;
private:
	// Starts a new session for user.
	// Returns pair of user ref and session id or throws.
	std::pair<std::string, std::string>
	start_session_for_obj(const mongo::BSONObj &user_obj,
						  const std::string &user_password) const;
	
	
	// Starts a new session for user.
	// Returns pair of user ref and session id or throws.
	std::pair<std::string, std::string>
	start_session_for_obj_without_password_check(const mongo::BSONObj &user_obj) const;
	
	
	// Restarts the session.
	// Returns pair of user ref and new session id or throws.
	std::pair<std::string, std::string>
	restart_session(const std::string &session_id,
					const mongo::BSONObj &session_obj) const;
};	// class base


};	// namespace logic


#endif	// LOGIC_BASE_H
