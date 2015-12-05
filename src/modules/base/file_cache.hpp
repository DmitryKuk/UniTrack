// Author: Dmitry Kukovinets (d1021976@gmail.com)


template<class T, class Generator>
inline
base::file_cache<T, Generator>::file_cache(generator_type &&generator):
	generator_{std::move(generator)}
{}


template<class T, class Generator>
inline
base::file_cache<T, Generator>::file_cache(const generator_type &generator):
	generator_{generator}
{}


// Caches file, specified by path.
// Returns shared_ptr to it.
template<class T, class Generator>
typename base::file_cache<T, Generator>::value_type
base::file_cache<T, Generator>::at(const boost::filesystem::path &path)
{
	auto path_str = path.string();
	auto it = this->cache_.find(path_str);
	
	if (it == this->cache_.end())
		it = this->cache_.emplace(std::move(path_str), this->generator_(path)).first;
	// NOTE: .second is not checked, because of data should be inserted (duplicate not found)
	
	return it->second;
}


// Removes file, specified by path, from cache.
template<class T, class Generator>
inline
void
base::file_cache<T, Generator>::erase(const boost::filesystem::path &path)
{
	this->cache_.erase(path.string());
}


// Removes all cached data.
template<class T, class Generator>
inline
void
base::file_cache<T, Generator>::clear() noexcept
{
	this->cache_.clear();
}
