// Author: Vera Produvnova
// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/modules/tolower_chunk.h>

#include <cctype>
#include <algorithm>

#include <templatizer/module.h>


const std::string templatizer::tolower_chunk::cmd = "tolower";


namespace {	// Module

templatizer::module<templatizer::tolower_chunk> module(templatizer::tolower_chunk::cmd);

};	// namespace


templatizer::tolower_chunk::tolower_chunk(const std::string &symbol):
	symbol_(symbol)
{}


templatizer::tolower_chunk::tolower_chunk(std::string &&symbol) noexcept:
	symbol_(std::move(symbol))
{}


// virtual
size_t
templatizer::tolower_chunk::generate(base::send_buffers_insert_functor buffers_ins_fn,
									 base::strings_cache &cache,
									 const templatizer::model &model) const
{ 
	auto tolow = model.variable(this->symbol_);
	std::transform(tolow.begin(), tolow.end(), tolow.begin(), std::tolower);
	
	return buffers_ins_fn(cache_inserter(tolow));
}


void
templatizer::tolower_chunk::export_symbols(std::unordered_set<std::string> &symbols) const
{
	symbols.insert(this->symbol_);
}
