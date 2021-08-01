#ifndef INFOARCH_H
#define INFOARCH_H

#include <fstream>


#include "InfoRAR4x.h"
#include "InfoRAR5.h"


class InfoArch {
    std::string m_sFileName;
    std::ifstream file;
    std::vector<char> vec;
//    char *data;
    std::vector<char> data;
    std::string rar_version;
    int cur_index = 0;
    int32_t size_header;
    int32_t header_type;
    bool add_size = false;
    int size = 0;

public:
    InfoArch(){
        rar_version = "nan";
    }
    ~InfoArch() {
    }
    BaseRAR *rar;
    bool open(std::string a_sFileName) {
        m_sFileName = a_sFileName;
        file.open(a_sFileName, std::ios::in | std::ios::out | std::ios::binary);
        if(!file.is_open()) {
            std::cout << "error open file " << a_sFileName << std::endl;
            return false;
        }
        std::streampos begin = file.tellg();
        file.seekg(0, std::ios::end);
        std::streampos end = file.tellg();
        size = end-begin;
        file.seekg(std::ios::beg);
//        data = new char[size];
        data.resize(size);
        file.read(&data[0], size);
        std::cout << "\nread " << data.size() << " bytes" << std::endl;
        file.close();
        if(!checkArchive())
            return false;
        std::cout << "Check archive success!\nFound: " << rar_version << std::endl;
        init();
        return true;
    }


    bool checkArchive() {
        if(std::equal(std::begin(InfoRAR5::signature), std::end(InfoRAR5::signature), data.data())) {
            rar_version = "RAR 5.0";
            rar = new InfoRAR5(data);
            return true;
        } else if(std::equal(std::begin(InfoRAR4x::signature), std::end(InfoRAR4x::signature), data.data())) {
            rar_version = "RAR 4.x";
            rar = new InfoRAR4x(data);
            return true;
        }
        return false;
    }

    void init() {

        // TODO запустить разбор в цикле
        // читать readNextBlock
        // пока не закончится файл
        rar->readNextBlock();
        rar->readNextBlock();
        // ...
    }


    void readMetaData() {




    }
};

#endif  // INFOARCH_H

