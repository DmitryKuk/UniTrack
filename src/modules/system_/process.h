// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SYTEM_PROCESS_H
#define SYTEM_PROCESS_H

#include <unistd.h>


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
	
	inline process(process &&other) noexcept;	// Move-constructable
	
	process(const process &other) = delete;		// But non-copy-constructable
	
	
	inline id get_id() const noexcept;
private:
	id id_ = 0;
};	// class process


};	// namespace system_


#include <system_/process.hpp>

#endif	// SYTEM_PROCESS_H
