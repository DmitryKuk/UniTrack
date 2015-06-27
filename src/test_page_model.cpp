// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <iostream>
#include <string>

#include <logger/logger.h>

#include <page_model.h>


int main()
{
	logger::logger logger(std::clog);
	
	page_model model(logger);
	
	model.emplace("key", "value");
	
	if (model.variable("key") != "value")
		return 1;
	
	return 0;
}
