// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGIC_LOGIN_H
#define LOGIC_LOGIN_H

#include <string>
#include <unordered_map>

#include <logic/base.h>


namespace logic {


class login:
	public ::logic::base
{
public:
	using form = std::unordered_map<std::string, std::string>;
	
	// Form should contain:
	// - email
	// - password
	
	
	// Constructor
	using ::logic::base::base;
	
	
	// Starts new session for user using data from login form.
	// Returns user ref and session cookie, if user logged in successfully.
	// Otherwise, throws.
	inline
	std::pair<std::string, std::string>
	login_user(const form &form) const;
};	// class login


};	// namespace logic


#include <logic/login.hpp>

#endif	// LOGIC_LOGIN_H
