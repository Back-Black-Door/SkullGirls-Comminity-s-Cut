#include "gfs.h"
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <io.h>
#include <bit>

namespace fs = std::filesystem;

// ===== ReadStuff =====
const size_t HEADER_SIZE = 0x33;
const size_t HEADER_COUNT_FILES_OFFSET = 0x2B;
const std::string FILE_IDENTIFIER = "Reverge Package File";
const std::string FILE_VERSION = "1.1";
#ifdef _32bit
const size_t MAX_BUFFER_SIZE = 1024 * 1024 * 4; //16MB
#else
const size_t MAX_BUFFER_SIZE = 1024 * 1024 * 16; //64MB
#endif

uint32_t readBufferChar_to_UnInt32(const unsigned char* buffer, size_t Start = 0) {
    return
        (uint32_t)buffer[Start + 3] |
        ((uint32_t)buffer[Start + 2] << 8) |
        ((uint32_t)buffer[Start + 1] << 16) |
        ((uint32_t)buffer[Start] << 24);
}

uint64_t readBufferChar_to_UnInt64(const unsigned char* buffer, size_t Start = 0) {
    return
        (uint64_t)buffer[Start + 7] |
        ((uint64_t)buffer[Start + 6] << 8) |
        ((uint64_t)buffer[Start + 5] << 16) |
        ((uint64_t)buffer[Start + 4] << 24) |
        ((uint64_t)buffer[Start + 3] << 32) |
        ((uint64_t)buffer[Start + 2] << 40) |
        ((uint64_t)buffer[Start + 1] << 48) |
        ((uint64_t)buffer[Start] << 56);
}
template<typename T>
void append_byteswapped(std::vector<unsigned char>& buffer, T value) {
    T swapped = std::byteswap(value);
    size_t old_size = buffer.size();
    buffer.resize(old_size + sizeof(T));
    std::memcpy(buffer.data() + old_size, &swapped, sizeof(T));
}

GFSEdit::GFSEdit(const fs::path path) : gfs_path(path) {
    hFile = CreateFile(
        path.string().c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Failed to handle for: " + gfs_path.string());
    }

    std::vector<unsigned char> meta_buffer(HEADER_SIZE);
    if (!ReadFile(hFile, meta_buffer.data(), meta_buffer.capacity(), NULL, NULL)) {
        throw std::runtime_error("Failed to read header: " + gfs_path.string());
    }
    unsigned char* ptr = meta_buffer.data();
    header.data_offset = readBufferChar_to_UnInt32(ptr);
    header.count_of_files = readBufferChar_to_UnInt64(ptr, HEADER_COUNT_FILES_OFFSET);

    meta_buffer.resize(header.data_offset);
    if (!ReadFile(hFile, meta_buffer.data(), meta_buffer.capacity(), NULL, NULL)) {
        throw std::runtime_error("Failed to read Meta Data: " + gfs_path.string());
    }
    ptr = meta_buffer.data();
    uint64_t data_offset = 0;
    for (size_t i = 0; i < header.count_of_files; ++i) {
        uint64_t path_len = readBufferChar_to_UnInt64(ptr);
        ptr += sizeof(uint64_t);


        std::string relative_path(ptr, ptr + path_len);
        ptr += path_len;


        uint64_t file_len = readBufferChar_to_UnInt64(ptr);
        ptr += sizeof(uint64_t);


        ptr += sizeof(uint32_t);
        files_meta_data.emplace_back(FileMetaData{
             relative_path,
             file_len,
             data_offset
            });

        data_offset += file_len;
    }
    CloseHandle(hFile);
}

GFSEdit::~GFSEdit() {
    CloseHandle(hFile);
}

void GFSEdit::print_header() {
    std::cout << "Data_Offset: " << header.data_offset << '\n';
    std::cout << "Count of files: " << header.count_of_files << '\n';
}

void GFSEdit::print_file_metadata(size_t idx) {
    std::cout << "relative_path: " << files_meta_data[idx].relative_path << '\n';
    std::cout << "data_length: " << files_meta_data[idx].data_length << '\n';
    std::cout << "data_offset: " << files_meta_data[idx].data_offset << '\n';
}

