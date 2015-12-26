// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGIC_REMOVE_FRIEND_H
#define LOGIC_REMOVE_FRIEND_H

#include <string>
#include <unordered_map>

#include <logic/base.h>


namespace logic {


class remove_friend:
	public ::logic::base
{
public:
	// Constructor
	using ::logic::base::base;
	
	
	// Returns new session cookie or throws.
	// If session is valid, session cookie is empty.
	std::string remove_friend_(const std::string &friend_ref, const std::string &session_id) const;
};	// class remove_friend


};	// namespace logic


#endif	// LOGIC_REMOVE_FRIEND_H
