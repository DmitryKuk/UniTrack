// Author: Vera Produvnova
// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef TEMPLATIZER_MODULES_TOUPPER_CHUNK_CPP
#define TEMPLATIZER_MODULES_TOUPPER_CHUNK_CPP

#include <string>

#include <base/buffer.h>
#include <base/strings_cache.h>

#include <templatizer/chunk.h>
#include <templatizer/model.h>


namespace templatizer {


class toupper_chunk:
	public templatizer::chunk
{
public:
	static const std::string cmd;	// = "up"
	
	
	toupper_chunk(const std::string &symbol);
	toupper_chunk(std::string &&symbol) noexcept;
	
	
	virtual size_t generate(base::send_buffers_insert_functor buffers_ins_fn,
							base::strings_cache &cache,
							const templatizer::model &model) const override;
	
	
	virtual void export_symbols(std::unordered_set<std::string> &symbols) const override;
private:
	const std::string symbol_;
};	// class toupper_chunk


};	// namespace templatizer


#endif	// TEMPLATIZER_MODULES_TOUPPER_CHUNK_CPP
