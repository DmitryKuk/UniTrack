// Author: Dmitry Kukovinets (d1021976@gmail.com)


// Checks server's thread for joinable
inline bool
server::server::joinable() const noexcept
{
	return this->server_thread_.joinable();
}


// Joins server's thread
inline void
server::server::join()
{
	this->server_thread_.join();
}


// Detaches server's thread
inline void
server::server::detach()
{
	this->server_thread_.detach();
}


// Returns the hosts manager of this server
inline
server::host_manager &
server::server::host_manager() noexcept
{
	return this->host_manager_;
}


inline
const server::host_manager &
server::server::host_manager() const noexcept
{
	return this->host_manager_;
}


// Returns server names
inline
const std::vector<std::string> &
server::server::names() const noexcept
{
	return this->parameters_.server_names;
}
