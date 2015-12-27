// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef HOST_MODULES_LOGIC_USERS_H
#define HOST_MODULES_LOGIC_USERS_H

#include <server/host/base.h>
#include <logic/users.h>


namespace host {
namespace logic {


class users:
	public ::logic::users,
	public server::host::base
{
public:
	using base_logic	= ::logic::users;
	using base_host		= server::host::base;
	
	
	users(const nlohmann::json &config, ::logic::global_instance &logic);
	
	
	virtual
	std::unique_ptr<server::protocol::http::response>
	response(const server::worker &worker, server::protocol::http::request &request) const override;
};	// class users


};	// namespace logic
};	// namespace host


#endif	// HOST_MODULES_LOGIC_USERS_H
