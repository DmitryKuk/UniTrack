// Author: Dmitry Kukovinets (d1021976@gmail.com)

// This file contains classes for programming modules:
//	- module_registrar, which registers some module data with id as key;
//	- module, which can register itself in register.


// Usage example 1:
//	module_registrar<std::string, int> registrar;
//	
//	module<std::string, int> m{"one"s, 1, registrar};
//	module<std::string, int> m{"two"s, 2, registrar};
//	
//	std::cout << registrar.module("one"s) << std::endl;
//	std::cout << registrar.module("two"s) << std::endl;

// Usage example 2:
//	class my_registrar: public module_registrar<std::string, int>
//	{
//		static my_registrar registrar;
//	};
//	
//	class my_module1: public module<std::string, int>
//	{
//		my_module1(): module<std::string, int>{"name_of_my_module1"s, 1, my_registrar::registrar}
//		{ ... }
//	};
//	my_module1 module1;	// Singleton! (because of fixed name)

// Usage example 3:
//	class my_registrar: public module_registrar<std::string, std::function<void (void)>>
//	{
//		static my_registrar registrar;
//	};
//	
//	static module<std::string, std::function<void (void)>, std::false_type>{	// Ignore errors with duplicate actions
//		"name_of_my_module1"s,
//		[] { std::cout << "Action for module 1" << std::endl; },
//		my_registrar::registrar
//	};
//	
//	const auto &action = my_registrar::registrar.module("name_of_my_module1");
//	action();


#ifndef BASE_MODULE_H
#define BASE_MODULE_H

#include <type_traits>
#include <unordered_map>


namespace base {


template<class ModuleID, class ModuleObject>
class module_registrar
{
public:
	using module_id		= ModuleID;
	using module_object	= ModuleObject;
	
	
	template<class MID, class MObject>
	bool add(MID &&id, MObject &&object);
	
	void erase(const module_id &id) noexcept;
	
	void clear() noexcept;
	
	
	// Returns the module object or throws std::out_of_range exception, if module not found
	inline const module_object & module(const module_id &id) const;
private:
	std::unordered_map<module_id, module_object> modules_;
};	// class module_registrar



template<class ModuleID, class ModuleObject, class ThrowIfNotAdded = std::true_type>
class module
{
public:
	using module_id		= ModuleID;
	using module_object	= ModuleObject;
	
	
	// Throws std::logic_error, if module was not added
	template<class MID, class MObject>
	module(MID &&id,
		   MObject &&object,
		   module_registrar<module_id, module_object> &registrar);
};	// class module



template<class ModuleID, class ModuleObject>
class module<ModuleID, ModuleObject, std::false_type>
{
public:
	using module_id		= ModuleID;
	using module_object	= ModuleObject;
	
	
	// Does NOT throw any exceptions, if module was not added, errors are ignored
	template<class MID, class MObject>
	module(MID &&id,
		   MObject &&object,
		   module_registrar<module_id, module_object> &registrar);
};	// class module


};	// namespace base


#include <base/module.hpp>

#endif // BASE_MODULE_H
