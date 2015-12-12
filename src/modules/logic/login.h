// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGIC_LOGIN_H
#define LOGIC_LOGIN_H

#include <string>
#include <unordered_map>

#include <logic/page_model_generator.h>


namespace logic {


class login:
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
	
	
	// Logging in user using data from login form.
	// Sets user_id and session_id to correct values, if user logged insuccessfully.
	// Otherwise, throws.
	void login_user(const form &fields,
					std::string &user_id,
					std::string &session_id) const;
};	// class login


};	// namespace logic


#endif	// LOGIC_LOGIN_H
