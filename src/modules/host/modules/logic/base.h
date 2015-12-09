// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef HOST_MODULES_LOGIC_BASE_H
#define HOST_MODULES_LOGIC_BASE_H

#include <memory>

#include <json.hpp>

#include <server/host/base.h>
#include <server/protocol/http.h>
#include <logic/global_instance.h>


namespace host {
namespace logic {


class base:
	public server::host::base
{
public:
	base(const nlohmann::json &config,
		 ::logic::global_instance &logic);
	
	
	// Non-copy/-move constructible/assignable. Use ptrs.
	base(const base &other) = delete;
	base(base &&other) = delete;
	
	base & operator=(const base &other) = delete;
	base & operator=(base &&other) = delete;
	
	
	virtual
	std::unique_ptr<server::protocol::http::response>
	response(const server::worker &worker,
			 server::protocol::http::request &request) const override;
protected:
	inline ::logic::global_instance & logic() noexcept;
private:
	::logic::global_instance &logic_;
};	// class base


};	// namespace logic
};	// namespace host


#include <host/modules/logic/base.hpp>

#endif	// HOST_MODULES_LOGIC_BASE_H
