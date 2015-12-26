// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logic/remove_friend.h>

#include <mongo/bson/bsonobj.h>
#include <mongo/bson/bsonobjbuilder.h>

#include <base/time.h>
#include <logger/logger.h>
#include <logic/exceptions.h>

using namespace std::literals;


// Returns new session cookie or throws.
// If session is valid, session cookie is empty.
std::string
logic::remove_friend::remove_friend_(const std::string &friend_ref, const std::string &session_id) const
{
	std::string current_user_id, session_cookie;
	std::tie(current_user_id, session_cookie) = this->continue_session(session_id);
	
	mongo::OID current_user_oid = mongo::OID(current_user_id);
	
	
	static const mongo::BSONObj friend_fields_to_return = BSON("_id"s << 1);
	
	mongo::BSONObj friend_obj = this->logic_gi().connection().findAndModify(
		this->logic_gi().collection_users(),
		BSON("ref"s << friend_ref),										// Search by user ref
		BSON("$pop"s << BSON("friend_of"s << current_user_oid) <<		// Update friend of set
			 "$pop"s << BSON("incoming_friends"s << current_user_oid)),	// Update incoming fields set
		false,															// Not upsert
		false,															// Return old version
		mongo::BSONObj{},												// Not sort
		friend_fields_to_return
	);
	
	if (friend_obj.isEmpty())
		throw logic::user_not_found{"For ref: \""s + friend_ref + '\"'};
	
	
	mongo::Date_t time_now{static_cast<unsigned long long>(::base::utc_time())};
	this->logic_gi().connection().findAndModify(
		this->logic_gi().collection_users(),							// In users collection
		BSON("_id"s << current_user_oid),								// Find user by OID
		BSON("$pop"s << BSON("friends"s << friend_obj["_id"].OID()) <<	// Add friend to friends of current user
			 "$set"s << BSON("last_visit_at"s << time_now))				// Set last_visit_at to current time
	);
	
	
	return std::move(session_cookie);
}
