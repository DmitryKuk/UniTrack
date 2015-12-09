// Author: Dmitry Kukovinets (d1021976@gmail.com)


template<class ModuleRepr>
template<class String, class HostGenerator>
inline
host::module<ModuleRepr>::module(String &&host_type,
								 HostGenerator &&host_generator,
								 host::module_registrar &registrar):
	base::module<std::string, host::host_generator_type>{
		std::forward<String>(host_type),
		std::forward<HostGenerator>(host_generator),
		registrar
	}
{}
