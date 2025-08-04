#include "Reader_Writer.h"
#include "include.h"



class GFS{
	public: //Change to private later
		struct HeaderStruct {
			unsigned __int32 dataOffset{51};
			unsigned __int64 file_identifier_length{20};
			std::vector<unsigned char> file_identifier = { 'R', 'e', 'v', 'e' ,'r' , 'g', 'e', ' ', 'P', 'a', 'c', 'k', 'a', 'g', 'e', ' ', 'F', 'i', 'l', 'e' }; //Reverge Package File
			unsigned __int64 file_version_lenght{ 3 };
			std::vector<unsigned char> file_version = { '1', '.', '1' };
			unsigned __int64 number_of_files{0};
			int size();
			public:
			void print_Header();
		};
		struct FileInfStruct {
			unsigned __int64 file_path_length{};
			std::vector<unsigned char> reference_path;
			unsigned __int64 file_length;
			unsigned __int32 file_alignment;
			int size();
			public:
			void print_File_MetaData();
		};


		
	public:
		HeaderStruct header;
		std::vector<FileInfStruct> file_MetaData{};
		std::vector<std::vector<unsigned char>> file_FileData{};

		GFS(std::filesystem::path pathtoread);
		void print_file_FileData(int index);
		void write_GFS(std::filesystem::path pathtoread);
		void add_files(GFS gfsAdd);
};


class GFSUnpacker
{
	struct MetaInfo
	{
		unsigned __int64 File_Path_Lenght;
		std::vector<char> File_Path;
		unsigned __int64 File_Lenght;
	};
public:
	void operator()(const std::filesystem::path& filetounpackcs) const
	{
		std::filesystem::path filetounpack = filetounpackcs;
		unsigned __int64 number_of_files{ 0 };
		unsigned int offset_to_filedata{ 0 };
		FILE* fGFSMetaInfo = fopen(filetounpack.string().c_str(), "rb");
		FILE* fGFSFileData = fopen(filetounpack.string().c_str(), "rb");
		std::fread(&offset_to_filedata, sizeof offset_to_filedata, 1, fGFSMetaInfo);
		offset_to_filedata = swap_endian_32(offset_to_filedata);
		std::fseek(fGFSMetaInfo, 0x2B, SEEK_SET);
		std::fread(&number_of_files, sizeof number_of_files, 1, fGFSMetaInfo);
		number_of_files = swap_endian_64(number_of_files);
		std::fseek(fGFSMetaInfo, 0x33, SEEK_SET);
		int Files_Lenght{ 0 };
		for (int i{ 0 }; i < number_of_files; i++) {
			MetaInfo CurrentFile;

			std::fread(&CurrentFile.File_Path_Lenght, 0x8, 1, fGFSMetaInfo);
			CurrentFile.File_Path_Lenght = swap_endian_64(CurrentFile.File_Path_Lenght);

			CurrentFile.File_Path.resize(CurrentFile.File_Path_Lenght);
			std::fread(reinterpret_cast<char*>(CurrentFile.File_Path.data()), CurrentFile.File_Path_Lenght, 1, fGFSMetaInfo);
			
			std::fread(&CurrentFile.File_Lenght, 0x8, 1, fGFSMetaInfo);
			CurrentFile.File_Lenght = swap_endian_64(CurrentFile.File_Lenght);

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
};


class GFSPacker {
	private:
	__int64 file_identifier_length = swap_endian_64(20);
	char file_identifier[20] { 'R', 'e', 'v', 'e' ,'r' , 'g', 'e', ' ', 'P', 'a', 'c', 'k', 'a', 'g', 'e', ' ', 'F', 'i', 'l', 'e' }; //Reverge Package File
	__int64 file_version_length = swap_endian_64(3);
	char file_version[3]{ '1', '.', '1'}; //Reverge Package File
	unsigned int file_aligned = swap_endian_32(0x1);

	public:
	void operator()(const std::filesystem::path& filestopackcs) const
	{	

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
				uint64_t i_file_path_lenght = swap_endian_64(i_file_path.size());
				FILE* CurrentFile = fopen(dir_entry.path().string().c_str(), "rb");
				std::fseek(CurrentFile, 0, SEEK_END); // seek to end
				const uint64_t filesize = std::ftell(CurrentFile);
				uint64_t FileSize = swap_endian_64(filesize);

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
		offset_to_filedata = swap_endian_32(offset_to_filedata);
		numbers_of_file = swap_endian_64(numbers_of_file);
		std::fwrite(&offset_to_filedata, 0x4, 1, fGFSMetaInfo);
		std::fwrite(&file_identifier_length, 0x8, 1, fGFSMetaInfo);
		std::fwrite(file_identifier, 20, 1, fGFSMetaInfo);
		std::fwrite(&file_version_length, 0x8, 1, fGFSMetaInfo);
		std::fwrite(file_version, 0x3, 1, fGFSMetaInfo);
		std::fwrite(&numbers_of_file, 0x8, 1, fGFSMetaInfo);
	}
};