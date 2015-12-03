// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logger/logger.h>

#include <syslog.h>

using namespace std::literals;


logger::global_instance::global_instance(const std::string &log_name):
	log_name_{log_name}
{
	this->start_log();
}


logger::global_instance::global_instance(std::string &&log_name) noexcept:
	log_name_{std::move(log_name)}
{
	this->start_log();
}


logger::global_instance::~global_instance()
{
	logger::log(level::info, "End of log."s);
	
	::closelog();
}


// private
void
logger::global_instance::start_log() noexcept
{
	::openlog(this->log_name_.c_str(), LOG_PID | LOG_CONS | LOG_PERROR, LOG_USER);
	// ::setlogmask(LOG_UPTO(LOG_WARNING));
	
	logger::log(level::info, "Log started..."s);
}
