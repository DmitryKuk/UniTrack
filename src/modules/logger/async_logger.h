// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGGER_ASYNC_LOGGER_H
#define LOGGER_ASYNC_LOGGER_H

#include <queue>

#include <boost/asio/io_service.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>

#include <json.hpp>

#include <logger/level.h>
#include <logger/stream.h>
#include <logger/enable_logger_impl.h>
#include <logger/enable_logger_ref_impl.h>
#include <logger/parameters.h>


namespace logger {


class async_logger
{
public:
	struct parameters:
		public ::logger::parameters
	{
		enum class file_type
		{
			regular,
			fifo
		};	// enum class file_type
		
		
		file_type file_type	= file_type::regular;
		size_t max_queue_size			= 1024;
		
		
		explicit parameters(const nlohmann::json &json);
		
		parameters(const parameters &other) = default;
		parameters(parameters &&other) = default;
		
		parameters & operator=(const parameters &other) = default;
		parameters & operator=(parameters &&other) = default;
	};	// struct parameters
	
	
	async_logger(boost::asio::io_service &io_service,
				 const parameters &parameters);
	~async_logger();
	
	
	// Non-copy/-move constructible/assignable. Use ptrs.
	async_logger(const async_logger &other) = delete;
	async_logger(async_logger &&other) = delete;
	
	async_logger & operator=(const async_logger &other) = delete;
	async_logger & operator=(async_logger &&other) = delete;
	
	
	inline bool colorize_output() const noexcept;
	inline size_t max_queue_size() const noexcept;
	
	
	::logger::stream<async_logger> stream(level level, bool add_level_prefix = true) noexcept;
	
	void log_raw(level level_, const std::string &data) noexcept;
	void log_raw(level level_, std::string &&data) noexcept;
	
	
	bool try_flush() noexcept;
private:
	bool force_flush_first() noexcept;
	void start_next_file() noexcept;
	
	
	// Implicit async execution flow!
	void add_write_handler_or_write();
	void write_handler(const boost::system::error_code &err, size_t bytes_transferred);
	
	
	// Data
	parameters parameters_;
	
	std::string pid_str_;
	size_t log_file_number_ = 0;
	size_t messages_logged_ = 0;
	
	boost::filesystem::path current_file_path_;
	
	boost::asio::posix::stream_descriptor stream_;
	
	std::queue<std::string> log_queue_;
	bool sending_ = false;
};	// class async_logger


typedef ::logger::enable_logger_impl<async_logger> enable_async_logger;
typedef ::logger::enable_logger_ref_impl<async_logger> enable_async_logger_ref;


};	// namespace logger


#include <logger/async_logger.hpp>

#endif	// LOGGER_ASYNC_LOGGER_H
