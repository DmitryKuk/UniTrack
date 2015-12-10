// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logic/registration.h>


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
