#ifndef INFORAR4X_H
#define INFORAR4X_H
#include "baserar.h"



class InfoRAR4x : public BaseRAR{
public:
    static const char signature[7];
    InfoRAR4x(std::vector<char> &data_):BaseRAR(data_){};

    //************* MAIN HEADER *************//
    bool readMainHead() override;
    bool readNextBlock() override;
    ~InfoRAR4x();

//    void readNextBlock() {
//        int length = 7;                     // Для 7 байтной сигнатуры
//        head.seek += head.next_head_size - length;
//        std::cout << std::endl;
//        for(int i = 0; i < length; i++) {
////            std::cout << std::setw(2) << std::setfill('0') << std::hex << (0xFF&head.data[i]) << " ";
//            head.data[i] = data[head.seek + i];
//        }
//        std::cout << "head.next_size =" << head.next_head_size << std::endl;
////        head.seek += length;              // сдвинули положение ещё следующие 7 байт
////        for(int i = 0; i < 7; i++)

//        head.next_head_size = *reinterpret_cast<uint16_t*>(head.data[5]);

//        std::cout << "head.next_size =" << head.next_head_size << std::endl;
//        char *block = new char[head.next_head_size]();
//        std::cout << "head.next_size =" << head.next_head_size << std::endl;
//        for(int i = 0; i < head.next_head_size; i++ ) {
//            block[i] = data[head.seek + i];
//            std::cout << std::dec << "i = " << i << "  " << data[head.seek +i] << " " <<  std::hex << (0xFF&data[head.seek +i]) << std::endl;
//        }
//        std::cout << std::endl;
//        if(block[2] == 0x74) {
//            std::cout << "this is file block" << std::endl;
//            head.packSize = reinterpret_cast<uint32_t*>(block + 7);
//            head.seek += *head.packSize;
//            head.attribut = reinterpret_cast<uint32_t*>(block + 28);
//            if(*head.attribut & 0x01) {
//                std::cout << "attr = r" << std::endl;
//            }
//            if(*head.attribut & 0x02) {
//                std::cout << "attr = h" << std::endl;
//            }
//            if(*head.attribut & 0x04) {
//                std::cout << "attr = s" << std::endl;
//            }
//            if(*head.attribut & 0x10 || *head.attribut & 0x4000) {
//                std::cout << "attr = d" << std::endl;
//            }
//            if(*head.attribut & 0x20) {
//                std::cout << "attr = a" << std::endl;
//            }
//            uint16_t lengthName = *reinterpret_cast<uint16_t*>(block+26);
//            std::cout << "filename ";
//            for(int i = 32; i < 32+lengthName; i++)
//                std::cout << block[i] << std::endl;
//            std::cout << std::endl;
//        } else {
//                uint16_t flag = *reinterpret_cast<uint16_t*>(block + 3);
//                std::cout << "flag =" << flag << std::endl;
//                if(flag & 0x8000) {
//                    uint32_t add_size = *reinterpret_cast<uint32_t*>(block + 7);
//                }
//        }


//    }
};

#endif  // INFORAR4x_H
