#ifndef BASERAR_H
#define BASERAR_H
#include <iostream>
#include <iterator>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <thread>
#include "keyboard.h"



class Keyboard;

class BaseRAR;

enum STATE_HEADER {STATE_MARKER_HEADER = 0, STATE_MAIN_HEADER = 1, STATE_FILE_HEADER = 2, STATE_SERVICE_HEADER = 3, STATE_END_OF_ARCHIVE = 5};

class BaseRAR {
public:
    std::fstream *file;
	std::streampos end;
	
    BaseRAR(std::fstream &file1): file(&file1){
        end=file->tellg();
    };

    virtual bool setStateHeader() = 0; 								//{ return bool();};
    virtual bool readNextBlock() = 0;								//{ return bool();};
    virtual void printInfo(size_t index, Keyboard &keyboard) = 0;	//{};
    virtual size_t getSizeHeaders() = 0;							//{ return 0; }
    virtual void deleteHeader(int index) = 0;						//

    virtual ~BaseRAR() {};
};


#endif
