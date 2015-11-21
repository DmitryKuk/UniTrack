// Author: Vera Produvnova
// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/modules/toupper_chunk.h>

#include <cctype>
#include <algorithm>

#include <templatizer/module.h>


const std::string templatizer::toupper_chunk::cmd = "toupper";


namespace {	// Module

templatizer::module<templatizer::toupper_chunk> module(templatizer::toupper_chunk::cmd);

};	// namespace


templatizer::toupper_chunk::toupper_chunk(const std::string &symbol):
	symbol_(symbol)
{}


templatizer::toupper_chunk::toupper_chunk(std::string &&symbol) noexcept:
	symbol_(std::move(symbol))
{}


// virtual
size_t
templatizer::toupper_chunk::generate(base::send_buffers_insert_functor buffers_ins_fn,
									 base::strings_cache &cache,
									 const templatizer::model &model) const
{ 
	auto toup = model.variable(this->symbol_);
	std::transform(toup.begin(), toup.end(), toup.begin(), std::toupper);
	
	return buffers_ins_fn(cache(toup));
}


void
templatizer::toupper_chunk::export_symbols(std::unordered_set<std::string> &symbols) const
{
	symbols.insert(this->symbol_);
}
