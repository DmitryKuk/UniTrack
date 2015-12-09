// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <string>
#include <stdexcept>


// class base::module_registrar
template<class ModuleID, class ModuleObject>
template<class MID, class MObject>
bool
base::module_registrar<ModuleID, ModuleObject>::add(MID &&id, MObject &&object)
{
	return this->modules_.emplace(std::forward<MID>(id), std::forward<MObject>(object)).second;
}


template<class ModuleID, class ModuleObject>
void
base::module_registrar<ModuleID, ModuleObject>::erase(const module_id &id) noexcept
{
	this->modules_.erase(id);
}


// Returns the module object or throws std::out_of_range exception, if module not found
template<class ModuleID, class ModuleObject>
inline
const typename base::module_registrar<ModuleID, ModuleObject>::module_object &
base::module_registrar<ModuleID, ModuleObject>::module(const module_id &id) const
{
	return this->modules_.at(id);
}


template<class ModuleID, class ModuleObject>
inline
const typename base::module_registrar<ModuleID, ModuleObject>::map_type &
base::module_registrar<ModuleID, ModuleObject>::map() const noexcept
{
	return this->modules_;
}



// class base::module
template<class ModuleID, class ModuleObject, class ThrowIfNotAdded>
template<class MID, class MObject>
base::module<ModuleID, ModuleObject, ThrowIfNotAdded>::module(
	MID &&id,
	MObject &&object,
	base::module_registrar<module_id, module_object> &registrar)
{
	bool added = registrar.add(std::forward<MID>(id), std::forward<MObject>(object));
	
	if (!added) {
		using namespace std::literals;
		throw std::logic_error{"Module not added"s};
	}
}


template<class ModuleID, class ModuleObject>
template<class MID, class MObject>
base::module<ModuleID, ModuleObject, std::false_type>::module(
	MID &&id,
	MObject &&object,
	base::module_registrar<module_id, module_object> &registrar
)
{
	registrar.add(std::forward<MID>(id), std::forward<MObject>(object));
}
