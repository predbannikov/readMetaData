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

enum STATE_HEADER {STATE_MARKER_HEADER = 0, STATE_MAIN_HEADER = 1, STATE_FILE_HEADER = 2};

class BaseRAR {
public:
    STATE_HEADER state = STATE_MARKER_HEADER;
    size_t seek;
    size_t sizeHeader;
    size_t typeHeader;
    const std::vector<char> *data;
    std::vector<Block*> blocks;
    BaseRAR(const std::vector<char> &data_) : data(&data_){
        block.data.resize(data->size());
        seek = 7;
    };

    virtual bool setStateHeader(){};
    virtual bool readMainHead(){};
    virtual bool readNextBlock(){};

    ~BaseRAR() {};

    Block block;
    std::string strInfo;
};


#endif
