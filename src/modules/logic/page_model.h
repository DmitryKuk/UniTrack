// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGIC_PAGE_MODEL_H
#define LOGIC_PAGE_MODEL_H

#include <string>
#include <unordered_map>

#include <logger/logger.h>
#include <templatizer/model.h>


namespace logic {


class page_model:
	public templatizer::model,
	public std::unordered_map<std::string, std::string>
{
public:
	// Throws templatizer::variable_not_found exception, if variable does not exist
	// in the model.
	virtual const std::string & variable(const std::string &var_name) const override;
};	// class page_model


};	// namespace logic


#endif	// LOGIC_PAGE_MODEL_H
