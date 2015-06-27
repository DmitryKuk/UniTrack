// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <stdexcept>
#include <type_traits>


namespace base {
namespace json_utils {


template<>
inline
nlohmann::json::const_reference
at<>(const nlohmann::json &json)
{
	return json;
}

template<>
inline
nlohmann::json::reference
at<>(nlohmann::json &json)
{
	return json;
}


template<class Key, class ...Keys>
inline
nlohmann::json::const_reference
at(const nlohmann::json &json, const Key &key, const Keys &...keys)
{
	return base::json_utils::at<Keys...>(json.at(key), keys...);
}

template<class Key, class ...Keys>
inline
nlohmann::json::reference
at(nlohmann::json &json, const Key &key, const Keys &...keys)
{
	return base::json_utils::at<Keys...>(json.at(key), keys...);
}


// Searches the json object for sub-object pointed by key or index and returns it.
// If no keys specified, returns object, casted to type T.
// If one of keys not found, std::out_of_range will thrown.
// If it's impossible to cast target object to type T, std::logic_error will thrown.
template<class T>
inline
typename std::enable_if<!std::is_same<T, const nlohmann::json &>::value, T>::type
get(const nlohmann::json &json)
{
	return json.get<T>();
}


template<class T>
inline
typename std::enable_if<std::is_same<T, const nlohmann::json &>::value, T>::type
get(const nlohmann::json &json)
{
	return json;
}


template<class T, class Key, class ...Keys>
inline
typename std::enable_if<!std::is_same<T, const nlohmann::json &>::value, T>::type
get(const nlohmann::json &json, const Key &key, const Keys &...keys)
{
	return base::json_utils::get<T>(base::json_utils::at<Key, Keys...>(json, key, keys...));
}


template<class T, class Key, class ...Keys>
inline
typename std::enable_if<std::is_same<T, const nlohmann::json &>::value, T>::type
get(const nlohmann::json &json, const Key &key, const Keys &...keys)
{
	return base::json_utils::at<Key, Keys...>(json, key, keys...);
}



// Same as get<...>(...), but saves (by moving) result into value.
// Returns true, if result was saved. Otherwise returns false.
template<class T, class ...Keys>
inline
bool
extract(const nlohmann::json &json, T &val, const Keys &...keys)
{
	try {
		val = std::move(base::json_utils::get<T, Keys...>(json, keys...));
	} catch (const std::out_of_range &) {
		return false;
	}
	
	return true;
}



// Tries to cast json object to each type of Ts...
// Returns true, if compatible type found (so, the callback functor called).
// Otherwise returns false and no callback functor called.
template<>
inline
bool
get_variant<>(const nlohmann::json &json)
{
	return false;
}


template<class T, class ...Ts>
inline
bool
get_variant(const nlohmann::json &json,
			std::function<void (T)> &&func,
			std::function<void (Ts)> &&...funcs)
{
	try {
		func(json_utils::get<T>(json));
	} catch (const std::logic_error &) {	// Thrown, when types are not compatible
		return json_utils::get_variant<Ts...>(json, funcs...);
	}
	
	return true;
}


};	// namespace json_utils
};	// namespace base
