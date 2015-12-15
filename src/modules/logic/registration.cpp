// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logic/registration.h>

#include <mongo/bson/bsonobj.h>
#include <mongo/bson/bsonobjbuilder.h>

#include <base/time.h>
#include <logger/logger.h>
#include <logic/exceptions.h>

using namespace std::literals;


void
logic::registration::register_user(const logic::registration::form &form,
								   std::string &user_id,
								   std::string &session_id) const
{
	mongo::BSONObjBuilder user_obj_builder;
	user_obj_builder.genOID();
	
	user_id = user_obj_builder.done()["_id"s].OID().toString();
	
	user_obj_builder.append("login"s, user_id);	// Default login is id (as string)
	
	for (const std::string &key: {"password"s, "email"s, "name"s, "surname"s}) {
		const std::string &value = form.at(key);
		if (value.empty())
			throw logic::incorrect_form{"Field empty: \""s + key + '\"'};
		
		user_obj_builder.append(key, value);
	}
	
	auto time_now = mongo::Date_t(static_cast<unsigned long long>(::base::utc_time()));
	user_obj_builder.append("last_visit_at"s, time_now);
	user_obj_builder.append("sessions"s, mongo::BSONArray{});
	user_obj_builder.append("registered_at"s, time_now);
	
	// Insert user object into users collection
	this->logic_gi().connection().insert(this->logic_gi().collection_users(), user_obj_builder.done());
	
	// Start new session for registered user
	session_id = this->start_session_for_id(user_id, form.at("password"s)).second;
}
