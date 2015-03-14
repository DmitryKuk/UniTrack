// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include "collection.h"

mongo::collection::collection(mongoc_collection_t *raw_collection) noexcept:
	collection_(raw_collection, mongoc_collection_destroy)
{}

mongo::collection::collection(collection &&other) noexcept:
	collection_(std::move(other.collection_))
{}


mongo::collection &
mongo::collection::operator=(collection &&other) noexcept
{
	this->collection_ = std::move(other.collection_);
	return *this;
}


std::string
mongo::collection::name() const noexcept
{ return std::string(mongoc_collection_get_name(this->raw())); }


bool
mongo::collection::drop(bson_error_t *error) noexcept
{ return mongoc_collection_drop(this->collection_.get(), error); }

bool
mongo::collection::drop_index(const std::string &index_name, bson_error_t *error) noexcept
{ return mongoc_collection_drop_index(this->collection_.get(), index_name.c_str(), error); }


bool
mongo::collection::insert(const bson_t *document,
						  mongoc_insert_flags_t flags,
						  const mongoc_write_concern_t *write_concern,
						  bson_error_t *error)
{ return mongoc_collection_insert(this->raw(), flags, document, write_concern, error); }
