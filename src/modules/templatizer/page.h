// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef TEMPLATIZER_PAGE_H
#define TEMPLATIZER_PAGE_H

#include <ostream>
#include <string>
#include <unordered_set>
#include <deque>
#include <memory>
#include <type_traits>

#include <boost/filesystem/path.hpp>

#include <base/mapped_file.h>
#include <base/buffer.h>
#include <base/strings_cache.h>

#include <templatizer/chunk.h>
#include <templatizer/model.h>


namespace templatizer {


class page:
	public base::mapped_file
{
public:
	enum class state
	{
		ok,
		file_error,
		parse_error
	};	// enum class state
	
	
	
	// Constructors
	page() noexcept;							// Default
	page(page &&other) noexcept;				// Move
	page(const boost::filesystem::path &path);	// Constructs and loads template data
	
	page & operator=(page &&other) noexcept;	// Move
	
	
	// Non-copy constructible/assignable
	page(const page &other) = delete;
	page & operator=(const page &other) = delete;
	
	
	// Loads template from file.
	// If an error occured, throws templatizer:: page_error, file_mapping_error, file_parsing_error.
	// or templatizer::file_parsing_error.
	void load(const boost::filesystem::path &path);
	
	// Loads template from current file.
	// If an error occured, throws templatizer:: page_error, file_mapping_error, file_parsing_error.
	// or templatizer::file_parsing_error.
	void load();
	
	
	// Simply deletes all loaded template data
	void clear() noexcept;
	
	
	// Generates result page from template using data model
	// adding all data to the buffers and using the cache.
	// Returns content length (sum of lengths of all added buffers).
	size_t generate(base::send_buffers_type &buffers,
					base::strings_cache &cache,
					const templatizer::model &model) const;
	
	
	// Syntax sugar, based on generate(). Usage:
	// 	std::cout << page(model);
	struct page_printer;
	inline page_printer operator()(const templatizer::model &model) const;
	
	
	// Symbols (variable names)
	typedef std::unordered_set<std::string> symbol_set_type;
	
	// All symbols need to get from model
	symbol_set_type symbols() const;
	
	// Same as symbols(), but puts them into set
	void export_symbols(symbol_set_type &symbols) const;
	
	
	// State
	inline enum state get_state() const noexcept;
	inline bool good() const noexcept;
	inline bool bad() const noexcept;
protected:
	typedef std::unique_ptr<templatizer::chunk> chunk_ptr_type;
	typedef std::deque<chunk_ptr_type> chunk_ptrs_deque_type;
	
	
	inline void set_state(enum state new_state) noexcept;
private:
	// State
	state state_;
	
	
	// Data
	chunk_ptrs_deque_type chunk_ptrs_;
};	// class page


};	// namespace templatizer


std::ostream &
operator<<(std::ostream &stream, const templatizer::page::page_printer &printer);


#include <templatizer/page.hpp>

#endif // TEMPLATIZER_PAGE_H
