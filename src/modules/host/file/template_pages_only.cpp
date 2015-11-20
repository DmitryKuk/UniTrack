// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/file/template_pages_only.h>

#include <tuple>

#include <server/host/exceptions.h>


// class host::file::template_pages_only::response
const templatizer::page &
host::file::template_pages_only::pages_cache::at(const boost::filesystem::path &path)
{
	auto it = this->cache_.find(path);
	
	if (it == this->cache_.end()) {
		bool inserted = false;
		std::tie(it, inserted) = this->cache_.emplace(path, templatizer::page(path));
		
		if (!inserted)
			throw server::host::error("Can\'t emplace template page");
	}
	
	return it->second;
}
