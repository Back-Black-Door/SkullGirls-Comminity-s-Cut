#include "gfs.h"
#include "Reader_Writer.h"



GFS::GFS(std::filesystem::path pathtoread) {

	if (pathtoread.extension() == "") {
        std::cout << "We are read folder?" << '\n';
        for (const std::filesystem::directory_entry& dir_entry : std::filesystem::recursive_directory_iterator(pathtoread)) {

            if (dir_entry.is_regular_file()) {
                
                

                std::string pathString = dir_entry.path().generic_string().erase(0, pathtoread.generic_string().size() + 1);
                std::vector<unsigned char> i_file_path(pathString.begin(), pathString.end());

                std::ifstream file(dir_entry.path(), std::ifstream::binary);
                //file.is_open();
                file.seekg(0, std::ios::end);
                std::streamsize size = file.tellg();
                file.seekg(0, std::ios::beg);

                std::vector<unsigned char> buffer(size);
                file.read(reinterpret_cast<char*>(buffer.data()), size);

                FileInfStruct i_struct
                {
                 i_file_path.size(),
                 i_file_path,
                 size,
                 1, //Check this
                };

                header.number_of_files++;
                header.dataOffset += i_struct.size();

                file_MetaData.push_back(i_struct);

                file_FileData.push_back(buffer);

                file.close();

            }
        }
        return;
    }
	else {
        std::cout << "We are read file?" << '\n';

        std::ifstream file(pathtoread, std::ifstream::binary); // Открываем файл в бинарном режиме
        
        if (file.is_open()) {
            try
            {
            
            file.seekg(0, std::ios::beg); 

            
            std::vector<unsigned char> bufferheader(0x33); // Создаем вектор для хранения байтов

            

            file.read(reinterpret_cast<char*>(bufferheader.data()), 0x33); // Читаем данные из файла

            //header Read
            header.dataOffset = readBuffer_VectorUnChar_to_UnInt32(bufferheader,0);
            header.file_identifier_length = readBuffer_VectorUnChar_to_UnInt64(bufferheader, 4);
            header.file_identifier = readBuffer_VectorUnChar_to_VectorUnChar(bufferheader, 12, header.file_identifier_length);
            header.file_version_lenght = readBuffer_VectorUnChar_to_UnInt64(bufferheader, (12 + header.file_identifier_length));
            header.file_version = readBuffer_VectorUnChar_to_VectorUnChar(bufferheader, (12 + header.file_identifier_length + 8), header.file_version_lenght);
            header.number_of_files = readBuffer_VectorUnChar_to_UnInt64(bufferheader, (12 + header.file_identifier_length + 8 + header.file_version_lenght));
            
            file.seekg(0x33, std::ios::beg);
            std::vector<unsigned char> bufferfilemeta(header.dataOffset - 0x33);
            file.read(reinterpret_cast<char*>(bufferfilemeta.data()), header.dataOffset - 0x33);

            //Meta_Info Reader
            int BytesOffsetMETA{0};
            for (size_t i{ 0 }; i < header.number_of_files; i++) {
                
                unsigned __int64 i_file_path_length
                    = readBuffer_VectorUnChar_to_UnInt64(bufferfilemeta, (BytesOffsetMETA));
                std::vector<unsigned char> i_reference_path = readBuffer_VectorUnChar_to_VectorUnChar(bufferfilemeta, BytesOffsetMETA +  8 , i_file_path_length );
                unsigned __int64 i_file_length = readBuffer_VectorUnChar_to_UnInt64(bufferfilemeta, (BytesOffsetMETA + 8 + i_file_path_length));
                unsigned __int32 i_file_alignment = readBuffer_VectorUnChar_to_UnInt32(bufferfilemeta, (BytesOffsetMETA + 8 + i_file_path_length + 8));

                FileInfStruct i_struct
                { i_file_path_length,     
                 i_reference_path, 
                 i_file_length,
                 i_file_alignment,
                };

                file_MetaData.push_back(i_struct); 
                BytesOffsetMETA += file_MetaData[i].size();
            }


            //File_Data Reader
            int BytesOffsetDATA{ 0 };
            for (size_t i{ 0 }; i < header.number_of_files; i++) {

                file.seekg(header.dataOffset + BytesOffsetDATA, std::ios::beg);

                std::vector<unsigned char> bufferfiledata(file_MetaData[i].file_length);

                file.read(reinterpret_cast<char*>(bufferfiledata.data()), file_MetaData[i].file_length);

                std::vector<unsigned char> i_File_Data = readBuffer_VectorUnChar_to_VectorUnChar(bufferfiledata,0,file_MetaData[i].file_length);
                file_FileData.push_back(i_File_Data);
                BytesOffsetDATA += file_MetaData[i].file_length;
            }

            file.close(); //close file
            }
            catch (std::string error_message)
            {
                std::cerr << "Can't read file" << std::endl;
                std::cerr << "Reason: " << error_message << std::endl;
            }
        }
        else {
            std::cerr << "Can't open file" << std::endl;
        }
	}
}

