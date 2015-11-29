// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SYSTEM__PROCESS_H
#define SYSTEM__PROCESS_H

#include <unistd.h>
#include <signal.h>


namespace system_ {


class process
{
public:
	typedef pid_t id;
	
	
	// Executes nothing
	process() = default;
	
	// Executes fn(args...) in separate process. fn should return int, that will be process exit status
	// Constructs correct child process object (only in parent process!)
	// Throws: std::system_error, if it's impossible to create process (only in parent process!)
	// NEVER returns in child process!
	template<class Fn, class... Args>
	process(Fn &&fn, Args &&... args);
	
	// Move-constructible/assignable
	inline process(process &&other) noexcept;
	inline process & operator=(process &&other) noexcept;
	
	// But non-copy-constructible/assignable
	process(const process &other) = delete;
	process & operator=(const process &other) = delete;
	
	
	// In parent process: returns process id, given to child process.
	// In child process: returns 0. Call getpid() manually.
	inline id get_id() const noexcept;
	
	
	void kill(int signal = SIGTERM);
private:
	id id_ = 0;
};	// class process


};	// namespace system_


#include <system_/process.hpp>

#endif	// SYSTEM__PROCESS_H
