// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logic/registration.h>

#include <logger/logger.h>
#include <server/protocol/http/utils.h>


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


void
logic::registration::register_user(const std::string &data) const
{
	std::unordered_map<std::string, std::string> map;
	std::unordered_set<std::string> set;
	
	logger::stream(logger::level::info)
		<< "New user registration (" << data.size() << "): \"" << data << "\".";
	
	bool parsed = server::protocol::http::decode_uri_args(
		data,
		[&](std::string &&key, std::string &&value) { map.emplace(std::move(key), std::move(value)); },
		[&](std::string &&key) { set.insert(std::move(key)); }
	);
	
	if (parsed) {
		auto &&stream = logger::stream(logger::level::info);
		stream << " Parsed:";
		
		for (const auto &x: map)
			stream << " [" << x.first << ": " << x.second << ']';
		
		for (const auto &x: set)
			stream << " {" << x << '}';
	}
}
