// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
#include <memory>
#include <stdexcept>

#include <boost/filesystem/path.hpp>

#include <logger/logger.h>
#include <logic/global_instance.h>
#include <server/server.h>
#include <project_config.h>


class application
{
public:
	application(int argc, char **argv) noexcept;
	
	// Non-copy/move constructable/assignable.
	application(const application &other) = delete;
	application(application &&other) = delete;
	application & operator=(const application &other) = delete;
	application & operator=(application &&other) = delete;
	
	
	// Returns status of execution
	inline int status() const noexcept;
	
	
	// Run application in interactive mode (reading std::cid and waiting until it will be closed)
	int run_interactive() noexcept;
protected:
	inline logger::logger & logger();
	inline logic::global_instance & logic();
private:
	void stop();
	
	void handle_error(const std::exception &e) noexcept;
	
	
	int status_;
	
	std::unique_ptr<logger::logger> logger_ptr_;
	std::unique_ptr<logic::global_instance> logic_ptr_;
	std::unique_ptr<server::host::manager> host_manager_ptr_;
	std::unique_ptr<server::server> server_ptr_;
};	// class application


#include <application.hpp>

#endif	// APPLICATION_H
