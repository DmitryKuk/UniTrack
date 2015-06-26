// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef PAGE_MODEL_H
#define PAGE_MODEL_H

#include <string>
#include <unordered_map>

#include <logger/logger.h>
#include <templatizer/model.h>


class page_model:
	public templatizer::model,
	public std::unordered_map<std::string, std::string>
{
public:
	// Throws templatizer::variable_not_found exception, if variable does not exist
	// in the model.
	virtual const std::string & variable(const std::string &var_name) const override;
};	// class model


#endif // PAGE_MODEL_H
