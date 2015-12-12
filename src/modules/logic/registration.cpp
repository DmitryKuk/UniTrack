// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logic/registration.h>

#include <mongo/bson/bsonobj.h>
#include <mongo/bson/bsonobjbuilder.h>
#include <mongo/bson/bsonelement.h>

#include <logger/logger.h>

using namespace std::literals;


// virtual
logic::page_model
logic::registration::page_model(const server::protocol::http::request &request,
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


bool
logic::registration::register_user(const logic::registration::form &fields,
								   std::string &user_id,
								   std::string &session_id) const
{
	try {
		mongo::BSONObjBuilder bson_obj_builder;
		bson_obj_builder.genOID();
		for (const std::string &key: {"name"s, "surname"s}) {
			const std::string &value = fields.at(key);
			bson_obj_builder.append(key, value);
		}
		
		mongo::BSONObj bson_obj = bson_obj_builder.done();
		mongo::BSONElement e;
		
		bson_obj.getObjectID(e);
		user_id = e.toString();
		
		this->logic_gi().connection().insert("unitrack.users"s, bson_obj);
	} catch (const std::out_of_range &) {
		return false;
	}
	return true;
}
