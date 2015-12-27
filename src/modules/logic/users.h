// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGIC_USERS_H
#define LOGIC_USERS_H

#include <string>
#include <utility>

#include <logic/base.h>


namespace logic {


class users:
	public ::logic::base
{
public:
	// Constructor
	using ::logic::base::base;
	
	
	// Returns users json string and new session cookie or throws.
	// If session is valid, session cookie is empty.
	std::pair<std::string, std::string> list_users(const std::string &session_id) const;
};	// class users


};	// namespace logic


#endif	// LOGIC_USERS_H
