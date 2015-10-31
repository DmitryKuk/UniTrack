// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/model_exceptions.h>

#include <page_model.h>


int main()
{
	try {
		page_model model;
		
		// Fill the model
		model.emplace("key", "value");
		
		
		if (model.variable("key") != "value")
			return 10;
	} catch (const templatizer::variable_not_found &) {
		return 11;
	} catch (...) {
		return 12;
	}
	
	return 0;
}
