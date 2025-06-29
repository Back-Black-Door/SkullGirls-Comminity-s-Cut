#include <filesystem>
#include "Reader_Writer.h"
#include <string>
#include <fstream>



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
		void addGFStoGFS(GFS gfsAdd);

};