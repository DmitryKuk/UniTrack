// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGIC_RANDOM_HASH_H
#define LOGIC_RANDOM_HASH_H

#include <string>
#include <random>

#include <cryptopp/sha.h>


namespace logic {


template<class HashType = CryptoPP::SHA512>
std::string
generate_random_hash(const byte *data, size_t data_len, std::random_device &rd);


template<class HashType = CryptoPP::SHA512>
inline
std::string
generate_random_hash(const std::string &str, std::random_device &rd);


};	// namespace logic


#include <logic/random_hash.hpp>

#endif	// LOGIC_RANDOM_HASH_H
