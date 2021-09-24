#include "InfoRAR4x.h"

const char InfoRAR4x::signature[7]  {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07, 0x0};


InfoRAR4x::InfoRAR4x(std::fstream &file_):BaseRAR(file_){

}


//bool InfoRAR4x::readMainHead() {
//    int length = 7;                     // Для 7 байтной сигнатуры
//    for(int i = 0; i < length; i++) {
////        block.data[i] = data->at(seek + i);
//    }

//    if(block.data[2] != 0x73)
//        std::cout << "what not right" << std::endl;

//    block.next_head_size = *reinterpret_cast<uint16_t*>(block.data[5]);

//    block.seek += length;              // сдвинули положение ещё следующие 7 байт
//    return true;
//}

bool InfoRAR4x::readNextBlock()
{
//    int length = 7;                     // Для 7 байтной сигнатуры
//    seek += sizeHeader - length;
//    std::cout << std::endl;
//    for(int i = 0; i < length; i++) {
//        //            std::cout << std::setw(2) << std::setfill('0') << std::hex << (0xFF&head.data[i]) << " ";
//        head.data[i] = data[head.seek + i];
//    }
//    std::cout << "head.next_size =" << sizeHeader << std::endl;
//    //        head.seek += length;              // сдвинули положение ещё следующие 7 байт
//    //        for(int i = 0; i < 7; i++)

//    sizeHeader = *reinterpret_cast<const uint16_t*>(&data->data()[5]);

//    std::cout << "head.next_size =" << sizeHeader << std::endl;
//    char *block = new char[sizeHeader]();
//    std::cout << "head.next_size =" << head.next_head_size << std::endl;
//    for(int i = 0; i < head.next_head_size; i++ ) {
//        block[i] = data[head.seek + i];
//        std::cout << std::dec << "i = " << i << "  " << data[head.seek +i] << " " <<  std::hex << (0xFF&data[head.seek +i]) << std::endl;
//    }
//    std::cout << std::endl;
//    if(block[2] == 0x74) {
//        std::cout << "this is file block" << std::endl;
//        head.packSize = reinterpret_cast<uint32_t*>(block + 7);
//        head.seek += *head.packSize;
//        head.attribut = reinterpret_cast<uint32_t*>(block + 28);
//        if(*head.attribut & 0x01) {
//            std::cout << "attr = r" << std::endl;
//        }
//        if(*head.attribut & 0x02) {
//            std::cout << "attr = h" << std::endl;
//        }
//        if(*head.attribut & 0x04) {
//            std::cout << "attr = s" << std::endl;
//        }
//        if(*head.attribut & 0x10 || *head.attribut & 0x4000) {
//            std::cout << "attr = d" << std::endl;
//        }
//        if(*head.attribut & 0x20) {
//            std::cout << "attr = a" << std::endl;
//        }
//        uint16_t lengthName = *reinterpret_cast<uint16_t*>(block+26);
//        std::cout << "filename ";
//        for(int i = 32; i < 32+lengthName; i++)
//            std::cout << block[i] << std::endl;
//        std::cout << std::endl;
//    } else {
//        uint16_t flag = *reinterpret_cast<uint16_t*>(block + 3);
//        std::cout << "flag =" << flag << std::endl;
//        if(flag & 0x8000) {
//            uint32_t add_size = *reinterpret_cast<uint32_t*>(block + 7);
//        }
//    }

    return false;
}

InfoRAR4x::~InfoRAR4x(){}
