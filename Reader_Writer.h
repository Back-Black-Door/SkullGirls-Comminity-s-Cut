#include <vector>

unsigned __int32 readBuffer_VectorUnChar_to_UnInt32(std::vector<unsigned char>& buffer, size_t Start);

unsigned __int64 readBuffer_VectorUnChar_to_UnInt64(std::vector<unsigned char>& buffer, size_t Start);

std::vector<unsigned char> readBuffer_VectorUnChar_to_VectorUnChar(std::vector<unsigned char>& buffer, size_t Start, size_t SizeOfString);

std::vector<unsigned char> convert_UnInt32_to_VectorChar(unsigned __int32 UNInt);

std::vector<unsigned char> convert_UnInt64_to_VectorChar(unsigned __int64 UNInt);

