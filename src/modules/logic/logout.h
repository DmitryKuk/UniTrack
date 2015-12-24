// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGIC_LOGOUT_H
#define LOGIC_LOGOUT_H

#include <string>

#include <logic/base.h>


namespace logic {


class logout:
	public ::logic::base
{
public:
	// Constructor
	using ::logic::base::base;
	
	
	// Finishes user session.
	// If session is invalid, throws.
	inline void logout_user(const std::string &session_id) const;
};	// class logout


};	// namespace logic


#include <logic/logout.hpp>

#endif	// LOGIC_LOGOUT_H
