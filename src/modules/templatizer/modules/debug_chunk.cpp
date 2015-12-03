// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/modules/debug_chunk.h>

#include <templatizer/module.h>

using namespace std::literals;


const std::string templatizer::debug_chunk::cmd = "debug"s;


namespace {	// Module

templatizer::module<templatizer::debug_chunk> module(templatizer::debug_chunk::cmd);

};	// namespace


templatizer::debug_chunk::debug_chunk(const std::string &symbol):
	symbol_(symbol)
{}


templatizer::debug_chunk::debug_chunk(std::string &&symbol) noexcept:
	symbol_(std::move(symbol))
{}


// virtual
size_t
templatizer::debug_chunk::generate(base::send_buffers_insert_functor buffers_ins_fn,
								   base::strings_cache &cache,
								   const templatizer::model &model) const
{
	const auto &value = model.variable(this->symbol_);	// This can throw
	
	size_t len = 0;
	
	len += buffers_ins_fn("DEBUG (length = "s);
	len += buffers_ins_fn(cache(std::to_string(value.size())));
	len += buffers_ins_fn("): \""s);
	len += buffers_ins_fn(value);
	len += buffers_ins_fn("\""s);
	
	return len;
}


void
templatizer::debug_chunk::export_symbols(std::unordered_set<std::string> &symbols) const
{
	symbols.insert(this->symbol_);
}
