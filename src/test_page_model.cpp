// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <string>

#include <page_model.h>


int main()
{
	page_model model;
	
	model.emplace("key", "value");
	
	if (model.variable("key") != "value")
		return 1;
	
	return 0;
}
