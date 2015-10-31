// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <string>
#include <unordered_map>

#include <base/buffer.h>

#include <templatizer/model.h>
#include <templatizer/model_exceptions.h>
#include <templatizer/modules/var_chunk.h>


class page_model:
	public templatizer::model,
	public std::unordered_map<std::string, std::string>
{
public:
	// Throws templatizer::variable_not_found exception, if variable does not exist
	// in the model.
	virtual const std::string & variable(const std::string &var_name) const override
	{
		try {
			return this->std::unordered_map<std::string, std::string>::at(var_name);
		} catch (const std::out_of_range &) {
			throw templatizer::variable_not_found(var_name);
		}
	}
};	// class model


int main()
{
	page_model model;
	model.emplace("key", "value");
	
	try {
		templatizer::var_chunk chunk("key");
		
		base::send_buffers_t buffers;
		base::strings_cache_t cache;
		
		
		// Primary work
		chunk.generate(std::back_inserter(buffers),
					   base::make_back_inserter_functor(cache),
					   model);
		
		
		// Must be 1 buffer
		if (buffers.size() != 1)
			return 10;
		
		
		// Buffer must contain string "key"
		std::string buffer_value(base::buffer_cast<const char *>(buffers[0]),
								 base::buffer_size(buffers[0]));
		if (buffer_value != "value")
			return 11;
	} catch (const templatizer::variable_not_found &) {	// Requested unknown variable
		return 12;
	} catch (...) {	// Unknown error
		return 1;
	}
	
	return 0;
}