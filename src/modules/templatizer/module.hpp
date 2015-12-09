// Author: Dmitry Kukovinets (d1021976@gmail.com)


template<class ModuleRepr>
inline
templatizer::module<ModuleRepr>::module(const std::string &command,
										templatizer::module_registrar &registrar):
	base::module<std::string, templatizer::chunk_generator_type>{
		command,
		[](const std::string &argument) -> std::unique_ptr<templatizer::chunk> {
			return std::make_unique<ModuleRepr>(argument);
		},
		registrar
	}
{}
