// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logic/base.h>

#include <memory>

#include <mongo/bson/bsonobj.h>
#include <mongo/bson/bsonobjbuilder.h>
#include <mongo/bson/bsonelement.h>

#include <base/time.h>
#include <logger/logger.h>
#include <logic/exceptions.h>

using namespace std::literals;


// Starts a new session for user by email and password.
// Returns pair of user ref and session id or throws.
std::pair<std::string, std::string>
logic::base::start_session_for_email(const std::string &user_email,
									 const std::string &user_password) const
{
	logic::cursor_ptr_type users_cursor_ptr = this->logic_gi().connection().query(
		this->logic_gi().collection_users(),
		MONGO_QUERY("email"s << user_email),	// Search by email
		1										// Need only 1 BSON object
	);
	
	if (users_cursor_ptr == nullptr)
		throw logic::incorrect_cursor{};
	
	if (!users_cursor_ptr->more())
		throw logic::user_not_found{"For email: \""s + user_email + "and password: \""s + user_password + '\"'};
	
	mongo::BSONObj user_obj = users_cursor_ptr->nextSafe();
	
	return this->start_session_for_obj(user_obj, user_password);
}


// Continues user session.
// Returns pair of user ref and session id or throws.
std::pair<std::string, std::string>
logic::base::continue_session(const std::string &session_id) const
{
	logic::cursor_ptr_type sessions_cursor_ptr = this->logic_gi().connection().query(
		this->logic_gi().collection_sessions(),
		MONGO_QUERY("session_id"s << session_id),	// Search by session id (NOT _id!!!)
		1											// Need only 1 BSON object
	);
	
	if (sessions_cursor_ptr == nullptr)
		throw logic::incorrect_cursor{};
	
	if (sessions_cursor_ptr->more()) {	// Current session found
		mongo::BSONObj session_obj = sessions_cursor_ptr->nextSafe();
		unsigned long long time_now = static_cast<unsigned long long>(::base::utc_time());
		
		if (time_now < session_obj["valid_until"s].Date().asInt64()) {	// If session is valid
			if (time_now < session_obj["restart_at"s].Date().asInt64())	// Don't need to restart
				return std::make_pair(session_obj["user_ref"s].str(), session_id);
			else														// Need to restart session
				return this->restart_session(session_id, session_obj);
		}
	}
	
	throw logic::session_not_found{session_id};
}


// Finishes user session.
void
logic::base::finish_session(const std::string &session_id) const
{
	mongo::Date_t time_now{static_cast<unsigned long long>(::base::utc_time())};
	
	mongo::BSONObj session_obj = this->logic_gi().connection().findAndModify(
		this->logic_gi().collection_sessions(),
		BSON("session_id"s	<< session_id),					// Search by session id (NOT _id!!!)
		BSON("$min"s << BSON("valid_until"s	<< time_now)),	// Update if session valid
		false,												// Not upsert
		false,												// Return old version
		mongo::BSONObj{},									// Not sort
		BSON("_id"s << 1 << "user_id"s << 1)				// Return user_id field
	);
	
	// Check session_obj for errors
	if (session_obj.hasField("$err"s))
		throw logic::session_error{session_id, session_obj["$err"s].String()};
	
	
	// Register session in user object
	this->logic_gi().connection().findAndModify(
		this->logic_gi().collection_users(),								// In users collection
		BSON("_id"s << session_obj["user_id"s].OID()),						// Find user by OID
		BSON("$pull"s << BSON("sessions"s << session_obj["_id"s].OID()))	// Remove session OID from "sessions" array
	);
}


// private
// Starts a new session for user.
// Returns pair of user ref and session id or throws.
std::pair<std::string, std::string>
logic::base::start_session_for_obj(const mongo::BSONObj &user_obj,
								   const std::string &user_password) const
{
	// Warning: this method should be atomic!
	mongo::OID user_oid = user_obj["_id"s].OID();
	std::string user_id = user_oid.toString();
	
	
	// Password check
	if (user_obj["password"s].str() != user_password)
		throw logic::password_not_match(user_id, user_password + '/' + user_obj["password"s].str());
	
	
	return this->start_session_for_obj_without_password_check(user_obj);
}


