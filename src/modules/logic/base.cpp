// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logic/base.h>

#include <memory>

#include <mongo/bson/bsonobj.h>
#include <mongo/bson/bsonobjbuilder.h>

#include <logger/logger.h>
#include <logic/exceptions.h>

using namespace std::literals;


// Starts a new session for user by id.
// Returns session id or throws.
std::string
logic::base::start_session(const std::string &user_id) const
{
	// WARNING! This section must be atomic!
	// Search for existing session
	{
		logic::cursor_ptr_type sessions_cursor_ptr =
			this->logic_gi().connection().query(
				this->logic_gi().collection_sessions(),
				MONGO_QUERY("user_id"s << user_id),	// Search by user id
				1									// Need only 1 BSON object
			);
		
		if (sessions_cursor_ptr == nullptr)
			throw logic::cant_create_cursor{};
		
		if (sessions_cursor_ptr->more()) {	// Previous session found
			std::string session_id = sessions_cursor_ptr->next()["_id"s].OID().toString();
			logger::log(logger::level::info, "Logic: Continue session: " + session_id);
			return std::move(session_id);
		}
	}
	
	// Creating new session
	{
		mongo::BSONObjBuilder session_obj_builder;
		session_obj_builder.genOID();
		session_obj_builder.append("user_id"s, user_id);
		
		mongo::BSONObj session_obj = session_obj_builder.done();
		std::string session_id = session_obj["_id"s].OID().toString();
		
		this->logic_gi().connection().insert(this->logic_gi().collection_sessions(), session_obj);
		
		logger::log(logger::level::info, "Logic: Starting new session: " + session_id);
		return std::move(session_id);
	}
	// WARNING! Until this!
}


// Starts a new session for user by login and password.
// Returns pair of user id and session id or throws.
std::pair<std::string, std::string>
logic::base::start_session(const std::string &user_login,
						   const std::string &user_password) const
{
	logic::cursor_ptr_type users_cursor_ptr =
		this->logic_gi().connection().query(
			this->logic_gi().collection_users(),
			MONGO_QUERY("login"s << user_login),	// Search by login
			1										// Need only 1 BSON object
		);
	
	if (users_cursor_ptr == nullptr)
		throw logic::cant_create_cursor{};
	
	if (!users_cursor_ptr->more())
		throw logic::login_not_found{user_login, user_password};
	
	mongo::BSONObj user_obj = users_cursor_ptr->next();
	if (user_obj["password"s].str() != user_password)
		throw logic::password_not_match(user_login, user_password);
	
	std::string user_id = user_obj["_id"s].OID().toString();
	
	return std::make_pair(std::move(user_id), this->start_session(user_id));
}


// Continues user session.
// Returns correct user id or throws.
std::string
logic::base::continue_session(const std::string &session_id) const
{
	logic::cursor_ptr_type sessions_cursor_ptr =
		this->logic_gi().connection().query(
			this->logic_gi().collection_sessions(),
			MONGO_QUERY("_id"s << session_id),	// Search by session id
			1								// Need only 1 BSON object
		);
	
	if (sessions_cursor_ptr == nullptr)
		throw logic::cant_create_cursor{};
	
	if (sessions_cursor_ptr->more())	// Previous session found
		return sessions_cursor_ptr->next()["user_id"s].str();
	
	throw logic::session_not_found{session_id};
}


// Finishes user session.
void
logic::base::finish_session(const std::string &session_id) const
{
	mongo::BSONObjBuilder session_obj_builder;
	session_obj_builder.append("_id"s, session_id);
	
	mongo::BSONObj session_obj = session_obj_builder.done();
	this->logic_gi().connection().remove(this->logic_gi().collection_sessions(), session_obj);
}
