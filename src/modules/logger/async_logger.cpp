// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logger/async_logger.h>

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cerrno>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>

#include <base/json_utils.h>
#include <logger/exceptions.h>


namespace {


const int		regular_file_open_flags	= O_WRONLY | O_CREAT | O_TRUNC;
const mode_t	regular_file_open_mode	= S_IFREG | S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH;

const int		fifo_open_flags			= O_WRONLY | O_TRUNC;
const mode_t	fifo_create_mode		= S_IFIFO | S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH;


};	// namespace


// explicit
logger::async_logger::parameters::parameters(const nlohmann::json &json):
	::logger::parameters{json}
{
	try {
		const auto &async_logger_json = base::json_utils::at(json, "async_logger");
		
		{
			std::string str;
			
			if (base::json_utils::extract(async_logger_json, str, "file_type")) {
				if (str == "regular")	this->file_type = file_type::regular;
				else if (str == "fifo")	this->file_type = file_type::fifo;
				else throw ::logger::incorrect_config{"Unknown file_type: \"" + str
													  + "\", correct values are: \"regular\", \"fifo\""};
			}
		}
		
		base::json_utils::extract(async_logger_json, this->max_queue_size, "max_queue_size");
	} catch (...) {}	// Using default parameters
}


logger::async_logger::async_logger(boost::asio::io_service &io_service,
								   const ::logger::async_logger::parameters &parameters):
	parameters_{parameters},
	pid_str_{std::to_string(::getpid())},
	log_file_number_{0},
	messages_logged_{0},
	
	// stream_{io_service},
	
	sending_{false}
{
	using namespace std::literals;
	
	std::cerr << "Here: " << __FILE__ << ": " << __LINE__ << std::endl;
	this->stream_ = std::make_unique<boost::asio::posix::stream_descriptor>(io_service);
	std::cerr << "Here: " << __FILE__ << ": " << __LINE__ << std::endl;
	
	if (!boost::filesystem::exists(this->parameters_.root))
		throw ::logger::incorrect_config{"Root directory not exists: \""s
										 + this->parameters_.root.string() + '\"'};
	
	if (!boost::filesystem::is_directory(this->parameters_.root))
		throw ::logger::incorrect_config{"Root path should be directory: \""s
										 + this->parameters_.root.string() + '\"'};
	
	switch (this->parameters_.file_type) {
		case parameters::file_type::regular: case parameters::file_type::fifo:
			break;
		default:
			throw ::logger::incorrect_config{"Unknown file_type, correct values are: \"regular\", \"fifo\". "
											 "Strange, because parameters was already constructed"s};
	}
	
	
	if (this->parameters_.file_type == parameters::file_type::fifo) {
		this->current_file_path_ = this->parameters_.root / (this->pid_str_ + ".log.fifo"s);
		if (::mkfifo(this->current_file_path_.c_str(), fifo_create_mode) != 0)
			throw ::logger::cant_create_fifo{'\"' + this->current_file_path_.string()
											 + "\": "s + std::strerror(errno)};
		
		int fd = ::open(this->current_file_path_.c_str(), fifo_open_flags);
		if (fd < 0)
			throw ::logger::cant_open_fifo{'\"' + this->current_file_path_.string()
										   + "\": "s + std::strerror(errno)};
		
		(*this->stream_) = boost::asio::posix::stream_descriptor{(*this->stream_).get_io_service(), fd};
	} else {
		this->start_next_file();
	}
}


logger::async_logger::~async_logger()
{
	try {
		this->stream(logger::level::info)
			<< "End of log.";
	} catch (...) {}
	
	this->try_flush();
	(*this->stream_).close();
	
	if (this->parameters_.file_type == parameters::file_type::fifo) {
		boost::system::error_code err;
		boost::filesystem::remove(this->current_file_path_, err);	// Ignore errors
	}
}


logger::stream<::logger::async_logger>
logger::async_logger::stream(::logger::level level, bool add_level_prefix) noexcept
{
	return {*this, level, add_level_prefix};
}


void
logger::async_logger::log_raw(::logger::level /* level */, const std::string &data) noexcept
{
	this->log_queue_.push(data + '\n');
	this->add_write_handler_or_write();
}


void
logger::async_logger::log_raw(::logger::level /* level */, std::string &&data) noexcept
{
	data += '\n';
	this->log_queue_.push(std::move(data));
	this->add_write_handler_or_write();
}


bool
logger::async_logger::try_flush() noexcept
{
	if (this->sending_)	// Can't flush
		return false;
	
	
	while (true) {
		if (this->messages_logged_ >= this->parameters_.max_log_size)
			this->start_next_file();
		
		if (!this->force_flush_first()) break;
	}
	
	
	return true;
}


// private
bool
logger::async_logger::force_flush_first() noexcept
{
	if (this->log_queue_.empty())
		return false;
	
	auto buffer = boost::asio::buffer(this->log_queue_.front());
	boost::system::error_code err;
	boost::asio::write((*this->stream_), buffer, err);	// Ignore errors: logging is impossible
	
	this->log_queue_.pop();
	
	if (this->parameters_.file_type == parameters::file_type::regular)	// Don't need count messages
		++this->messages_logged_;
	
	return true;
}


void
logger::async_logger::start_next_file() noexcept
{
	using namespace std::literals;
	
	
	size_t old_log_file_number = this->log_file_number_;
	while (true) {
		this->current_file_path_ = this->parameters_.root
								   / (this->pid_str_ + '_' + std::to_string(this->log_file_number_) + ".log"s);
		++this->log_file_number_;
		
		
		// Try to open chosen file
		int fd = ::open(this->current_file_path_.c_str(), regular_file_open_flags, regular_file_open_mode);
		if (fd > 0) {	// Normal
			(*this->stream_) = boost::asio::posix::stream_descriptor{(*this->stream_).get_io_service(), fd};
			break;
		}
		
		
		if (this->log_file_number_ != old_log_file_number) {	// Try open one of next files
			continue;
		} else {												// All files tested :(
			if ((*this->stream_).is_open()) {
				const std::string last_message = "Unable to open any of next (and previous) log files. It is "
												 "the worst scenario. I don\'t know, what to, it\'s really "
												 "terrible. Good bye!"s;
				
				auto buffer = boost::asio::buffer(last_message);
				boost::system::error_code err;
				boost::asio::write((*this->stream_), buffer, err);	// Ignore errors: logging is impossible
			}
			
			std::abort();
		}
	}
	
	
	try {
		this->stream(logger::level::info)
			<< "Log file started: pid: " << this->pid_str_ << ", file number: " << this->log_file_number_;
	} catch (...) {}
}


// Implicit async execution flow!
void
logger::async_logger::add_write_handler_or_write()
{
	if (this->sending_ || this->log_queue_.empty() || !(*this->stream_).is_open())
		return;
	
	if (this->log_queue_.size() < this->parameters_.max_queue_size) {	// Normal
		auto buffer = boost::asio::buffer(this->log_queue_.front());
		
		using namespace std::placeholders;
		boost::asio::async_write((*this->stream_), buffer,
								 std::bind(&::logger::async_logger::write_handler, this, _1, _2));
		
		this->sending_ = true;
	} else {															// Queue overflow
		this->try_flush();
	}
}


void
logger::async_logger::write_handler(const boost::system::error_code & /* err */, size_t /* bytes_transferred */)
{
	// Ignore errors: logging is impossible
	this->sending_ = false;
	this->log_queue_.pop();
	
	if (this->parameters_.file_type == parameters::file_type::regular)	// Don't need count messages
		++this->messages_logged_;
	
	this->add_write_handler_or_write();
}
