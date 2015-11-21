// Author: Dmitry Kukovinets (d1021976@gmail.com)


// Returns cached copy of given string
inline
const std::string &
base::strings_cache::cache(const std::string &str)
{
	return *(this->strings_.insert(this->strings_.end(), str));
}


inline
const std::string &
base::strings_cache::cache(std::string &&str)
{
	return *(this->strings_.insert(this->strings_.end(), std::move(str)));
}


inline
const std::string &
base::strings_cache::operator()(const std::string &str)
{
	return this->cache(str);
}


inline
const std::string &
base::strings_cache::operator()(std::string &&str)
{
	return this->cache(std::move(str));
}
