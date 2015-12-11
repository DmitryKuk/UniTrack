// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGIC_USER_H
#define LOGIC_USER_H

#include <logic/page_model_generator.h>


namespace logic {


class registration:
	public ::logic::page_model_generator
{
public:
	// Constructor
	using ::logic::page_model_generator::page_model_generator;
	
	
	virtual
	logic::page_model
	page_model(const server::protocol::http::request &request,
			   const boost::filesystem::path &path) const override;
	
	
	void register_user(const std::string &data) const;
};	// class registration


};	// namespace logic


#endif	// LOGIC_USER_H
