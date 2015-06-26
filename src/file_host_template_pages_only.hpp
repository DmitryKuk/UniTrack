// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <stdexcept>
#include <numeric>

#include <server/host.h>
#include <server/file_host_handler_exceptions.h>

#include <templatizer/model_exceptions.h>


inline
template_pages_only::template_pages_only(page_model &page_model):
	page_model_(page_model)
{}


template<class FileHost>
std::pair<base::send_buffers_t, base::send_buffers_t>
template_pages_only::operator()(const FileHost &host,
								server::file_host_cache<template_pages_only> &cache)
{
	// Getting the page
	try {										// Try to use cached page
		cache.page_ptr = this->pages_cache_.at(cache.path.string());
	} catch (const std::out_of_range &e) {		// ...or load the new one, if not cached
		cache.page_ptr = std::make_shared<templatizer::page>(cache.path);
		this->pages_cache_.emplace(cache.path.string(), cache.page_ptr);
	}
	
	
	// Generating data to send
	std::pair<base::send_buffers_t, base::send_buffers_t> res;
	
	try {
		cache.page_ptr->generate(res.second, cache.strings, this->page_model_);
	} catch (const templatizer::model_error &e) {
		static const std::string model_error = "Model error: ";
		
		throw server::file_host_handler_error(model_error + e.what());
	}
	
	size_t content_len = 0;
	for (const auto &buffer: res.second)
		content_len += base::buffer_size(buffer);
	
	cache.content_len_str = std::move(std::to_string(content_len));
	
	server::host::add_header(res.first, server::http::header_content_length,
										cache.content_len_str);
	
	
	return res;
}
