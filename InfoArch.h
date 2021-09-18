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
        file.seekg(0, std::ios::end); // îòíîñèòåëüíî 0-âîé ïîçèöèè ïåðåøëè â êîíåö
        std::streampos end = file.tellg();
        size = end-begin; //ðàçìåð âñåãî àðõèâà
        file.seekg(std::ios::beg); // ïåðåøëè â íà÷àëî

        data.resize(size);
        file.read(&data[0], size);
        // â data ïîìåñòèëè àðõèâ
        std::cout << "\nread " << data.size() << " bytes" << std::endl;
        file.close();
        if(!checkArchive()) // åñëè íå ôîðìàòà RAR èëè RAR, íî íåñîâðåìåííîé âåðñèè 5.0
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
