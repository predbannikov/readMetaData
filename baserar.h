#ifndef BASERAR_H
#define BASERAR_H
#include <iostream>
#include <vector>
#include <iterator>
#include <iomanip>
#include <algorithm>



#define HEX_OUT_LAMBDA [](const unsigned char & byte) { std::cout << std::hex << std::setfill('0') << std::setw(2) << (0xFF&byte) << " " << std::flush; }

#define CHAR_OUT_LAMBDA [](const unsigned char & byte) { if(byte > 31 && byte ) {std::cout << byte << std::flush; } }



struct Block {
    uint16_t crc;
    uint16_t flags;
    uint16_t next_head_size;
    uint32_t *add_size;
    uint8_t *type;
    uint32_t *fullSize;
    uint32_t *packSize;
    uint32_t *attribut;
    int seek;
    std::vector<char> data;
    uint8_t length;

};


class BaseRAR;

enum STATE_HEADER {STATE_MARKER_HEADER = 0, STATE_MAIN_HEADER = 1, STATE_FILE_HEADER = 2, STATE_END_OF_ARCHIVE_HEADER = 5};

class BaseRAR {
public:
    STATE_HEADER state = STATE_MARKER_HEADER;
    size_t seek;
    size_t sizeHeader;
    size_t typeHeader;
    const std::vector<char> *data;
    std::vector<Block*> blocks;
//    std::ios_base::fmtflags hexFlags;
    std::ios_base::fmtflags defFlags;
    BaseRAR(const std::vector<char> &data_) : data(&data_){
        block.data.resize(data->size());
        defFlags = std::cout.flags();
//        std::cout << std::hex << std::setfill('0') << std::setw(2) << std::uppercase;
//        hexFlags = std::cout.flags();
//        std::cout.flags(defFlags);
        seek = 7;
    };

    virtual bool setStateHeader() { return bool();};
    virtual bool readNextBlock() { return bool();};

    ~BaseRAR() {};
    std::vector<char>::const_iterator pos;
    Block block;
    std::string strInfo;
};


#endif
