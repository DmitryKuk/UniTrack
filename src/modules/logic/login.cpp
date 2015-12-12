// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logic/login.h>

#include <mongo/bson/bsonobj.h>
#include <mongo/bson/bsonobjbuilder.h>
#include <mongo/bson/bsonelement.h>

#include <logger/logger.h>

using namespace std::literals;


// virtual
logic::page_model
logic::login::page_model(const server::protocol::http::request &request,
						 const boost::filesystem::path &path) const
{
	auto &&stream = logger::stream(logger::level::info);
	stream << "Here! " << request.path;
	
	
	logic::page_model model;
	
	model.emplace("USERNAME", "Dmitry Kukovinets");
	model.emplace("UNIVERSITY", "STANKIN");
	model.emplace("DEPARTMENT", "Inteh");
	model.emplace("TAGS", "Student");
	
	return model;
}


void
logic::login::login_user(const logic::login::form &fields,
						 std::string &user_id,
						 std::string &session_id) const
{
	mongo::BSONObjBuilder bson_obj_builder;
	bson_obj_builder.genOID();
	for (const std::string &key: {"name"s, "surname"s}) {
		const std::string &value = fields.at(key);
		bson_obj_builder.append(key, value);
	}
	
	mongo::BSONObj bson_obj = bson_obj_builder.done();
	mongo::BSONElement e;
	
	bson_obj.getObjectID(e);
	user_id = e.OID().toString();
	logger::log(logger::level::info, user_id);
	
	this->logic_gi().connection().insert("unitrack.users"s, bson_obj);
}
