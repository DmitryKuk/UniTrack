// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/page.h>

#include <regex>

#include <templatizer/module_registrar.h>
#include <templatizer/modules/var_chunk.h>

#include <templatizer/page_exceptions.h>
#include <templatizer/module_registrar_exceptions.h>


// Default constructor
templatizer::page::page() noexcept:
	state_(templatizer::page::state::ok)
{}


// Move constructor
templatizer::page::page(templatizer::page &&other) noexcept:
	state_(other.state_),
	chunk_ptrs_(std::move(other.chunk_ptrs_)),
	file_mapping_(std::move(other.file_mapping_)),
	mapped_region_(std::move(other.mapped_region_))
{
	other.state_ = templatizer::page::state::ok;
}


// Constructs and loads template data
templatizer::page::page(const std::string &file):
	state_(templatizer::page::state::ok)
{
	this->load(file);	// This can throw
}


// Move assignment
templatizer::page &
templatizer::page::operator=(templatizer::page &&other) noexcept
{
	this->state_			= other.state_;
	this->chunk_ptrs_		= std::move(other.chunk_ptrs_);
	this->file_mapping_		= std::move(other.file_mapping_);
	this->mapped_region_	= std::move(other.mapped_region_);
	
	other.state_ = templatizer::page::state::ok;
	
	return *this;
}

#include <iostream>
// Loads template from file.
// If an error occured, throws templatizer::file_mapping_error
// or templatizer::file_parsing_error.
void
templatizer::page::load(const std::string &file)
{
	using namespace boost::interprocess;
	
	try {
		file_mapping mapping(file.c_str(), read_only);
		mapped_region region(mapping, read_only);
		chunk_ptrs_list_t chunk_ptrs;
		
		
		// Parsing
		{
			const char *region_data = static_cast<const char *>(region.get_address());
			size_t region_size = region.get_size() / sizeof(char);
			
			
			// REGEX: ${command ARGUMENTS}	-- full form
			//    or: $VAR_NAME				-- short form, equvivalents to: $(var VAR_NAME)
			// VAR_NAME can contain characters: a-z, A-Z, 0-9 and '_', but must to begin with
			// letter or '_' (like in C). Minimum length of name is 1 symbol.
			// NOTE: Some space symbols around VAR_NAME is allowed (but not a newline).
			// VAR_NAME is a first group of the regex.
			static const std::regex regex(
				"("
					// Full form: ${ COMMAND ... }
					"\\$\\{[[:space:]]*([[:alpha:]_][[:alnum:]_]*)"		// COMMAND [2]
						"[[:space:]]*([^\\}]*)\\}"						// ARG [3]
				"|"	// 'Or'
					// Short form: $VAR_NAME
					"\\$([[:alpha:]_][[:alnum:]_]*)"					// VAR_NAME (in short form) [4]
				")",
				std::regex::optimize);
			
			
			std::regex_iterator<const char *>
				it(region_data, region_data + region_size, regex),
				end;
			
			size_t old_pos = 0;
			while (it != end) {
				// Adding previous raw chunk
				size_t current_pos = it->position();
				if (current_pos > old_pos) {	// Indexing previous raw chunk...
					chunk_ptrs.emplace_back(new raw_chunk(region_data + old_pos, current_pos - old_pos));
					old_pos = current_pos + it->length();
				}
				
				
				// Adding current extra chunk
				std::string command  = std::move(it->str(2)),
							argument = std::move(it->str(3));
				if (it->length(2) == 0) {	// Short form (because of command is empty)
					command  = templatizer::var_chunk::var_chunk_cmd;
					argument = std::move(it->str(4));
				}
				
				
				// These can throw
				auto chunk_generator =
					templatizer::module_registrar::default_module_registrar.module(command);
				chunk_ptrs.emplace_back(std::move(chunk_generator(std::move(argument))));
				
				++it;
			}
			
			// Remember to index the last raw chunk
			if (old_pos < region_size)
				chunk_ptrs.emplace_back(new templatizer::raw_chunk(region_data + old_pos,
																   region_size - old_pos));
		}	// End of parsing
		
		// Move data to *this, if success (if not success, see catch blocks below)
		this->chunk_ptrs_		= std::move(chunk_ptrs);
		this->file_mapping_		= std::move(mapping);
		this->mapped_region_	= std::move(region);
		
		this->set_state(templatizer::page::state::ok);
	} catch (const interprocess_exception &e) {	// Thrown by mapped_region, when the file is empty
		if (e.get_error_code() == invalid_argument) {
			this->clear();
		} else {
			this->set_state(templatizer::page::state::file_error);
			throw templatizer::file_mapping_error(file, e.what());
		}
	} catch (const templatizer::module_not_found &e) {
		this->set_state(templatizer::page::state::parse_error);
		throw templatizer::file_parsing_error(file, e.what());
	}
}


// Simply deletes all loaded template data
void
templatizer::page::clear() noexcept
{
	using namespace boost::interprocess;
	
	this->chunk_ptrs_.clear();
	this->file_mapping_ = std::move(file_mapping());
	this->mapped_region_ = std::move(mapped_region());
	this->set_state(templatizer::page::state::ok);
}


// Generates result page from template using data model sending all data to the stream.
// Returns number of sent bytes.
size_t
templatizer::page::generate(std::ostream &stream, const templatizer::model &model) const
{
	size_t content_len = 0;
	for (const auto &chunk_ptr: this->chunk_ptrs_)
		content_len += chunk_ptr->generate(stream, model);
	return content_len;
}


// Generates result page from template using data model adding all data to the buffers.
// Returns summary size of all added buffers.
size_t
templatizer::page::generate(server::send_buffers_t &buffers, const templatizer::model &model) const
{
	size_t content_len = 0;
	for (const auto &chunk_ptr: this->chunk_ptrs_)
		content_len += chunk_ptr->generate(buffers, model);
	return content_len;
}


// All symbols need to get from model
templatizer::page::symbol_set
templatizer::page::symbols() const
{
	templatizer::page::symbol_set res;
	this->export_symbols(res);
	return res;
}


// Same as symbols(), but puts them into set
void
templatizer::page::export_symbols(templatizer::page::symbol_set &symbols) const
{
	for (const auto &chunk_ptr: this->chunk_ptrs_)
		chunk_ptr->export_symbols(symbols);
}
