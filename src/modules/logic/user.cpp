// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logic/user.h>

#include <tuple>
#include <algorithm>

#include <logic/exceptions.h>

using namespace std::literals;


logic::user::user(::logic::global_instance &logic_gi):
	::logic::base{logic_gi},
	
	generator_{logic_gi.random_device()()}	// Init random generator
{}


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
	std::string user_info;
	user_info.reserve(4096);
	user_info += "{\"status\":\"ok\""s;
	
	const auto add_info =
		[&](char separator, const std::string &key, const std::string &value)
		{
			user_info += separator;
			user_info += '\"';
			user_info += key;
			user_info += "\":\""s;
			user_info += value;
			user_info += '\"';
		};
	
	const auto add_info_by_key =
		[&](char separator, const std::string &key, const mongo::BSONObj &obj)
		{
			add_info(separator, key, obj[key].str());
		};
	
	// Adding location
	add_info_by_key('{', "ref"s, user_obj);
	
	for (const std::string &key: {"name"s, "surname"s})
		add_info_by_key(',', key, user_obj);
	
	// Adding email for user, if it is his email
	if (user_obj["_id"s].OID().toString() == user_id)
		add_info_by_key(',', "email"s, user_obj);
	
	// Adding user birthday, if it is set
	{
		std::string birthday = user_obj["birthday"s].str();
		if (!birthday.empty())
			add_info(',', "birthday"s, birthday);
	}
	
	// Adding up to max_friends random friends
	{
		constexpr size_t max_friends = 6;
		
		auto friends = user_obj["friends"s].Array();
		if (!friends.empty()) {	// If user has friends
			std::uniform_int_distribution<size_t> distribution{0, friends.size()};
			
			user_info += '{';
			for (size_t i = 0, i_max = std::min(friends.size(), max_friends); i < i_max; ++i) {
				size_t index = distribution(this->generator_);
				
				// Searching friend with generated index
				::logic::cursor_ptr_type friend_cursor_ptr = this->logic_gi().connection().query(
					this->logic_gi().collection_users(),
					MONGO_QUERY("_id"s << friends[index].OID()),	// Search by _id
					1												// Need only 1 BSON object
				);
				
				if (friend_cursor_ptr == nullptr)
					throw logic::incorrect_cursor{};
				
				if (!friend_cursor_ptr->more())
					throw logic::user_not_found{"For id: \""s + user_id + '\"'};
				
				mongo::BSONObj friend_obj = friend_cursor_ptr->next();
				
				
				add_info_by_key('{', "ref"s, friend_obj);
				for (const std::string &key: {"name"s, "surname"s})
					add_info_by_key(',', key, friend_obj);
				user_info += '}';
			}
			user_info += '}';
		}
	}
	
	user_info += '}';
	
	
	return std::make_pair(std::move(user_info), std::move(session_cookie));
}
