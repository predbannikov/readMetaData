#ifndef INFOARCH_H
#define INFOARCH_H

#include <fstream>


#include "InfoRAR4x.h"
#include "InfoRAR5.h"


class InfoArch {
    std::string FileName;
    std::ifstream file;
    std::vector<char> data;
    std::string rar_version;

    int size = 0;

public:
    InfoArch(){
        rar_version = "nan";
    }
    ~InfoArch() {
    }


    BaseRAR *rar;
    bool open(std::string FileName_1) {
        FileName = FileName_1;
        file.open(FileName_1, std::ios::in | std::ios::out | std::ios::binary);
        if(!file.is_open()) {
            std::cout << "error open file " << FileName_1 << std::endl;
            return false;
        }
        std::streampos begin = file.tellg();
        file.seekg(0, std::ios::end); 			// относительно 0-вой позиции перешли в конец
        std::streampos end = file.tellg();
        size = end-begin; 						// размер всего архива
        file.seekg(std::ios::beg); 				// перешли в начало

        data.resize(size);
        file.read(&data[0], size);
        // в data поместили архив
        std::cout << "\nread " << data.size() << " bytes" << std::endl;
        file.close();
        if(!checkArchive()) // если не формата RAR или RAR, но несовременной версии 5.0
            return false;
        std::cout << "Check archive success!\nFound: " << rar_version << std::endl;
        work();
        delete rar;
        return true;
    }


    bool checkArchive() {
        if(std::equal(std::begin(InfoRAR5::signature), std::end(InfoRAR5::signature), data.data())) {
            rar_version = "RAR 5.0";
            rar = new InfoRAR5(data);
            return true;
        } else if(std::equal(std::begin(InfoRAR4x::signature), std::end(InfoRAR4x::signature), data.data())) {
            rar_version = "RAR 4.x";
            std::cout <<"unsupported " << rar_version << std::endl;
            return false;
        }
        return false;
    }

    void work() {

        while(rar->readNextBlock()) {

        }

    }



};

#endif
