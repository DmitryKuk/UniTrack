// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logic/user.h>

#include <tuple>

#include <logic/exceptions.h>

using namespace std::literals;


// Returns user information as json string and new session cookie or throws.
// If session is valid, session cookie is empty.
// If user_ref is empty, returns user info for current user (by session_id).
std::pair<std::string, std::string>
logic::user::user_info(const std::string &user_ref, const std::string &session_id) const
{
	std::string user_id, session_cookie;
	std::tie(user_id, session_cookie) = this->continue_session(session_id);
	
	
	logic::cursor_ptr_type users_cursor_ptr;
	if (user_ref.empty())	// Info for current user
		users_cursor_ptr = this->logic_gi().connection().query(
			this->logic_gi().collection_users(),
			MONGO_QUERY("_id"s << mongo::OID(user_id)),	// Search by _id
			1											// Need only 1 BSON object
		);
	else					// Info for specified user
		users_cursor_ptr = this->logic_gi().connection().query(
			this->logic_gi().collection_users(),
			MONGO_QUERY("ref"s << user_ref),			// Search by ref
			1											// Need only 1 BSON object
		);
	
	
	if (users_cursor_ptr == nullptr)
		throw logic::incorrect_cursor{};
	
	if (!users_cursor_ptr->more())
		throw logic::user_not_found{"For id: \""s + user_id + '\"'};
	
	
	mongo::BSONObj user_obj = users_cursor_ptr->nextSafe();
	std::string user_info = "{\"status\":\"ok\""s;
	
	// Adding location
	{
		user_info += ",\"location\":\"/user/"s;
		user_info += user_obj["ref"s].str();
		user_info += '\"';
	}
	
	// Adding email for user, if it is his email
	if (user_obj["_id"s].OID().toString() == user_id) {
		user_info += ",\"email\":\""s;
		user_info += user_obj["email"s].str();
		user_info += '\"';
	}
	
	for (const std::string &key: {"name"s, "surname"s}) {
		user_info += ",\""s;
		user_info += key;
		user_info += "\":\""s;
		user_info += user_obj[key].str();
		user_info += '\"';
	}
	user_info += '}';
	
	
	return std::make_pair(std::move(user_info), std::move(session_id));
}
