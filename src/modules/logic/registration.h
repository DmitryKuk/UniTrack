// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGIC_USER_H
#define LOGIC_USER_H

#include <string>
#include <unordered_map>

#include <logic/page_model_generator.h>


namespace logic {


class registration:
	public ::logic::page_model_generator
{
public:
	using form = std::unordered_map<std::string, std::string>;
	
	
	// Constructor
	using ::logic::page_model_generator::page_model_generator;
	
	
	virtual
	logic::page_model
	page_model(const server::protocol::http::request &request,
			   const boost::filesystem::path &path) const override;
	
	
	// Registers new user using data from registration form.
	// Returns true and sets user_id and session_id to correct values, if user registered successfully.
	// Otherwise, returns false.
	bool register_user(const form &fields,
					   std::string &user_id,
					   std::string &session_id) const;
};	// class registration


};	// namespace logic


#endif	// LOGIC_USER_H
