// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/chunk.h>


// class raw_chunk
templatizer::raw_chunk::raw_chunk(const char *data, size_t size) noexcept:
	data_{data},
	size_{size}
{}


// virtual
size_t
templatizer::raw_chunk::generate(base::send_buffers_insert_functor buffers_ins_fn,
								 base::strings_cache & /*cache*/,							// Unused
								 const templatizer::model & /*model*/) const				// Unused
{
	return buffers_ins_fn(this->data_, this->size_);
}


// virtual
void
templatizer::raw_chunk::export_symbols(std::unordered_set<std::string> &) const noexcept
{}
