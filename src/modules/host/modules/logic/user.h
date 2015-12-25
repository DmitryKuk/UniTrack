// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef HOST_MODULES_LOGIC_USER_H
#define HOST_MODULES_LOGIC_USER_H

#include <server/host/base.h>
#include <logic/user.h>


namespace host {
namespace logic {


class user:
	public ::logic::user,
	public server::host::base
{
public:
	using base_logic	= ::logic::user;
	using base_host		= server::host::base;
	
	
	user(const nlohmann::json &config, ::logic::global_instance &logic);
	
	
	virtual
	std::unique_ptr<server::protocol::http::response>
	response(const server::worker &worker, server::protocol::http::request &request) const override;
	
	
	using section_handler_type = std::unique_ptr<server::protocol::http::response>
								 (user::*)(const server::worker &,
										   server::protocol::http::request &,
										   const std::string &,
										   const std::string &) const;
private:
	static const std::unordered_map<std::string, section_handler_type> section_handlers_;
	
	
	// Section handlers
	std::unique_ptr<server::protocol::http::response>
	section_root(const server::worker &worker,
				 server::protocol::http::request &request,
				 const std::string &user_ref,
				 const std::string &section) const;
};	// class user


};	// namespace logic
};	// namespace host


#endif	// HOST_MODULES_LOGIC_USER_H
