// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef HOST_MODULE_H
#define HOST_MODULE_H

#include <string>
#include <memory>
#include <functional>

#include <base/module.h>
#include <server/host/base.h>
#include <logic/global_instance.h>


namespace host {


using host_generator_type	= std::function<std::shared_ptr<server::host::base> (const nlohmann::json &,
																				 ::logic::global_instance &)>;
using module_registrar		= base::module_registrar<std::string, host_generator_type>;


extern module_registrar default_module_registrar;


template<class ModuleRepr>
class module:
	public base::module<std::string, host_generator_type>
{
public:
	template<class String, class HostGenerator>
	inline module(String &&host_type,
				  HostGenerator &&host_generator,
				  module_registrar &registrar = default_module_registrar);
};	// class module


};	// namespace host


#include <host/module.hpp>

#endif	// HOST_MODULE_H
