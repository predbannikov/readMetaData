#ifndef BASERAR_H
#define BASERAR_H
#include <iostream>
#include <vector>
#include <iterator>
#include <iomanip>
#include <algorithm>






class BaseRAR;

enum STATE_HEADER {STATE_MARKER_HEADER = 0, STATE_MAIN_HEADER = 1, STATE_FILE_HEADER = 2, STATE_SERVICE_HEADER = 3, STATE_END_OF_ARCHIVE = 5};

class BaseRAR {
public:
    size_t seek;
    size_t type_header;
    const std::vector<char> *data;

    std::ios_base::fmtflags defFlags;
    BaseRAR(const std::vector<char> &data_) : data(&data_){
        defFlags = std::cout.flags();
//
        seek = 7;
    };

    virtual bool setStateHeader(){ return bool();};
    //virtual bool readMainHead(){ return bool();};
    virtual bool readNextBlock(){ return bool();};

    ~BaseRAR() {};
    std::vector<char>::const_iterator pos;
    std::string strInfo;
};


#endif
