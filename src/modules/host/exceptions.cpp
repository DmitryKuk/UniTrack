// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host/exceptions.h>


host::template_pages_cache_error::template_pages_cache_error(const std::string &path):
	server::host::error{"Can\'t add template page to cache: \"" + path + '\"'}
{}
