// Author: Dmitry Kukovinets (d1021976@gmail.com)


// Returns cached copy of given string
inline
const std::string &
base::strings_cache::cache(const std::string &str)
{
	return *(this->strings_.insert(this->end(), str));
}


inline
const std::string &
base::strings_cache::cache(std::string &&str)
{
	return *(this->strings_.insert(this->end(), str));
}
