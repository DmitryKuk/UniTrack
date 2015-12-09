// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGIC_PAGE_MODEL_GENERATOR_H
#define LOGIC_PAGE_MODEL_GENERATOR_H

#include <boost/filesystem/path.hpp>

#include <server/protocol/http/request.h>
#include <logic/base.h>
#include <logic/page_model.h>


namespace logic {


class page_model_generator:
	public ::logic::base
{
public:
	// Constructor
	using ::logic::base::base;
	
	
	virtual
	logic::page_model page_model(const server::protocol::http::request &request,
								 const boost::filesystem::path &path) const = 0;
};	// class page_model_generator


};	// namespace logic


#endif	// LOGIC_PAGE_MODEL_GENERATOR_H
