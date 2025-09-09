#include "reader_writer.h"

namespace reader {
    uint32_t BE_readBuffer_VectorUnChar_to_UnInt32(std::vector<unsigned char>& buffer, size_t Start) {
        return
            (__int32)buffer[Start + 3] +
            ((__int32)buffer[Start + 2] << 8) +
            ((__int32)buffer[Start + 1] << 16) +
            ((__int32)buffer[Start] << 24);
    }

    uint32_t LE_readBuffer_VectorUnChar_to_UnInt32(std::vector<unsigned char>& buffer, size_t Start) {
        return
            (__int32)buffer[Start] +
            ((__int32)buffer[Start + 1] << 8) +
            ((__int32)buffer[Start + 2] << 16) +
            ((__int32)buffer[Start + 3] << 24);
    }

    uint64_t BE_readBuffer_VectorUnChar_to_UnInt64(std::vector<unsigned char>& buffer, size_t Start) {
        return
            (__int64)buffer[Start + 7] +
            ((__int64)buffer[Start + 6] << 8) +
            ((__int64)buffer[Start + 5] << 16) +
            ((__int64)buffer[Start + 4] << 24) +
            ((__int64)buffer[Start + 3] << 32) +
            ((__int64)buffer[Start + 2] << 40) +
            ((__int64)buffer[Start + 1] << 48) +
            ((__int64)buffer[Start] << 56);
    }

    uint64_t LE_readBuffer_VectorUnChar_to_UnInt64(std::vector<unsigned char>& buffer, size_t Start) {
        return
            (__int64)buffer[Start] +
            ((__int64)buffer[Start + 1] << 8) +
            ((__int64)buffer[Start + 2] << 16) +
            ((__int64)buffer[Start + 3] << 24) +
            ((__int64)buffer[Start + 4] << 32) +
            ((__int64)buffer[Start + 5] << 40) +
            ((__int64)buffer[Start + 6] << 48) +
            ((__int64)buffer[Start + 7] << 56);
    }
    std::string readBuffer_VectorUnChar_to_String(
        const std::vector<unsigned char>& buffer, size_t Start, size_t SizeOfString) {
        // Проверка на выход за границы
        if (Start + SizeOfString > buffer.size()) {
            throw ("Buffer read out of range!");
        }

        // Создаём строку напрямую из данных буфера
        return std::string(
            reinterpret_cast<const char*>(&buffer[Start]),
            SizeOfString
        );
    }
}
namespace writer {
    // Добавление uint16_t в Big-Endian порядке (2 байта)
    void appendBE16(std::vector<unsigned char>& vec, uint16_t value) {
        vec.push_back(static_cast<unsigned char>((value >> 8) & 0xFF));  // Старший байт
        vec.push_back(static_cast<unsigned char>(value & 0xFF));         // Младший байт
    }

    // Добавление uint16_t в Little-Endian порядке (2 байта)
    void appendLE16(std::vector<unsigned char>& vec, uint16_t value) {
        vec.push_back(static_cast<unsigned char>(value & 0xFF));         // Младший байт
        vec.push_back(static_cast<unsigned char>((value >> 8) & 0xFF));  // Старший байт
    }



    // Добавление uint32_t в Big-Endian порядке
    void appendBE32(std::vector<unsigned char>& vec, uint32_t value) {
        vec.push_back(static_cast<unsigned char>((value >> 24) & 0xFF));
        vec.push_back(static_cast<unsigned char>((value >> 16) & 0xFF));
        vec.push_back(static_cast<unsigned char>((value >> 8) & 0xFF));
        vec.push_back(static_cast<unsigned char>(value & 0xFF));
    }

    // Добавление uint32_t в Little-Endian порядке
    void appendLE32(std::vector<unsigned char>& vec, uint32_t value) {
        vec.push_back(static_cast<unsigned char>(value & 0xFF));
        vec.push_back(static_cast<unsigned char>((value >> 8) & 0xFF));
        vec.push_back(static_cast<unsigned char>((value >> 16) & 0xFF));
        vec.push_back(static_cast<unsigned char>((value >> 24) & 0xFF));
    }
    


    // Добавление uint64_t в Big-Endian порядке
    void appendBE64(std::vector<unsigned char>& vec, uint64_t value) {
        vec.push_back(static_cast<unsigned char>((value >> 56) & 0xFF));
        vec.push_back(static_cast<unsigned char>((value >> 48) & 0xFF));
        vec.push_back(static_cast<unsigned char>((value >> 40) & 0xFF));
        vec.push_back(static_cast<unsigned char>((value >> 32) & 0xFF));
        vec.push_back(static_cast<unsigned char>((value >> 24) & 0xFF));
        vec.push_back(static_cast<unsigned char>((value >> 16) & 0xFF));
        vec.push_back(static_cast<unsigned char>((value >> 8) & 0xFF));
        vec.push_back(static_cast<unsigned char>(value & 0xFF));
    }

    // Добавление uint64_t в Little-Endian порядке
    void appendLE64(std::vector<unsigned char>& vec, uint64_t value) {
        vec.push_back(static_cast<unsigned char>(value & 0xFF));
        vec.push_back(static_cast<unsigned char>((value >> 8) & 0xFF));
        vec.push_back(static_cast<unsigned char>((value >> 16) & 0xFF));
        vec.push_back(static_cast<unsigned char>((value >> 24) & 0xFF));
        vec.push_back(static_cast<unsigned char>((value >> 32) & 0xFF));
        vec.push_back(static_cast<unsigned char>((value >> 40) & 0xFF));
        vec.push_back(static_cast<unsigned char>((value >> 48) & 0xFF));
        vec.push_back(static_cast<unsigned char>((value >> 56) & 0xFF));
    }



    // Добавление строки (без указания длины)
    void appendString(std::vector<unsigned char>& vec, const std::string& str) {
        vec.insert(vec.end(), str.begin(), str.end());
    }
}