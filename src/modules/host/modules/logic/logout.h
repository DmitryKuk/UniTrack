// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef HOST_MODULES_LOGIC_LOGOUT_H
#define HOST_MODULES_LOGIC_LOGOUT_H

#include <server/host/base.h>
#include <logic/logout.h>


namespace host {
namespace logic {


class logout:
	public ::logic::logout,
	public server::host::base
{
public:
	using base_host = server::host::base;
	
	
	logout(const nlohmann::json &config,
		  ::logic::global_instance &logic);
	
	
	virtual
	std::unique_ptr<server::protocol::http::response>
	response(const server::worker &worker,
			 server::protocol::http::request &request) const override;
};	// class logout


};	// namespace logic
};	// namespace host


#endif	// HOST_MODULES_LOGIC_LOGOUT_H
