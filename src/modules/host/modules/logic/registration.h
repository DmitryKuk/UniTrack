// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef HOST_MODULES_LOGIC_REGISTRATION_H
#define HOST_MODULES_LOGIC_REGISTRATION_H

#include <server/host/file.h>
#include <server/host/file_handlers/files_only.h>
#include <logic/registration.h>


namespace host {
namespace logic {


class registration:
	public ::logic::registration,
	public server::host::file<server::host::file_handlers::files_only>
{
public:
	using file_host = server::host::file<server::host::file_handlers::files_only>;
	
	
	registration(const nlohmann::json &config,
				 ::logic::global_instance &logic);
	
	
	virtual
	std::unique_ptr<server::protocol::http::response>
	response(const server::worker &worker,
			 server::protocol::http::request &request) const override;
protected:
	// Validators
	void validate_request_body(const std::vector<char> &body) const;
};	// class registration


};	// namespace logic
};	// namespace host


#endif	// HOST_MODULES_LOGIC_REGISTRATION_H
