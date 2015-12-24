// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef BASE_FILE_CACHE_H
#define BASE_FILE_CACHE_H

#include <string>
#include <memory>
#include <unordered_map>

#include <boost/filesystem/path.hpp>


namespace base {


// T -- file type (base::mapped_file, for example)
// Generator should have call operator:
// 	std::shared_ptr<T> operator()(const boost::filesystem::path &path);

template<class T, class Generator>
class file_cache
{
public:
	typedef std::shared_ptr<T> value_type;
	typedef Generator generator_type;
	
	
	inline file_cache(generator_type &&generator = generator_type{});
	inline file_cache(const generator_type &generator);
	
	file_cache(const file_cache &other) = default;
	file_cache(file_cache &&other) = default;
	
	file_cache & operator=(const file_cache &other) = default;
	file_cache & operator=(file_cache &&other) = default;
	
	
	// Caches file, specified by path.
	// Returns shared_ptr to it.
	value_type at(const boost::filesystem::path &path);
	
	// Removes file, specified by path, from cache.
	inline void erase(const boost::filesystem::path &path);
	
	// Removes all cached data.
	inline void clear() noexcept;
	
	
	inline size_t size() const noexcept;
	inline bool empty() const noexcept;
private:
	// Data
	std::unordered_map<std::string, value_type> cache_;
	
	generator_type generator_;
};	// class file_cache


};	// namespace base


#include <base/file_cache.hpp>

#endif	// BASE_FILE_CACHE_H
