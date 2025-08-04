#include "Reader_Writer.h"

unsigned __int32 readBuffer_VectorUnChar_to_UnInt32(std::vector<unsigned char>& buffer, size_t Start) {
	unsigned __int32 ReturnValue{};
	ReturnValue =
		(__int32)buffer[Start + 3] +
		((__int32)buffer[Start + 2] << 8) +
		((__int32)buffer[Start + 1] << 16) +
		((__int32)buffer[Start] << 24);
	return ReturnValue;
}

unsigned __int64 readBuffer_VectorUnChar_to_UnInt64(std::vector<unsigned char>& buffer, size_t Start) {
	unsigned __int64 ReturnValue{};
	ReturnValue = 
		 (__int64)buffer[Start + 7] +
		((__int64)buffer[Start + 6] << 8) +
		((__int64)buffer[Start + 5] << 16) +
		((__int64)buffer[Start + 4] << 24) +
		((__int64)buffer[Start + 3] << 32) +
		((__int64)buffer[Start + 2] << 40) +
		((__int64)buffer[Start + 1] << 48) +
		((__int64)buffer[Start]     << 56);
	return ReturnValue;
}

std::vector<unsigned char> readBuffer_VectorUnChar_to_VectorUnChar(std::vector<unsigned char>& buffer, size_t Start, size_t SizeOfString) {
	std::vector<unsigned char> ReturnVector;
	for (size_t i{ 0 }; i < SizeOfString; i++) {
		ReturnVector.push_back(buffer[Start + i]);
	}
	return ReturnVector;
}

std::vector<unsigned char> convert_UnInt32_to_VectorChar(unsigned __int32 UNInt) {
	std::vector<unsigned char> ReturnVector
	{ unsigned char(UNInt >> 24),
	  unsigned char(UNInt >> 16),
	  unsigned char(UNInt >> 8),
	  unsigned char(UNInt)
	};
	return ReturnVector;
}

std::vector<unsigned char> convert_UnInt64_to_VectorChar(unsigned __int64 UNInt) {
	std::vector<unsigned char> ReturnVector
	{ 
	unsigned char(UNInt >> 56),
	unsigned char(UNInt >> 48),
	unsigned char(UNInt >> 40),
	unsigned char(UNInt >> 32),
	unsigned char(UNInt >> 24),
	unsigned char(UNInt >> 16),
	unsigned char(UNInt >> 8),
	unsigned char(UNInt)
	};
	return ReturnVector;
}

unsigned int readCharArray_to_UnInt(char buffer[]) {
	return (buffer[3] << 0) | (buffer[2] << 8) | (buffer[1] << 16) | ((unsigned)buffer[0] << 24);
}

uint32_t swap_endian_32(uint32_t num) {
	return 
		((num >> 24) & 0xff)    |
		((num << 8) & 0xff0000) |
		((num >> 8) & 0xff00)   |
		((num << 24) & 0xff000000);
}

uint64_t swap_endian_64(uint64_t value) {
	return 
		((value & 0x00000000000000FFULL) << 56) |
		((value & 0x000000000000FF00ULL) << 40) |
		((value & 0x0000000000FF0000ULL) << 24) |
		((value & 0x00000000FF000000ULL) << 8)  |
		((value & 0x000000FF00000000ULL) >> 8)  |
		((value & 0x0000FF0000000000ULL) >> 24) |
		((value & 0x00FF000000000000ULL) >> 40) |
		((value & 0xFF00000000000000ULL) >> 56);
}