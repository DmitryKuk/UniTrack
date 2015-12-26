// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef HOST_MODULES_LOGIC_REMOVE_FRIEND_H
#define HOST_MODULES_LOGIC_REMOVE_FRIEND_H

#include <server/host/base.h>
#include <logic/remove_friend.h>


namespace host {
namespace logic {


class remove_friend:
	public ::logic::remove_friend,
	public server::host::base
{
public:
	using base_logic	= ::logic::remove_friend;
	using base_host		= server::host::base;
	
	
	remove_friend(const nlohmann::json &config, ::logic::global_instance &logic);
	
	
	virtual
	std::unique_ptr<server::protocol::http::response>
	response(const server::worker &worker, server::protocol::http::request &request) const override;
};	// class remove_friend


};	// namespace logic
};	// namespace host


#endif	// HOST_MODULES_LOGIC_REMOVE_FRIEND_H
