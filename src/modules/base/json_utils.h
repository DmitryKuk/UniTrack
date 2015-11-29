// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include <functional>

#include <boost/filesystem/path.hpp>

#include <json.hpp>


namespace base {
namespace json_utils {


nlohmann::json
json_from_file(const boost::filesystem::path &path);


template<class... Keys>
inline nlohmann::json::const_reference at(const nlohmann::json &json, const Keys &... keys);

template<class... Keys>
inline nlohmann::json::reference at(nlohmann::json &json, const Keys &... keys);


// Searches the json object for sub-object pointed by key or index and returns it.
// If no keys specified, returns object, casted to type T.
// If one of keys not found, std::out_of_range will thrown.
// If it's impossible to cast target object to type T, std::logic_error will thrown.

// Usage example:
//	auto str = base::json_utils::get<std::string>(json, "key1", "key2", 3);	// str == "three"
// With this json object:
//	{
//		"key1": {
//			"key2": [ "zero", "one", "two", "three" ]
//		}
//	}
template<class T, class... Keys>
inline T get(const nlohmann::json &json, const Keys &... keys);


// Same as get<...>(...), but saves (by moving) result into value.
// Returns true, if result was saved. Otherwise returns false.
template<class T, class... Keys>
inline bool extract(const nlohmann::json &json, T &val, const Keys &... keys);


// Tries to cast json object to each type of Ts...
// Returns true, if compatible type found (so, the callback functor called).
// Otherwise returns false and no callback functor called.
template<class... Ts>
inline bool get_variant(const nlohmann::json &json, std::function<void (Ts)> &&... funcs);


};	// namespace json_utils
};	// namespace base


#include <base/json_utils.hpp>

#endif	// JSON_UTILS_H
