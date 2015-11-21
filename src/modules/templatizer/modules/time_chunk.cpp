// Author: Vera Produvnova
// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/modules/time_chunk.h>

#include <ctime>

#include <templatizer/module.h>


const std::string templatizer::time_chunk::cmd = "time";


namespace {	// Module

templatizer::module<templatizer::time_chunk> module(templatizer::time_chunk::cmd);

};	// namespace


templatizer::time_chunk::time_chunk(const std::string &symbol):
	symbol_(symbol)
{}


templatizer::time_chunk::time_chunk(std::string &&symbol) noexcept:
	symbol_(std::move(symbol))
{}


// virtual
size_t
templatizer::time_chunk::generate(base::send_buffers_insert_functor buffers_ins_fn,
								  base::strings_cache &cache,
								  const templatizer::model &model) const
{
	std::time_t raw_time;
	std::time(&raw_time);
	auto *time_info = std::localtime(&raw_time);
	char time_str[6];
	std::strftime(time_str, 6, "%H:%M", time_info);
	
	return buffers_ins_fn(cache(time_str));
}


void
templatizer::time_chunk::export_symbols(std::unordered_set<std::string> &symbols) const
{
	symbols.insert(this->symbol_);
}
