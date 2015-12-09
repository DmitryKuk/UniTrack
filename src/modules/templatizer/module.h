// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef TEMPLATIZER_MODULE_H
#define TEMPLATIZER_MODULE_H

#include <string>
#include <memory>
#include <functional>

#include <base/module.h>
#include <templatizer/chunk.h>


namespace templatizer {


using chunk_generator_type	= std::function<std::unique_ptr<templatizer::chunk> (const std::string &)>;
using module_registrar		= base::module_registrar<std::string, chunk_generator_type>;


extern module_registrar default_module_registrar;


template<class ModuleRepr>
class module:
	public base::module<std::string, chunk_generator_type>
{
public:
	inline module(const std::string &command,
				  module_registrar &registrar = default_module_registrar);
};	// class module


};	// namespace templatizer


#include <templatizer/module.hpp>

#endif	// TEMPLATIZER_MODULE_H
