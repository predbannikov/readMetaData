#include "InfoRAR4x.h"

const char InfoRAR4x::signature[7]  {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07, 0x0};


bool InfoRAR4x::readMainHead() {
    int length = 7;                     // Для 7 байтной сигнатуры
    for(int i = 0; i < length; i++) {
//        block.data[i] = data->at(seek + i);
    }

    if(block.data[2] != 0x73)
        std::cout << "what not right" << std::endl;

    block.next_head_size = *reinterpret_cast<uint16_t*>(block.data[5]);

    block.seek += length;              // сдвинули положение ещё следующие 7 байт
    return true;
}

bool InfoRAR4x::readNextBlock()
{
    return true;
}

InfoRAR4x::~InfoRAR4x(){}
