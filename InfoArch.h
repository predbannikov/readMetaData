#ifndef INFOARCH_H
#define INFOARCH_H

#include <fstream>
#include "InfoRAR4x.h"
#include "InfoRAR5.h"
#include "keyboard.h"

class InfoArch {
    std::string FileName;
    std::fstream file;
    std::string rar_version;

    int size = 0;

public:
    InfoArch(){
        rar_version = "nan";
    }
    ~InfoArch() {
        file.close();
        delete rar;
    }
    Keyboard keyboard;
    BaseRAR *rar;
    bool open(std::string FileName_1) {
        FileName = FileName_1;
        file.open(FileName_1, std::ios::in | std::ios::binary);
        if(!file.is_open()) {
            std::cout << "error open file " << FileName_1 << std::endl;
            return false;
        }
        std::streampos begin = file.tellg();
        file.seekg(0, std::ios::end); 			// относительно 0-вой позиции перешли в конец
        std::streampos end = file.tellg();
        size = end-begin; 						// размер всего архива
        file.seekg(std::ios::beg); 				// перешли в начало

        std::cout << "\nread " << size << " bytes" << std::endl;
        if(!checkArchive()) // если не формата RAR или RAR, но несовременной версии 5.0
            return false;
        std::cout << "Check archive success!\nFound: " << rar_version << std::endl;
        while(rar->readNextBlock()) {
        }
        return true;
    }

    bool checkArchive() {
        char buff[8];
        file.read(buff, 8);
        int size = file.gcount();
        if(file.eof()) {
            std::cout << "eof" << std::endl;
            return false;
        }
        if(std::equal(std::begin(InfoRAR5::signature), std::end(InfoRAR5::signature), std::begin(buff))) {
            rar_version = "RAR 5.0";
            rar = new InfoRAR5(file);
            return true;
        } else if(std::equal(std::begin(InfoRAR4x::signature), std::end(InfoRAR4x::signature), std::begin(buff))) {
            rar_version = "RAR 4.x";
            std::cout <<"unsupported " << rar_version << std::endl;
            return false;
        }
        return false;
    }


    void work() {
        int key = 48;
        int width, height;
        int index = 0;
        const int count_headers = rar->getSizeHeaders();
        while(key != 27 && key != 'q') {
            if(key == 'j') {
                if(index < count_headers - 1) {
                    index++;
                }
            }
            if(key == 'k')
                if(index > 0) {
                    index--;
                }
            if(key == 'h') {
                if(index < count_headers - 1 - 10)
                    index += 10;
                else if (index < count_headers - 1)
                    index = count_headers -1;
            }
            if(key == 'l')
                if(index > 0 + 10)
                    index -= 10;
                else if(index > 0)
                    index = 0;
            if(key == 'd')
                rar->deleteHeader(index);
            rar->printInfo(index, keyboard);
            key = keyboard.__getch();
        }
        clrscr();
    };
};


#endif
