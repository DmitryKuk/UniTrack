// Author: Vera Produvnova
// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/modules/date_chunk.h>

#include <ctime>

#include <templatizer/module.h>

using namespace std::literals;


const std::string templatizer::date_chunk::cmd = "date"s;


namespace {	// Module

templatizer::module<templatizer::date_chunk> module(templatizer::date_chunk::cmd);

};	// namespace


templatizer::date_chunk::date_chunk(const std::string &symbol):
	symbol_(symbol)
{}


templatizer::date_chunk::date_chunk(std::string &&symbol) noexcept:
	symbol_(std::move(symbol))
{}


// virtual
size_t
templatizer::date_chunk::generate(base::send_buffers_insert_functor buffers_ins_fn,
								  base::strings_cache &cache,
								  const templatizer::model &model) const
{
	std::time_t raw_time;
	std::time(&raw_time);
	auto *time_info = std::localtime(&raw_time);
	char date_str[11];
	std::strftime(date_str, 11, "%d.%m.%Y", time_info);
	
	return buffers_ins_fn(cache(date_str));
}


void
templatizer::date_chunk::export_symbols(std::unordered_set<std::string> &symbols) const
{
	symbols.insert(this->symbol_);
}