void GFS::write_GFS(std::filesystem::path pathtowrite) {
    
    if (pathtowrite.extension() == "") {
        std::cout << "We are write folder" << '\n';
        for(size_t i{ 0 }; i < header.number_of_files; i++)
        {
            std::string my_string(file_MetaData[i].reference_path.begin(), file_MetaData[i].reference_path.end());
            std::filesystem::path filepath = pathtowrite / my_string;
            std::filesystem::path n = filepath.parent_path();
            std::filesystem::create_directories(filepath.parent_path());
            std::ofstream file(filepath, std::ios::out | std::ifstream::binary);
            file.write(reinterpret_cast<char*>(file_FileData[i].data()), file_MetaData[i].file_length);
            file.close();
        }
        return;
    }
    else {
        std::cout << "We are write file" << '\n';

        std::ofstream file(pathtowrite, std::ofstream::binary);

        //write header
        file.write(reinterpret_cast<char*>(convert_UnInt32_to_VectorChar(header.dataOffset).data()), sizeof(unsigned __int32));
        file.write(reinterpret_cast<char*>(convert_UnInt64_to_VectorChar(header.file_identifier_length).data()), sizeof(unsigned __int64));
        file.write(reinterpret_cast<char*>(header.file_identifier.data()), header.file_identifier_length);
        file.write(reinterpret_cast<char*>(convert_UnInt64_to_VectorChar(header.file_version_lenght).data()), sizeof(unsigned __int64));
        file.write(reinterpret_cast<char*>(header.file_version.data()), header.file_version_lenght);
        file.write(reinterpret_cast<char*>(convert_UnInt64_to_VectorChar(header.number_of_files).data()), sizeof(unsigned __int64));

        //write file_MetaData
        for (size_t i{ 0 }; i < header.number_of_files; i++) {
            file.write(reinterpret_cast<char*>(convert_UnInt64_to_VectorChar(file_MetaData[i].file_path_length).data()), sizeof(unsigned __int64));
            file.write(reinterpret_cast<char*>(file_MetaData[i].reference_path.data()), file_MetaData[i].file_path_length);
            file.write(reinterpret_cast<char*>(convert_UnInt64_to_VectorChar(file_MetaData[i].file_length).data()), sizeof(unsigned __int64));
            file.write(reinterpret_cast<char*>(convert_UnInt32_to_VectorChar(file_MetaData[i].file_alignment).data()), sizeof(unsigned __int32));
        }

        //write file_FileData
        for (size_t i{ 0 }; i < header.number_of_files; i++) {
            file.write(reinterpret_cast<char*>(file_FileData[i].data()), file_MetaData[i].file_length);
        }
        file.close();
        return;
    }
}



void GFS::HeaderStruct::print_Header() {
    std::cout << "Data Offset: " << dataOffset << '\n';
    std::cout << "File Identifier Length: " << file_identifier_length << '\n';
    std::cout << "File Identifier: ";
    for (char n : file_identifier) {
        std::cout << n;
    }
    std::cout << '\n';
    std::cout << "File Version Lenght: " << file_version_lenght << '\n';
    std::cout << "file Version: ";
    for (char n : file_version) {
        std::cout << n;
    }
    std::cout << '\n';
    std::cout << "Number of Files: " << number_of_files << '\n';
}

int GFS::HeaderStruct::size() {
    return (4 + 8 + file_identifier_length + 8 + file_version_lenght + 8);
}



void GFS::FileInfStruct::print_File_MetaData() {
    std::cout << "File Path Length: " << file_path_length << '\n';
    std::cout << "Reference Path: ";
    for (char n : reference_path) {
        std::cout << n;
    }
    std::cout << '\n';
    std::cout << "Reference Length: " << file_length << '\n';
    std::cout << "Reference Alignment: " << file_alignment << '\n';
}

int GFS::FileInfStruct::size() {
    return (8 + file_path_length + 8 + 4);
}

void GFS::print_file_FileData(int index) {
    std::cout << "File Data: " << '\n';
    std::cout << '\n';
    for (unsigned char n : file_FileData[index]) {
        std::cout << n;
    }
    
}
 
void GFS::add_files(GFS gfsAdd) {
    int filesalready{ 0 };
    for (size_t n{ 0 }; n < header.number_of_files; n++) {
        for (size_t i{ 0 }; i < gfsAdd.header.number_of_files; i++) {
            if (file_MetaData[n].reference_path == gfsAdd.file_MetaData[i].reference_path) {
                file_MetaData[n] = gfsAdd.file_MetaData[i];
                auto iter_file_MetaData = gfsAdd.file_MetaData.cbegin();
                gfsAdd.file_MetaData.erase(iter_file_MetaData + i);
                file_FileData[n] = gfsAdd.file_FileData[i];
                auto iter_file_FileData = gfsAdd.file_FileData.cbegin();
                gfsAdd.file_FileData.erase(iter_file_FileData + i);
                filesalready++;
            }
        }
    }

    header.number_of_files += (gfsAdd.header.number_of_files - filesalready);

    for (auto n : gfsAdd.file_MetaData) {
        header.dataOffset += n.size();
        file_MetaData.push_back(n);
    }
    for (auto n : gfsAdd.file_FileData) {
        file_FileData.push_back(n);
    }

}