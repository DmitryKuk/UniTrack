// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <array>
#include <chrono>


template<class HashType>
std::string
logic::generate_random_hash(const byte *data, size_t data_len, std::random_device &rd)
{
	static const char hex_numbers[] = { '0', '1', '2', '3', '4', '5', '6', '7',
										'8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
	
	
	std::time_t current_time = std::chrono::system_clock::now().time_since_epoch().count();
	
	HashType hash;
	hash.Update(data, data_len);
	hash.Update(reinterpret_cast<const byte *>(&current_time), sizeof(current_time));
	
	constexpr size_t digest_size = HashType::DIGESTSIZE;
	
	// Generating more data for hashing
	do {	// do-while for repeat at least 1 time
		unsigned int random_value = rd();
		hash.Update(reinterpret_cast<const byte *>(&random_value), sizeof(random_value));
		data_len += sizeof(random_value);
	} while (data_len <= digest_size);
	
	
	std::array<byte, digest_size> digest;
	hash.Final(digest.data());
	
	
	static_assert(CHAR_BIT == 8, "Code below works only on architectures with CHAR_BIT == 8.");
	
	std::string res;
	res.reserve(digest.size() * 2);
	
	for (const byte b: digest) {
		res += hex_numbers[(b >> (CHAR_BIT / 2)) & 0x0F];
		res += hex_numbers[b & 0x0F];
	}
	
	return std::move(res);
}


template<class HashType>
inline
std::string
logic::generate_random_hash(const std::string &str, std::random_device &rd)
{
	return logic::generate_random_hash<HashType>(reinterpret_cast<const byte *>(str.c_str()), str.size(), rd);
}
