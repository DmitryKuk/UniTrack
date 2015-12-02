// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logger/exceptions.h>


logger::error::error(const std::string &what_arg):
	std::logic_error(what_arg)
{}


logger::incorrect_config::incorrect_config(const std::string &what_arg):
	logger::error("Incorrect logger config: " + what_arg)
{}


logger::cant_create_fifo::cant_create_fifo(const std::string &what_arg):
	logger::error("Can't create FIFO: " + what_arg)
{}


logger::cant_open_fifo::cant_open_fifo(const std::string &what_arg):
	logger::error("Can't open FIFO: " + what_arg)
{}