void GFSEdit::add_file(const fs::path& file_path, const std::string& relative_path_in_archive, bool replace_existing) {
    if (!fs::exists(file_path)) {
        throw std::runtime_error("File does not exist: " + file_path.string());
    }
    if (!fs::is_regular_file(file_path)) {
        throw std::runtime_error("Path is not a regular file: " + file_path.string());
    }

    auto pending_it = std::find_if(
        pending_changes.begin(),
        pending_changes.end(),
        [&](const PendingChange& pc)
        { return pc.relative_path == relative_path_in_archive; });

    if (pending_it != pending_changes.end()) {
        pending_it->source_path = file_path;
        return;
    }

    auto meta_it = std::find_if(
        files_meta_data.begin(),
        files_meta_data.end(),
        [&](const FileMetaData& meta)
        { return meta.relative_path == relative_path_in_archive; });

    if (meta_it != files_meta_data.end() && !replace_existing) {
        throw std::runtime_error("File already exists in archive: " + relative_path_in_archive);
    }



    pending_changes.push_back({
        relative_path_in_archive,
        file_path,
        meta_it == files_meta_data.end() // is_new
        });
}

void GFSEdit::add_files(const fs::path& files_path, const std::string& relative_path_in_archive, bool replace_existing) {
    if (!fs::is_directory(files_path)) {
        throw std::runtime_error("Path is not a directory: " + files_path.string());
    }
    for (const std::filesystem::directory_entry& dir_entry : std::filesystem::recursive_directory_iterator(files_path)) {
        if (fs::is_regular_file(dir_entry.path())) {
            fs::path relative_path = fs::relative(dir_entry.path(), files_path);
            fs::path archive_path = (fs::path(relative_path_in_archive) / relative_path);
            try {
                add_file(dir_entry.path(), archive_path.generic_string(), replace_existing);
            }
            catch (const std::exception& e) {
                std::cerr << "Error adding file " << dir_entry.path() << ": " << e.what() << std::endl;
            }
        }
    }
}