// Starts a new session for user.
// Returns pair of user ref and session id or throws.
std::pair<std::string, std::string>
logic::base::start_session_for_obj_without_password_check(const mongo::BSONObj &user_obj) const
{
	// Warning: this method should be atomic!
	mongo::OID user_oid = user_obj["_id"s].OID();
	std::string user_id = user_oid.toString();
	
	
	std::string session_id;
	int attempts_availible = this->logic_gi().session_id_create_attempts();
	do {
		--attempts_availible;
		session_id = this->logic_gi().generate_session_id(user_id);
		
		logic::cursor_ptr_type sessions_cursor_ptr =
			this->logic_gi().connection().query(
				this->logic_gi().collection_sessions(),
				MONGO_QUERY("session_id"s << session_id),	// Search by session id (NOT _id!!!)
				1											// Need only 1 BSON object
			);
		
		if (sessions_cursor_ptr == nullptr)
			throw logic::incorrect_cursor{};
		
		if (sessions_cursor_ptr->more())	// Duplicate found, regenerating session id
			session_id.clear();	// Will continue, if attempts availible
	} while (session_id.empty() && attempts_availible > 0);
	
	if (session_id.empty())
		throw logic::cant_start_session{user_id};
	// After this we have unique session id
	
	
	std::time_t time_now = ::base::utc_time();
	
	mongo::BSONObjBuilder session_obj_builder;
	session_obj_builder.genOID();
	session_obj_builder.append("session_id"s,	session_id);
	session_obj_builder.append("user_id"s,		user_oid);
	
	{
		mongo::Date_t valid_until{static_cast<unsigned long long>(time_now + this->logic_gi().session_lifetime())};
		session_obj_builder.append("valid_until"s,	valid_until);
	}
	
	{
		mongo::Date_t restart_at{static_cast<unsigned long long>(time_now + this->logic_gi().session_restart_after())};
		session_obj_builder.append("restart_at"s,	restart_at);
	}
	
	{
		mongo::Date_t forget_at{static_cast<unsigned long long>(time_now + this->logic_gi().session_forget_after())};
		session_obj_builder.append("forget_at"s,	forget_at);
	}
	
	{
		mongo::Date_t started_at{static_cast<unsigned long long>(time_now)};
		session_obj_builder.append("started_at"s,	started_at);
	}
	
	mongo::BSONObj session_obj = session_obj_builder.done();
	mongo::OID session_oid = session_obj["_id"s].OID();	// Warning: NOT the same as session_id!
	
	
	// Insert session object
	this->logic_gi().connection().insert(
		this->logic_gi().collection_sessions(),
		session_obj
	);
	
	// Register session in user object
	this->logic_gi().connection().findAndModify(
		this->logic_gi().collection_users(),				// In users collection
		BSON("_id"s << user_oid),							// Find user by OID
		BSON("$push"s << BSON("sessions"s << session_oid))	// And append session OID to "sessions" array
	);
	
	
	return std::make_pair(std::move(user_obj["ref"s].str()), std::move(session_id));
}


// Restarts the session.
// Returns pair of user ref and new session id or throws.
std::pair<std::string, std::string>
logic::base::restart_session(const std::string &session_id,
							 const mongo::BSONObj &session_obj) const
{
	mongo::OID user_oid = session_obj["_id"s].OID();
	std::string user_id = user_oid.toString();
	
	logic::cursor_ptr_type users_cursor_ptr = this->logic_gi().connection().query(
		this->logic_gi().collection_users(),
		MONGO_QUERY("_id"s << user_oid),	// Search by user _id
		1									// Need only 1 BSON object
	);
	
	if (users_cursor_ptr == nullptr)
		throw logic::incorrect_cursor{};
	
	if (!users_cursor_ptr->more())
		throw logic::user_not_found{"For id: \""s + user_id + '\"'};
	
	this->finish_session(session_id);
	return this->start_session_for_obj_without_password_check(users_cursor_ptr->next());
}
