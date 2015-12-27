// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logic/users.h>

#include <mongo/bson/bsonobj.h>
#include <mongo/bson/bsonobjbuilder.h>

#include <base/time.h>
#include <logger/logger.h>
#include <logic/exceptions.h>

using namespace std::literals;


// Returns users json string and new session cookie or throws.
// If session is valid, session cookie is empty.
std::pair<std::string, std::string>
logic::users::list_users(const std::string &session_id) const
{
	std::string session_cookie;
	session_cookie = this->continue_session(session_id).second;
	
	
	static const mongo::BSONObj
		user_fields_to_return = BSON(
			"_id"s				<< 0 <<
			"ref"s				<< 1 <<
			"name"s				<< 1 <<
			"surname"s			<< 1 <<
			"last_visit_at"s	<< 1 <<
			"birthday"s			<< 1
		);
	
	logic::cursor_ptr_type user_cursor_ptr = this->logic_gi().connection().query(
		this->logic_gi().collection_users(),
		mongo::BSONObj{},						// Find all
		0,										// Return all
		0,										// Skip 0
		&user_fields_to_return
	);
	
	if (user_cursor_ptr == nullptr)
		throw logic::incorrect_cursor{};
	
	
	std::string users_json_str = "{\"status\":\"ok\""s;
	
	const auto add_info =
		[&](char separator, const std::string &key, const std::string &value)
		{
			users_json_str += separator;
			users_json_str += '\"';
			users_json_str += key;
			users_json_str += "\":\""s;
			users_json_str += value;
			users_json_str += '\"';
		};
	
	const auto add_info_by_key =
		[&](char separator, const std::string &key, const mongo::BSONObj &obj)
		{
			add_info(separator, key, obj[key].str());
		};
	
	
	if (user_cursor_ptr->more()) {
		users_json_str += ",\"users\":["s;
		
		while (user_cursor_ptr->more()) {
			mongo::BSONObj user_obj = user_cursor_ptr->nextSafe();
			
			
			add_info_by_key('{', "ref"s, user_obj);
			
			for (const std::string &key: {"name"s, "surname"s})
				add_info_by_key(',', key, user_obj);
			
			add_info(',', "last_visit_at"s, mongo::dateToISOStringUTC(user_obj["last_visit_at"s].Date()));
			
			// Adding user birthday, if it is set
			{
				std::string birthday = user_obj["birthday"s].str();
				if (!birthday.empty())
					add_info(',', "birthday"s, birthday);
			}
			
			users_json_str += '}';	// End of user object
		}
		
		users_json_str += ']';	// End of users list
	}
	
	users_json_str += '}';	// End of object
	
	
	return std::make_pair(std::move(users_json_str), std::move(session_cookie));
}
