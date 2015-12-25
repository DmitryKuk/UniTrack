// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logic/registration.h>

#include <mongo/bson/bsonobj.h>
#include <mongo/bson/bsonobjbuilder.h>

#include <base/time.h>
#include <logger/logger.h>
#include <logic/exceptions.h>

using namespace std::literals;


// Registers new user using data from registration form.
// Returns user ref and session cookie, if user registered successfully.
// Otherwise, throws.
std::pair<std::string, std::string>
logic::registration::register_user(const logic::registration::form &form) const
{
	mongo::OID user_oid = mongo::OID::gen();
	std::string user_id = user_oid.toString();
	
	
	std::string user_ref;
	int attempts_availible = this->logic_gi().session_id_create_attempts();
	do {
		--attempts_availible;
		user_ref = this->logic_gi().generate_user_ref(user_id);
		
		logic::cursor_ptr_type users_cursor_ptr =
			this->logic_gi().connection().query(
				this->logic_gi().collection_users(),
				MONGO_QUERY("ref"s << user_ref),	// Search by user ref
				1									// Need only 1 BSON object
			);
		
		if (users_cursor_ptr == nullptr)
			throw logic::incorrect_cursor{};
		
		if (users_cursor_ptr->more())	// Duplicate found, regenerating session id
			user_ref.clear();	// Will continue, if attempts availible
	} while (user_ref.empty() && attempts_availible > 0);
	
	if (user_ref.empty())
		throw logic::cant_create_user_ref{user_id};
	// After this we have unique session id
	
	
	mongo::BSONObjBuilder user_obj_builder;
	user_obj_builder.append("_id"s, user_oid);
	user_obj_builder.append("ref"s, user_ref);
	
	for (const std::string &key: {"email"s, "password"s, "name"s, "surname"s}) {
		const std::string &value = form.at(key);
		if (value.empty())
			throw logic::incorrect_form{"Field empty: \""s + key + '\"'};
		
		user_obj_builder.append(key, value);
	}
	
	mongo::BSONArray empty_array;
	user_obj_builder.append("sessions"s,			empty_array);
	
	auto time_now = mongo::Date_t(static_cast<unsigned long long>(::base::utc_time()));
	user_obj_builder.append("last_visit_at"s,		time_now);
	user_obj_builder.append("registered_at"s,		time_now);
	
	user_obj_builder.append("birthday"s,			""s);	// Just empty string or YYYY-MM-DD
	
	user_obj_builder.append("friends"s,				empty_array);
	user_obj_builder.append("friend_of"s,			empty_array);
	user_obj_builder.append("incoming_friends"s,	empty_array);
	
	
	// Insert user object into users collection
	try {
		this->logic_gi().connection().insert(this->logic_gi().collection_users(), user_obj_builder.done());
	} catch (const mongo::OperationException &e) {
		throw logic::duplicate_user_found{e.what()};
	}
	
	// Start new session for registered user
	return this->start_session_for_email(form.at("email"s), form.at("password"s));
}
