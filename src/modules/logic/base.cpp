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
// Returns pair of user ref and session cookie or throws.
std::pair<std::string, std::string>
logic::base::start_session_for_email(const std::string &user_email,
									 const std::string &user_password) const
{
	static const mongo::BSONObj
		user_fields_to_return = BSON(
			"_id"s << 1 <<
			"ref"s << 1 <<
			"password"s << 1
		);
	
	
	// Find user
	mongo::BSONObj user_obj = this->logic_gi().connection().findOne(
		this->logic_gi().collection_users(),
		MONGO_QUERY("email"s << user_email),	// Search by email
		&user_fields_to_return
	);
	
	if (user_obj.isEmpty())
		throw logic::user_not_found{"For email: \""s + user_email + "and password: \""s + user_password + '\"'};
	
	
	return this->start_session_for_obj(user_obj, user_password);
}


// Continues user session.
// Returns pair of user id and session cookie or throws.
// If session is valid, session cookie is empty.
std::pair<std::string, std::string>
logic::base::continue_session(const std::string &session_id) const
{
	static const mongo::BSONObj
		session_fields_to_return = BSON(
			"user_id"s		<< 1 <<
			"valid_until"s	<< 1 <<
			"restart_at"s	<< 1
		);
	
	
	mongo::BSONObj session_obj = this->logic_gi().connection().findOne(
		this->logic_gi().collection_sessions(),
		MONGO_QUERY("session_id"s << session_id),	// Search by session id (NOT _id!!!)
		&session_fields_to_return
	);
	
	
	if (!session_obj.isEmpty()) {	// Session found
		mongo::Date_t time_now{static_cast<unsigned long long>(::base::utc_time())};
		
		if (time_now < session_obj["valid_until"s].Date()) {	// If session is valid
			if (time_now < session_obj["restart_at"s].Date()) {	// Don't need to restart
				// Update last_visit_at
				this->logic_gi().connection().findAndModify(
					this->logic_gi().collection_users(),				// In users collection
					BSON("_id"s << session_obj["user_id"s].OID()),		// Find user by OID
					BSON("$set"s << BSON("last_visit_at"s << time_now))	// Set last_visit_at to current time
				);
				
				// And return current session
				return std::make_pair(session_obj["user_id"s].OID().toString(), ""s);
			} else {														// Need to restart session
				return this->restart_session(session_id, session_obj);
			}
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
		BSON("$min"s << BSON("valid_until"s	<< time_now)),	// Update if session is valid
		false,												// Not upsert
		false,												// Return old version
		mongo::BSONObj{},									// Not sort
		BSON("_id"s << 1 << "user_id"s << 1)				// Return _id and user_id fields
	);
	
	// Check session_obj for errors
	if (session_obj.isEmpty())
		throw logic::session_not_found{session_id};
	
	if (session_obj.hasField("$err"s))
		throw logic::session_error{session_id, session_obj["$err"s].String()};
	
	
	// Unregister session in user object
	this->logic_gi().connection().findAndModify(
		this->logic_gi().collection_users(),								// In users collection
		BSON("_id"s << session_obj["user_id"s].OID()),						// Find user by OID
		BSON("$pull"s << BSON("sessions"s << session_obj["_id"s].OID()) <<	// Remove session OID from "sessions" array
			 "$set"s << BSON("last_visit_at"s << time_now))					// Set last_visit_at to current time
	);
}


// private
// Starts a new session for user.
// Returns pair of user ref and session cookie or throws.
std::pair<std::string, std::string>
logic::base::start_session_for_obj(const mongo::BSONObj &user_obj,
								   const std::string &user_password) const
{
	// Password check
	if (user_obj["password"s].str() != user_password)
		throw logic::password_not_match(user_obj["_id"s].OID().toString(), user_password);
	
	return this->start_session_for_obj_without_password_check(user_obj);
}


// Starts a new session for user.
// Returns pair of user ref and session cookie or throws.
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
		
		mongo::BSONObj session_obj = this->logic_gi().connection().findOne(
			this->logic_gi().collection_sessions(),
			MONGO_QUERY("session_id"s << session_id)	// Search by session id (NOT _id!!!)
		);
		
		if (!session_obj.isEmpty())	// Duplicate found, regenerating session id
			session_id.clear();	// Will continue, if attempts availible
	} while (session_id.empty() && attempts_availible > 0);
	
	if (session_id.empty())
		throw logic::cant_start_session{user_id};
	// After this we have unique session id
	
	
	std::time_t time_now = ::base::utc_time();
	
	std::string session_cookie = "sid="s + session_id + "; expires="s
								 + ::base::time_for_cookie(time_now + this->logic_gi().session_lifetime()) + ';';
	
	mongo::Date_t last_visit_at{static_cast<unsigned long long>(time_now)};
	
	
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
		const auto &started_at = last_visit_at;
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
		this->logic_gi().collection_users(),						// In users collection
		BSON("_id"s << user_oid),									// Find user by OID
		BSON("$push"s << BSON("sessions"s << session_oid) <<		// Append session OID to "sessions" array
			 "$set"s << BSON("last_visit_at"s << last_visit_at))	// Set last_visit_at to current time
	);
	
	
	return std::make_pair(std::move(user_obj["ref"s].str()), std::move(session_cookie));
}


// Restarts the session.
// Returns pair of user ref and new session cookie or throws.
std::pair<std::string, std::string>
logic::base::restart_session(const std::string &session_id,
							 const mongo::BSONObj &session_obj) const
{
	mongo::OID user_oid = session_obj["user_id"s].OID();
	
	mongo::BSONObj user_obj = this->logic_gi().connection().findOne(
		this->logic_gi().collection_users(),
		MONGO_QUERY("_id"s << user_oid)	// Search by user _id
	);
	
	if (user_obj.isEmpty())
		throw logic::user_not_found{"For id: \""s + user_oid.toString() + '\"'};
	
	this->finish_session(session_id);
	return this->start_session_for_obj_without_password_check(user_obj);
}
