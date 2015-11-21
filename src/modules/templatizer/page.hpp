// Author: Dmitry Kukovinets (d1021976@gmail.com)


// With model and cache (you can call operator*)
inline
templatizer::page::const_iterator
templatizer::page::begin(const templatizer::model &model,
					   base::strings_cache &cache) const noexcept
{
	return templatizer::page::const_iterator{this->chunk_ptrs_.begin(), model, cache};
}


inline
templatizer::page::const_iterator
templatizer::page::end(const templatizer::model &model,
					   base::strings_cache &cache) const noexcept
{
	return templatizer::page::const_iterator{this->chunk_ptrs_.end(), model, cache};
}


// Without model and cache (you can NOT call operator*)
inline
templatizer::page::const_iterator
templatizer::page::begin() const noexcept
{
	return templatizer::page::const_iterator{this->chunk_ptrs_.begin()};
}


inline
templatizer::page::const_iterator
templatizer::page::end() const noexcept
{
	return templatizer::page::const_iterator{this->chunk_ptrs_.end()};
}



// State
inline
templatizer::page::state
templatizer::page::get_state() const noexcept
{
	return this->state_;
}


inline
bool
templatizer::page::good() const noexcept
{
	return this->state_ == templatizer::page::state::ok;
}


inline
bool
templatizer::page::bad() const noexcept
{
	return !this->good();
}


inline
void
templatizer::page::set_state(templatizer::page::state new_state) noexcept
{
	this->state_ = new_state;
}


struct templatizer::page::page_printer
{
	const templatizer::page &page;
	const templatizer::model &model;
};


inline
templatizer::page::page_printer
templatizer::page::operator()(const templatizer::model &model) const
{
	return page_printer{ .page = *this, .model = model };
}
