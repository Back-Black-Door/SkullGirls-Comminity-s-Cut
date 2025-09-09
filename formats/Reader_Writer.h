#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include <algorithm>

namespace reader {
	uint32_t BE_readBuffer_VectorUnChar_to_UnInt32(std::vector<unsigned char>& buffer, size_t Start = 0);
	uint32_t LE_readBuffer_VectorUnChar_to_UnInt32(std::vector<unsigned char>& buffer, size_t Start = 0);
	uint64_t BE_readBuffer_VectorUnChar_to_UnInt64(std::vector<unsigned char>& buffer, size_t Start = 0);
	uint64_t LE_readBuffer_VectorUnChar_to_UnInt64(std::vector<unsigned char>& buffer, size_t Start = 0);
	std::string readBuffer_VectorUnChar_to_String(const std::vector<unsigned char>& buffer, size_t Start = 0, size_t SizeOfString = 0);
}
namespace writer {
	void appendBE16(std::vector<unsigned char>& vec, uint16_t value);
	void appendLE16(std::vector<unsigned char>& vec, uint16_t value);
	void appendBE32(std::vector<unsigned char>& vec, uint32_t value);
	void appendLE32(std::vector<unsigned char>& vec, uint32_t value);
	void appendBE64(std::vector<unsigned char>& vec, uint64_t value);
	void appendLE64(std::vector<unsigned char>& vec, uint64_t value);
	void appendString(std::vector<unsigned char>& vec, const std::string& str);
}