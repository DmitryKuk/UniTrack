// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/file/template_pages_only.h>

#include <tuple>

#include <server/host/exceptions.h>


// class host::file::template_pages_only::response
const templatizer::page &
host::file::template_pages_only::pages_cache::at(const boost::filesystem::path &path)
{
	auto path_str = path.string();
	auto it = this->cache_.find(path_str);
	
	if (it == this->cache_.end()) {
		bool inserted = false;
		std::tie(it, inserted) = this->cache_.emplace(std::move(path_str), std::make_unique<templatizer::page>(path));
		
		if (!inserted)
			throw server::host::error("Can\'t emplace template page");
	}
	
	return *(it->second);
}
