// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGIC_REGISTRATION_H
#define LOGIC_REGISTRATION_H

#include <string>
#include <unordered_map>

#include <logic/base.h>


namespace logic {


class registration:
	public ::logic::base
{
public:
	using form = std::unordered_map<std::string, std::string>;
	
	
	// Constructor
	using ::logic::base::base;
	
	
	// Registers new user using data from registration form.
	// Returns user ref and session cookie, if user registered successfully.
	// Otherwise, throws.
	std::pair<std::string, std::string>
	register_user(const form &form) const;
};	// class registration


};	// namespace logic


#endif	// LOGIC_REGISTRATION_H
