// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <functional>
#include <system_error>
#include <cstring>


// Executes fn(args...) in separate process. fn should return int, that will be process exit status
// Constructs correct child process object (only in parent process!)
// Throws: std::system_error, if it's impossible to create process (only in parent process!)
// NEVER returns in child process!
template<class Fn, class... Args>
system_::process::process(Fn &&fn, Args &&... args)
{
	this->id_ = fork();
	if (this->id_ != 0) {
		// Parent process
		if (this->id_ > 0)	// Normal
			return;	// Return only in parent process!
		
		throw std::system_error{errno, std::system_category(), std::strerror(errno)};
	}
	
	
	// Child process
	int status = 0;
	
	try {
		status = std::bind<int>(std::move(fn), std::move(args)...)();
	} catch (...) {
		status = 1;
	}
	
	exit(status);
}


inline
system_::process::process(system_::process &&other) noexcept:
	id_(other.id_)
{
	other.id_ = 0;
}


inline
system_::process::id
system_::process::get_id() const noexcept
{
	return this->id_;
}
