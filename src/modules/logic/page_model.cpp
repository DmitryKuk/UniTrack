// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logic/page_model.h>

#include <stdexcept>

#include <templatizer/model_exceptions.h>


const std::string &
logic::page_model::variable(const std::string &var_name) const
{
	try {
		return this->std::unordered_map<std::string, std::string>::at(var_name);
	} catch (const std::out_of_range &) {
		throw templatizer::variable_not_found(var_name);
	}
}
