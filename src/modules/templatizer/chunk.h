// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef TEMPLATIZER_CHUNK_H
#define TEMPLATIZER_CHUNK_H

#include <string>
#include <unordered_set>

#include <base/buffer.h>
#include <base/strings_cache.h>

#include <templatizer/model.h>


namespace templatizer {


// Data chunks
class chunk
{
public:
	virtual ~chunk() = default;
	
	
	// Generates page content adding it to the buffers.
	// Returns size of added buffer.
	virtual size_t generate(base::send_buffers_insert_functor buffers_ins_fn,
							base::strings_cache &cache,
							const templatizer::model &model) const = 0;
	
	
	virtual void export_symbols(std::unordered_set<std::string> &symbols) const = 0;
};	// class chunk


class raw_chunk: public chunk
{
public:
	raw_chunk(const char *data, size_t size) noexcept;
	
	
	virtual void generate(base::send_buffers_insert_functor buffers_ins_fn,
						  base::strings_cache &cache,
						  const templatizer::model &model) const override;
	
	
	virtual void export_symbols(std::unordered_set<std::string> &symbols) const noexcept override;
private:
	char const * const data_;
	const size_t size_;
};	// class raw_chunk


};	// namespace templatizer


#endif	// TEMPLATIZER_CHUNK_H
