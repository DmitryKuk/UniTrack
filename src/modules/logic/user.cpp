// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logic/user.h>

#include <tuple>
#include <array>
#include <algorithm>
#include <functional>

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
	std::string current_user_id, session_cookie;
	std::tie(current_user_id, session_cookie) = this->continue_session(session_id);
	
	
	mongo::BSONObj user_obj;
	{
		static const mongo::BSONObj
			user_fields_to_return = BSON(
				"_id"s				<< 1 <<
				"ref"s				<< 1 <<
				"email"s			<< 1 <<
				"name"s				<< 1 <<
				"surname"s			<< 1 <<
				"last_visit_at"s	<< 1 <<
				"birthday"s			<< 1 <<
				"friends"s			<< 1
			);
		
		
		if (user_ref.empty())	// Info for current user
			user_obj = this->logic_gi().connection().findOne(
				this->logic_gi().collection_users(),
				MONGO_QUERY("_id"s << mongo::OID(current_user_id)),	// Search by _id
				&user_fields_to_return
			);
		else					// Info for specified user
			user_obj = this->logic_gi().connection().findOne(
				this->logic_gi().collection_users(),
				MONGO_QUERY("ref"s << user_ref),			// Search by ref
				&user_fields_to_return
			);
		
		
		if (user_obj.isEmpty())
			throw logic::user_not_found{"For id: \""s + current_user_id + '\"'};
	}
	
	
	std::string user_info;
	user_info.reserve(4096);
	
	
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
	
	
	// Adding status
	user_info += "{\"status\":\"ok\""s;
	
	// Adding ref
	add_info_by_key(',', "ref"s, user_obj);
	
	for (const std::string &key: {"name"s, "surname"s})
		add_info_by_key(',', key, user_obj);
	
	// Adding last visit time
	add_info(',', "last_visit_at"s, mongo::dateToISOStringUTC(user_obj["last_visit_at"s].Date()));
	
	// Adding email for user, if it is his email
	if (user_obj["_id"s].OID().toString() == current_user_id)
		add_info_by_key(',', "email"s, user_obj);
	
	// Adding user birthday, if it is set
	{
		std::string birthday = user_obj["birthday"s].str();
		if (!birthday.empty())
			add_info(',', "birthday"s, birthday);
	}
	
	
	// Adding up to max_friends random friends
	{
		using index_type = unsigned int;
		constexpr index_type max_friends = 6;
		
		
		auto friends = user_obj["friends"s].Array();
		if (!friends.empty()) {	// If user has friends
			std::array<index_type, max_friends> indexes;
			
			// Generating up to max_friends unique random indexes
			{
				const index_type friends_size = static_cast<index_type>(friends.size());
				std::uniform_int_distribution<index_type> distribution{0, friends_size};
				
				auto first_it = indexes.begin();
				for (index_type i = 0, i_max = std::min(friends_size, max_friends); i < i_max; ++i) {
					auto current_it = first_it + i;
					do {	// Regenerating, while not unique
						*current_it = distribution(this->generator_);
					} while (std::find(first_it, current_it, *current_it) != current_it);
				}
			}
			
			
			user_info += ",\"friends\":["s;	// Friends array begin
				
			for (const auto index: indexes) {
				static const mongo::BSONObj
					friend_fields_to_return = BSON(
						"_id"s				<< 0 <<	// Don't include _id
						"ref"s				<< 1 <<
						"name"s				<< 1 <<
						"surname"s			<< 1 <<
						"last_visit_at"s	<< 1
					);
				
				
				// Searching friend with generated index
				mongo::BSONObj friend_obj = this->logic_gi().connection().findOne(
					this->logic_gi().collection_users(),
					MONGO_QUERY("_id"s << friends[index].OID()),	// Search by _id
					&friend_fields_to_return						// Return only specified fields
				);
				
				if (friend_obj.isEmpty())
					throw logic::user_not_found{"For id: \""s + friends[index].OID().toString() + '\"'};
				
				
				// Add friend objects separator
				if (user_info.back() == '}')
					user_info += ',';
				
				// Add json string for current friend
				add_info_by_key('{', "ref"s, friend_obj);
				for (const std::string &key: {"name"s, "surname"s})
					add_info_by_key(',', key, friend_obj);
				add_info(',', "last_visit_at"s, mongo::dateToISOStringUTC(friend_obj["last_visit_at"s].Date()));
				user_info += '}';
			}
			
			user_info += ']';				// Friends array end
		}
	}
	
	user_info += '}';
	
	
	return std::make_pair(std::move(user_info), std::move(session_cookie));
}
