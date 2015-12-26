// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGIC_ADD_FRIEND_H
#define LOGIC_ADD_FRIEND_H

#include <string>
#include <unordered_map>

#include <logic/base.h>


namespace logic {


class add_friend:
	public ::logic::base
{
public:
	// Constructor
	using ::logic::base::base;
	
	
	// Returns new session cookie or throws.
	// If session is valid, session cookie is empty.
	std::string add_friend_(const std::string &friend_ref, const std::string &session_id) const;
};	// class add_friend


};	// namespace logic


#endif	// LOGIC_ADD_FRIEND_H
