// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef HOST_MODULES_LOGIC_ADD_FRIEND_H
#define HOST_MODULES_LOGIC_ADD_FRIEND_H

#include <server/host/base.h>
#include <logic/add_friend.h>


namespace host {
namespace logic {


class add_friend:
	public ::logic::add_friend,
	public server::host::base
{
public:
	using base_logic	= ::logic::add_friend;
	using base_host		= server::host::base;
	
	
	add_friend(const nlohmann::json &config, ::logic::global_instance &logic);
	
	
	virtual
	std::unique_ptr<server::protocol::http::response>
	response(const server::worker &worker, server::protocol::http::request &request) const override;
};	// class add_friend


};	// namespace logic
};	// namespace host


#endif	// HOST_MODULES_LOGIC_ADD_FRIEND_H
