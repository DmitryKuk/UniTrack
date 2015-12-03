// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/modules/var_chunk.h>

#include <templatizer/module.h>

using namespace std::literals;


const std::string templatizer::var_chunk::cmd = "var"s;


namespace {	// Module

templatizer::module<templatizer::var_chunk> module(templatizer::var_chunk::cmd);

};	// namespace


templatizer::var_chunk::var_chunk(const std::string &symbol):
	symbol_(symbol)
{}


templatizer::var_chunk::var_chunk(std::string &&symbol) noexcept:
	symbol_(std::move(symbol))
{}


// virtual
size_t
templatizer::var_chunk::generate(base::send_buffers_insert_functor buffers_ins_fn,
								 base::strings_cache & /*cache*/,
								 const templatizer::model &model) const
{
	return buffers_ins_fn(model.variable(this->symbol_));
}


void
templatizer::var_chunk::export_symbols(std::unordered_set<std::string> &symbols) const
{
	symbols.insert(this->symbol_);
}
