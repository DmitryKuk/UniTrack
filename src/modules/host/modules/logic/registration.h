// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef HOST_MODULES_LOGIC_USER_H
#define HOST_MODULES_LOGIC_USER_H

#include <server/host/file.h>
#include <host/modules/file_handlers/files_and_template_pages.h>
#include <logic/registration.h>


namespace host {
namespace logic {


class registration:
	public ::logic::registration,
	public server::host::file<host::file_handlers::files_and_template_pages>
{
public:
	using file_host = server::host::file<host::file_handlers::files_and_template_pages>;
	
	
	registration(const nlohmann::json &config,
				 ::logic::global_instance &logic);
	
	
	virtual
	std::unique_ptr<server::protocol::http::response>
	response(const server::worker &worker,
			 server::protocol::http::request &request) const override;
protected:
	void validate_request_body(const std::vector<char> &body) const;
};	// class registration


};	// namespace logic
};	// namespace host


#endif	// HOST_MODULES_LOGIC_USER_H
