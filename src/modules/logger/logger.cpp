// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logger/logger.h>

#include <syslog.h>


logger::logger::logger(const std::string &log_name):
	log_name_{log_name}
{
	this->start_log();
}


logger::logger::logger(std::string &&log_name) noexcept:
	log_name_{std::move(log_name)}
{
	this->start_log();
}


logger::logger::~logger()
{
	using namespace std::literals;
	this->log_raw(level::info, "End of log."s);
	
	::closelog();
}


// private
void
logger::logger::start_log() noexcept
{
	::openlog(this->log_name_.c_str(), LOG_PID | LOG_CONS | LOG_PERROR, LOG_USER);
	// ::setlogmask(LOG_UPTO(LOG_WARNING));
	
	using namespace std::literals;
	this->log_raw(level::info, "Log started..."s);
}
