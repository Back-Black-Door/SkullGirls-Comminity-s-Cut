#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <future>
#include <memory>
#include <windows.h>

namespace fs = std::filesystem;

class GFSEdit {
public:
    GFSEdit(const fs::path path);
    ~GFSEdit();

    void print_header();
    void print_file_metadata(size_t idx);
    void add_file(const fs::path& file_path, const std::string& relative_path_in_archive, bool replace_existing = false);
    void add_files(const fs::path& files_path, const std::string& relative_path_in_archive = "", bool replace_existing = false);
    void extract_file(const std::string& relative_path_in_archive, const fs::path& output_path);
    void extract_files(const fs::path& output_path, const std::string& relative_path_in_archive = "");
    void commit_changes();
    fs::path gfs_path;
public:
    struct PendingChange {
        std::string relative_path;
        fs::path source_path;
        bool is_new;
    };
    struct Header {
        uint32_t data_offset;
        uint64_t count_of_files;
    };
    struct FileMetaData {
        std::string relative_path;
        uint64_t data_length;
        uint64_t data_offset;
        FileMetaData(const std::string& path, uint64_t length, uint64_t offset)
            : relative_path(path), data_length(length), data_offset(offset) {
        }
    };
    HANDLE hFile;
    Header header{ NULL };
    std::vector<FileMetaData> files_meta_data;
    std::vector<PendingChange> pending_changes;
};

class GFSUnpacker {
private:
    struct MetaInfo
    {
        unsigned __int64 File_Path_Lenght;
        std::vector<char> File_Path;
        unsigned __int64 File_Lenght;
    };
public:
    void operator()(const std::filesystem::path& filetounpackcs);
};

class GFSPacker {
private:
    __int64 file_identifier_length = std::byteswap(uint64_t(20));
    char file_identifier[20]{ 'R', 'e', 'v', 'e' ,'r' , 'g', 'e', ' ', 'P', 'a', 'c', 'k', 'a', 'g', 'e', ' ', 'F', 'i', 'l', 'e' }; //Reverge Package File
    __int64 file_version_length = std::byteswap(uint64_t(3));
    char file_version[3]{ '1', '.', '1' }; //Reverge Package File
    unsigned int file_aligned = std::byteswap(uint32_t(0x1));
public:
    void operator()(const std::filesystem::path& filestopackcs);
};

inline std::vector<GFSEdit> GFS_CHANGES; //Only for CC;