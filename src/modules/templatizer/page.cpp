// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/page.h>

#include <regex>
#include <functional>

#include <base/mapped_file_exceptions.h>
#include <templatizer/module.h>
#include <templatizer/modules/var_chunk.h>
#include <templatizer/page_exceptions.h>


// Default constructor
templatizer::page::page() noexcept:
	state_{templatizer::page::state::ok}
{}


// Move constructor
templatizer::page::page(templatizer::page &&other) noexcept:
	base::mapped_file{std::move(other)},
	
	state_{other.state_},
	chunk_ptrs_{std::move(other.chunk_ptrs_)}
{
	other.state_ = templatizer::page::state::ok;
}


// Constructs and loads template data
templatizer::page::page(const boost::filesystem::path &path):
	base::mapped_file{path, boost::interprocess::read_only, MAP_SHARED},	// This can throw
	
	state_{templatizer::page::state::ok}
{
	this->load();	// This can throw
}


// Move assignment
templatizer::page &
templatizer::page::operator=(templatizer::page &&other) noexcept
{
	this->base::mapped_file::operator=(std::move(other));
	
	this->state_			= other.state_;
	this->chunk_ptrs_		= std::move(other.chunk_ptrs_);
	
	other.state_ = templatizer::page::state::ok;
	
	return *this;
}


// Loads template from file.
// If an error occured, throws templatizer:: page_error, file_mapping_error, file_parsing_error.
// or templatizer::file_parsing_error.
void
templatizer::page::load(const boost::filesystem::path &path)
{
	*this = std::move(templatizer::page(path));
}


// Loads template from current file.
// If an error occured, throws templatizer:: page_error, file_mapping_error, file_parsing_error.
// or templatizer::file_parsing_error.
void
templatizer::page::load()
{
	using namespace boost::interprocess;
	
	try {
		chunk_ptrs_deque_type chunk_ptrs;
		
		
		// Parsing
		{
			const char *mapped_data = static_cast<const char *>(this->data());
			size_t mapped_size = this->size() / sizeof(char);
			
			
			// REGEX: ${command ARGUMENTS}	-- full form
			//    or: $(command ARGUMENTS)	-- full form
			//    or: $VAR_NAME				-- short form, equvivalent to: $(var VAR_NAME)
			// VAR_NAME can contain characters: a-z, A-Z, 0-9 and '_', but must to begin with
			// letter or '_' (like in C). Minimum length of name is 1 symbol.
			// NOTE: Some space symbols around VAR_NAME is allowed (but not a newline).
			// VAR_NAME is a first group of the regex.
			static const std::regex regex{
				"("
					// Full form: ${ COMMAND ... }
					"\\$[\\{\\(][[:space:]]*([[:alpha:]_][[:alnum:]_]*)"	// COMMAND [2]
						"[[:space:]]*([^\\}\\)]*)[\\}\\)]"					// ARG [3]
				"|"	// 'Or'
					// Short form: $VAR_NAME
					"\\$([[:alpha:]_][[:alnum:]_]*)"						// VAR_NAME [4]
				")",
				std::regex::optimize
			};
			
			static const std::regex_iterator<const char *> end;
			
			
			size_t old_pos = 0;
			
			for (std::regex_iterator<const char *> it{mapped_data, mapped_data + mapped_size, regex};
				 it != end;
				 ++it) {
				// Adding previous raw chunk
				{
					size_t current_pos = it->position();
					
					if (current_pos >= old_pos) {	// Indexing previous raw chunk...
						chunk_ptrs.emplace_back(
							std::make_unique<templatizer::raw_chunk>(
								mapped_data + old_pos,
								current_pos - old_pos
							)
						);
						old_pos = current_pos + it->length();
					}
				}
				
				
				// Adding current extra chunk
				{
					std::string command  = it->str(2),
								argument = it->str(3);
					
					if (command.empty()) {	// Short form (because of command is empty)
						command  = templatizer::var_chunk::cmd;
						argument = it->str(4);
					}
					
					
					try {
						auto chunk_generator = templatizer::default_module_registrar.module(command);
						chunk_ptrs.emplace_back(chunk_generator(std::move(argument)));
					} catch (const std::out_of_range &) {
						this->set_state(templatizer::page::state::parse_error);
						
						using namespace std::literals;
						throw templatizer::file_parsing_error{this->path().string(),
															  "Module not found: \""s + command + '\"'};
					}
				}
			}
			
			
			// Remember to index the last raw chunk
			if (old_pos < mapped_size)
				chunk_ptrs.emplace_back(
					std::make_unique<templatizer::raw_chunk>(
						mapped_data + old_pos,
						mapped_size - old_pos
					)
				);
		}	// End of parsing
		
		
		// Move data to *this, if success (if not success, see catch blocks below)
		this->chunk_ptrs_ = std::move(chunk_ptrs);
		
		this->set_state(templatizer::page::state::ok);
	} catch (const interprocess_exception &e) {	// Thrown by mapped_region, when the file is empty
		if (e.get_error_code() == invalid_argument) {
			this->clear();
		} else {
			this->set_state(templatizer::page::state::file_error);
			throw templatizer::file_mapping_error{this->path().string(), e.what()};
		}
	} catch (const base::path_not_found &) {
		throw templatizer::path_not_found{this->path().string()};
	} catch (const std::exception &e) {
		throw templatizer::page_error{e.what()};
	}
}


// Simply deletes all loaded template data
void
templatizer::page::clear() noexcept
{
	using namespace boost::interprocess;
	
	this->chunk_ptrs_.clear();
	this->set_state(templatizer::page::state::ok);
}


// Generates result page from template using data model
// adding all data to the buffers and using the cache.
size_t
templatizer::page::generate(base::send_buffers_type &buffers,
							base::strings_cache &cache,
							const templatizer::model &model) const
{
	base::send_buffers_insert_functor buffers_insert_fn{buffers};
	
	size_t len = 0;
	for (const auto &chunk_ptr: this->chunk_ptrs_)
		len += chunk_ptr->generate(buffers_insert_fn, cache, model);
	
	return len;
}


// All symbols need to get from model
templatizer::page::symbol_set_type
templatizer::page::symbols() const
{
	templatizer::page::symbol_set_type res;
	this->export_symbols(res);
	return res;
}


// Same as symbols(), but puts them into set
void
templatizer::page::export_symbols(templatizer::page::symbol_set_type &symbols) const
{
	for (const auto &chunk_ptr: this->chunk_ptrs_)
		chunk_ptr->export_symbols(symbols);
}


std::ostream &
operator<<(std::ostream &stream, const templatizer::page::page_printer &printer)
{
	base::send_buffers_type buffers;
	base::strings_cache cache;
	
	// Generating the page...
	printer.page.generate(buffers, cache, printer.model);
	
	// ...and printing it
	for (const auto &buffer: buffers)
		stream.write(base::buffer_cast<const char *>(buffer),
					 base::buffer_size(buffer));
	
	return stream;
}
