// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGIC_USER_H
#define LOGIC_USER_H

#include <string>
#include <utility>
#include <random>

#include <logic/base.h>


namespace logic {


class user:
	public ::logic::base
{
public:
	// Constructor
	user(::logic::global_instance &logic_gi);
	
	
	// Returns user information as json string and new session cookie or throws.
	// If session is valid, session cookie is empty.
	// If user_ref is empty, returns user info for current user (by session_id).
	std::pair<std::string, std::string>
	user_info(const std::string &user_ref, const std::string &session_id) const;
protected:
	mutable std::default_random_engine generator_;
};	// class user


};	// namespace logic


#endif	// LOGIC_USER_H
