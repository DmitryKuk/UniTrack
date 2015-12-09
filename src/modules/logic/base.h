// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGIC_BASE_H
#define LOGIC_BASE_H

#include <logic/global_instance.h>


namespace logic {


class base:
	public ::logic::enable_global_instance_ref
{
public:
	// Constructor
	using ::logic::enable_global_instance_ref::enable_global_instance_ref;
};	// class base


};	// namespace logic


#endif	// LOGIC_BASE_H
