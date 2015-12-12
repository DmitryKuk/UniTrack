// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logic/registration.h>

#include <mongo/bson/bsonobj.h>
#include <mongo/bson/bsonobjbuilder.h>

#include <logger/logger.h>
#include <logic/exceptions.h>

using namespace std::literals;


void
logic::registration::register_user(const logic::registration::form &fields,
								   std::string &user_id,
								   std::string &session_id) const
{
	mongo::BSONObjBuilder bson_obj_builder;
	bson_obj_builder.genOID();
	for (const std::string &key: {"name"s, "surname"s, "email"s, "password"s}) {
		const std::string &value = fields.at(key);
		if (value.empty())
			throw logic::incorrect_form{"Field empty: \""s + key + '\"'};
		
		bson_obj_builder.append(key, value);
	}
	
	mongo::BSONObj user_obj = bson_obj_builder.done();
	
	user_id = user_obj["_id"s].OID().toString();
	session_id = this->start_session(user_id);
	
	this->logic_gi().connection().insert(this->logic_gi().collection_users(), user_obj);
}