void GFSEdit::extract_file(const std::string& relative_path_in_archive, const fs::path& output_path) {
    auto it = std::find_if(
        files_meta_data.begin(),
        files_meta_data.end(),
        [&](const FileMetaData& meta)
        { return meta.relative_path == relative_path_in_archive; });

    if (it == files_meta_data.end()) {
        throw std::runtime_error("File not found in archive: " + relative_path_in_archive);
    }

    fs::create_directories(output_path.parent_path());

    HANDLE ext_FILE = CreateFile(
        output_path.string().c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (ext_FILE == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Failed to create output file: " + output_path.string());
    }



    if (!SetFilePointer(hFile, header.data_offset + it->data_offset, NULL, FILE_BEGIN)) {
        throw std::runtime_error("Failed to set file pointer in archive");
    }

    std::vector<unsigned char> buffer(it->data_length);


    if (!ReadFile(hFile, buffer.data(), buffer.size(), NULL, NULL)) {
        throw std::runtime_error("Failed to read from archive");
    }

    if (!WriteFile(ext_FILE, buffer.data(), buffer.size(), NULL, NULL)) {
        throw std::runtime_error("Failed to write to output file");
    }

    CloseHandle(ext_FILE);
}

void GFSEdit::extract_files(const fs::path& output_path, const std::string& relative_path_in_archive) {
    // Ќормализуем путь дл€ сравнени€: добавл€ем / в конец если это папка
    std::string search_path = relative_path_in_archive;
    if (!search_path.empty() && search_path.back() != '/') {
        search_path += '/';
    }

    for (const auto& meta : files_meta_data) {
        // ѕровер€ем принадлежность к целевой директории
        if (search_path.empty() ||
            meta.relative_path == relative_path_in_archive ||
            (meta.relative_path.size() > search_path.size() &&
                meta.relative_path.substr(0, search_path.size()) == search_path)) {

            // ‘ормируем полный путь дл€ извлечени€
            fs::path full_output_path = output_path;
            if (!search_path.empty()) {
                // ”бираем префикс родительской директории из пути
                std::string relative_part = meta.relative_path.substr(search_path.size());
                full_output_path /= relative_part;
            }
            else {
                full_output_path /= meta.relative_path;
            }

            try {
                extract_file(meta.relative_path, full_output_path);
            }
            catch (const std::exception& e) {
                std::cerr << "Error extracting file " << meta.relative_path << ": " << e.what() << std::endl;
            }
        }
    }
}

void GFSEdit::commit_changes() {
    if (pending_changes.empty()) return;

    const fs::path temp_path = gfs_path.string() + ".tmp";
    HANDLE Temp_hFile = INVALID_HANDLE_VALUE;
    try {
       
        uint32_t old_offset = header.data_offset;
        Temp_hFile = CreateFile(
            temp_path.string().c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            CREATE_NEW,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        hFile = CreateFile(
            gfs_path.string().c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (Temp_hFile == INVALID_HANDLE_VALUE) {
            throw std::runtime_error("Failed to handle for temp file ");
        }

        std::vector<unsigned char> buffer(HEADER_SIZE);


        auto it_files = files_meta_data.begin();
        while (it_files != files_meta_data.end()) {
            bool should_erase = false;
            for (const auto& pc : pending_changes) {
                if (it_files->relative_path == pc.relative_path) {
                    should_erase = true;
                    break;
                }
            }
            if (should_erase) {
                std::cout << "Delete File (Because of replace):" << gfs_path.filename() << '\n' 
                    << it_files->relative_path << '\n';
                it_files = files_meta_data.erase(it_files); // erase возвращает следующий валидный итератор
            }
            else {
                append_byteswapped(buffer, uint64_t(it_files->relative_path.size()));
                buffer.insert(buffer.end(),
                    it_files->relative_path.c_str(),
                    it_files->relative_path.c_str() + it_files->relative_path.size());
                append_byteswapped(buffer, it_files->data_length);
                append_byteswapped(buffer, uint32_t(1));
                ++it_files;
            }

        }

        for (const auto& change : pending_changes) {
            std::cout << "Adding NEW File:" << gfs_path.filename() << '\n'
                << change.relative_path << '\n';
            append_byteswapped(buffer, uint64_t(change.relative_path.size()));
            buffer.insert(buffer.end(),
                change.relative_path.c_str(),
                change.relative_path.c_str() + change.relative_path.size());
            append_byteswapped(buffer, uint64_t(fs::file_size(change.source_path)));
            append_byteswapped(buffer, uint32_t(1));
        }
        header.data_offset = buffer.size();
        header.count_of_files = files_meta_data.size() + pending_changes.size();
        uint32_t BE_data_offset = std::byteswap(uint32_t(header.data_offset));
        uint64_t BE_count_of_files = std::byteswap(uint64_t(header.count_of_files));
        uint64_t BE_file_indidentifier_size = std::byteswap(uint64_t(FILE_IDENTIFIER.size()));
        uint64_t BE_file_version_size = std::byteswap(uint64_t(FILE_VERSION.size()));
        size_t ptr{ 0 };
        std::memcpy(buffer.data() + ptr, &BE_data_offset, sizeof(BE_data_offset));
        ptr += sizeof(BE_data_offset);
        std::memcpy(buffer.data() + ptr, &BE_file_indidentifier_size, sizeof(BE_file_indidentifier_size));
        ptr += sizeof(BE_file_indidentifier_size);
        std::memcpy(buffer.data() + ptr, FILE_IDENTIFIER.data(), FILE_IDENTIFIER.size());
        ptr += FILE_IDENTIFIER.size();
        std::memcpy(buffer.data() + ptr, &BE_file_version_size, sizeof(BE_file_version_size));
        ptr += sizeof(BE_file_version_size);
        std::memcpy(buffer.data() + ptr, FILE_VERSION.data(), FILE_VERSION.size());
        ptr += FILE_VERSION.size();
        std::memcpy(buffer.data() + ptr, &BE_count_of_files, sizeof(BE_count_of_files));
        if (!WriteFile(Temp_hFile, buffer.data(), buffer.size(), NULL, NULL)) {
            throw std::runtime_error("Failed to write for: " + temp_path.string());
        }
        buffer.clear();
        size_t i = 0;
        DWORD dwPos = SetFilePointer(hFile, old_offset, NULL, FILE_BEGIN);
        if (dwPos == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR) {
            throw std::runtime_error(std::format("Failed to set file pointer: {}", GetLastError()));
        }

        while (i < files_meta_data.size()) {
            uint64_t current_offset = files_meta_data[i].data_offset;
            uint64_t total_size = files_meta_data[i].data_length;
            size_t files_in_block = 1;

            while (i + files_in_block < files_meta_data.size()) {
                uint64_t expected_offset = files_meta_data[i + files_in_block - 1].data_offset + files_meta_data[i + files_in_block - 1].data_length;

                if (expected_offset == files_meta_data[i + files_in_block].data_offset) {

                    if (total_size + files_meta_data[i + files_in_block].data_length > MAX_BUFFER_SIZE) {
                        break;
                    }
                    total_size += files_meta_data[i + files_in_block].data_length;
                    files_in_block++;
                }
                else {
                    break;
                }
            }


            buffer.resize(total_size);
            dwPos = SetFilePointer(hFile, current_offset + old_offset, NULL, FILE_BEGIN);
            if (dwPos == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR) {
                throw std::runtime_error("Failed to set file pointer: " + gfs_path.string());
            }

            if (!ReadFile(hFile, buffer.data(), buffer.size(), NULL, NULL)) {
                throw std::runtime_error("Failed to read File Data: " + gfs_path.string());
            }

            if (!WriteFile(Temp_hFile, buffer.data(), buffer.size(), NULL, NULL)) {
                throw std::runtime_error("Failed to write File Data: " + temp_path.string());
            }


            i += files_in_block;
            buffer.clear();
        }
        buffer.clear();
        dwPos = SetFilePointer(hFile, 0, NULL, FILE_END);
        for (const auto& change : pending_changes) {
            HANDLE change_hFile = CreateFile(
                change.source_path.string().c_str(),
                GENERIC_READ | GENERIC_WRITE,
                0,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL
            );
            if (change_hFile == INVALID_HANDLE_VALUE) {
                CloseHandle(change_hFile);
                throw std::runtime_error("Failed to handle for change file: " + change.source_path.string());
            }

            size_t change_file_size = fs::file_size(change.source_path);
            buffer.resize(change_file_size);
            if (!ReadFile(change_hFile, buffer.data(), buffer.size(), NULL, NULL)) {
                throw std::runtime_error("Failed to read File Data: " + gfs_path.string());
            }

            if (!WriteFile(Temp_hFile, buffer.data(), buffer.size(), NULL, NULL)) {
                throw std::runtime_error("Failed to write File Data: " + temp_path.string());
            }
            dwPos = SetFilePointer(hFile, 0, NULL, FILE_END);
            buffer.clear();
            CloseHandle(change_hFile);
            files_meta_data.emplace_back(FileMetaData{
                 change.relative_path,
                 change_file_size,
                 dwPos - change_file_size - header.data_offset
                });
        }

        CloseHandle(Temp_hFile);
        CloseHandle(hFile);
        fs::remove(gfs_path);
        fs::rename(temp_path, gfs_path);

        hFile = CreateFile(
            gfs_path.string().c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        pending_changes.clear();
    }
    catch (const std::exception& e) {

        if (fs::exists(temp_path)) {
            CloseHandle(Temp_hFile);
            fs::remove(temp_path);
        }
        std::cout << e.what();
        throw;
    }
}

void GFSUnpacker::operator()(const std::filesystem::path& filetounpackcs) {
    std::filesystem::path filetounpack = filetounpackcs;
    unsigned __int64 number_of_files{ 0 };
    unsigned int offset_to_filedata{ 0 };
    FILE* fGFSMetaInfo = fopen(filetounpack.string().c_str(), "rb");
    FILE* fGFSFileData = fopen(filetounpack.string().c_str(), "rb");
    std::fread(&offset_to_filedata, sizeof offset_to_filedata, 1, fGFSMetaInfo);
    offset_to_filedata = std::byteswap((uint32_t)offset_to_filedata);
    std::fseek(fGFSMetaInfo, 0x2B, SEEK_SET);
    std::fread(&number_of_files, sizeof number_of_files, 1, fGFSMetaInfo);
    number_of_files = std::byteswap((uint64_t)number_of_files);
    std::fseek(fGFSMetaInfo, 0x33, SEEK_SET);
    int Files_Lenght{ 0 };
    for (int i{ 0 }; i < number_of_files; i++) {
        MetaInfo CurrentFile;

        std::fread(&CurrentFile.File_Path_Lenght, 0x8, 1, fGFSMetaInfo);
        CurrentFile.File_Path_Lenght = std::byteswap((uint64_t)CurrentFile.File_Path_Lenght);

        CurrentFile.File_Path.resize(CurrentFile.File_Path_Lenght);
        std::fread(reinterpret_cast<char*>(CurrentFile.File_Path.data()), CurrentFile.File_Path_Lenght, 1, fGFSMetaInfo);

        std::fread(&CurrentFile.File_Lenght, 0x8, 1, fGFSMetaInfo);
        CurrentFile.File_Lenght = std::byteswap((uint64_t)CurrentFile.File_Lenght);

        std::fseek(fGFSMetaInfo, 0x4, SEEK_CUR);

        //Read File Data		
        std::vector<char> FileData(CurrentFile.File_Lenght);

        std::fseek(fGFSFileData, offset_to_filedata + Files_Lenght, SEEK_SET);
        std::fread(reinterpret_cast<char*>(FileData.data()), CurrentFile.File_Lenght, 1, fGFSFileData);

        //Now we ready to write

        std::string path(CurrentFile.File_Path.begin(), CurrentFile.File_Path.end());
        std::filesystem::path filetowrite = filetounpack.replace_extension("") / path;
        filetowrite.make_preferred();
        std::filesystem::create_directories(filetowrite.parent_path());
        std::ofstream file(filetowrite, std::ios::out | std::ifstream::binary);
        file.write(reinterpret_cast<char*>(FileData.data()), CurrentFile.File_Lenght);
        file.close();

        Files_Lenght += CurrentFile.File_Lenght;
    }
}

void GFSPacker::operator()(const std::filesystem::path& filestopackcs) {

    unsigned __int64 numbers_of_file{ 0 };
    unsigned int offset_to_filedata{ 0x33 };

    std::filesystem::path pathGFS{};

    pathGFS = filestopackcs.parent_path() / filestopackcs.filename();
    pathGFS.replace_extension(".gfs");
    FILE* fGFSFileData = fopen(pathGFS.string().c_str(), "ab");
    FILE* fGFSMetaInfo = fopen(pathGFS.string().c_str(), "rb+");
    std::fseek(fGFSMetaInfo, 0x33, SEEK_SET);
    for (const std::filesystem::directory_entry& dir_entry : std::filesystem::recursive_directory_iterator(filestopackcs)) {
        if (dir_entry.is_regular_file()) {

            numbers_of_file++;
            std::string pathString = dir_entry.path().generic_string().erase(0, filestopackcs.generic_string().size() + 1);
            std::vector<unsigned char> i_file_path(pathString.begin(), pathString.end());
            uint64_t i_file_path_lenght = std::byteswap(uint64_t(i_file_path.size()));
            FILE* CurrentFile = fopen(dir_entry.path().string().c_str(), "rb");
            std::fseek(CurrentFile, 0, SEEK_END); // seek to end
            const uint64_t filesize = std::ftell(CurrentFile);
            uint64_t FileSize = std::byteswap(uint64_t(filesize));

            std::fseek(CurrentFile, 0, SEEK_SET);

            std::vector<unsigned char> buffer(filesize);
            std::fread(reinterpret_cast<char*>(buffer.data()), filesize, 1, CurrentFile);

            std::fclose(CurrentFile);

            std::fwrite(&i_file_path_lenght, 8, 1, fGFSMetaInfo);

            std::fwrite(reinterpret_cast<char*>(i_file_path.data()), i_file_path.size(), 1, fGFSMetaInfo);

            std::fwrite(&FileSize, 8, 1, fGFSMetaInfo);

            std::fwrite(&file_aligned, 4, 1, fGFSMetaInfo);

            std::fwrite(reinterpret_cast<char*>(buffer.data()), filesize, 1, fGFSFileData);

            offset_to_filedata += (20 + i_file_path.size());
        }
    } // End For
    std::fseek(fGFSMetaInfo, 0, SEEK_SET);
    offset_to_filedata = std::byteswap(uint32_t(offset_to_filedata));
    numbers_of_file = std::byteswap(uint64_t(numbers_of_file));
    std::fwrite(&offset_to_filedata, 0x4, 1, fGFSMetaInfo);
    std::fwrite(&file_identifier_length, 0x8, 1, fGFSMetaInfo);
    std::fwrite(file_identifier, 20, 1, fGFSMetaInfo);
    std::fwrite(&file_version_length, 0x8, 1, fGFSMetaInfo);
    std::fwrite(file_version, 0x3, 1, fGFSMetaInfo);
    std::fwrite(&numbers_of_file, 0x8, 1, fGFSMetaInfo);
}















