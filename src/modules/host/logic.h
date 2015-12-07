// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef HOST_LOGIC_H
#define HOST_LOGIC_H

#include <memory>

#include <json.hpp>

#include <server/host/base.h>
#include <server/protocol/http.h>
#include <logic/global_instance.h>


namespace host {


class logic:
	public server::host::base
{
public:
	logic(const nlohmann::json &config,
		  ::logic::global_instance &logic);
	
	
	// Non-copy/-move constructible/assignable. Use ptrs.
	logic(const logic &other) = delete;
	logic(logic &&other) = delete;
	
	logic & operator=(const logic &other) = delete;
	logic & operator=(logic &&other) = delete;
	
	
	virtual
	std::unique_ptr<server::protocol::http::response>
	response(const server::worker &worker,
			 server::protocol::http::request &request) const override;
private:
	::logic::global_instance &logic_;
};	// class logic


};	// namespace host


#endif	// HOST_LOGIC_H
