#include "Reader_Writer.h"
#include <vector>
#include <iostream>

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