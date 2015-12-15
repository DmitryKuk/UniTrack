// Author: Dmitry Kukovinets (d1021976@gmail.com)


inline
mongo::DBClientBase &
logic::global_instance::connection()
{
	return *this->connection_ptr_;
}


inline
const std::string &
logic::global_instance::collection_users() const noexcept
{
	return this->mongo_collection_users_;
}


inline
const std::string &
logic::global_instance::collection_sessions() const noexcept
{
	return this->mongo_collection_sessions_;
}


inline
std::time_t
logic::global_instance::session_lifetime() const noexcept
{
	return this->session_lifetime_;
}


inline
std::time_t
logic::global_instance::session_forget_time() const noexcept
{
	return this->session_forget_time_;
}


inline
unsigned int
logic::global_instance::session_create_attempts() const noexcept
{
	return this->session_create_attempts_;
}


inline
std::string
logic::global_instance::generate_session_id(const std::string &user_id)
{
	return logic::generate_session_id(user_id, this->random_device_);
}



inline
logic::enable_global_instance_ref::enable_global_instance_ref(global_instance &logic_gi) noexcept:
	logic_gi_{logic_gi}
{}


inline
logic::global_instance &
logic::enable_global_instance_ref::logic_gi() const noexcept
{
	return this->logic_gi_;
}
