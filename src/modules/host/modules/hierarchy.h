// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef HOST_HIERARCHY_H
#define HOST_HIERARCHY_H

#include <string>
#include <memory>
#include <unordered_map>

#include <json.hpp>

#include <server/host/base.h>
#include <server/protocol/http.h>
#include <logic/global_instance.h>


namespace host {


class hierarchy:
	public server::host::base
{
public:
	hierarchy(const nlohmann::json &config,
			  ::logic::global_instance &logic);
	
	
	// Non-copy/-move constructible/assignable. Use ptrs.
	hierarchy(const hierarchy &other) = delete;
	hierarchy(hierarchy &&other) = delete;
	
	hierarchy & operator=(const hierarchy &other) = delete;
	hierarchy & operator=(hierarchy &&other) = delete;
	
	
	virtual
	std::unique_ptr<server::protocol::http::response>
	response(const server::worker &worker,
			 server::protocol::http::request &request) const override;
	
	
	virtual void clear_cache() noexcept override;
protected:
	::logic::global_instance &logic_;
private:
	// Hosts from parameters
	std::unordered_map<std::string, std::shared_ptr<server::host::base>> hosts_;	// Optional
	std::shared_ptr<server::host::base> default_host_;								// Optional
};	// class hierarchy


};	// namespace host


#endif	// HOST_HIERARCHY_H
