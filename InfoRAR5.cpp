#include "InfoRAR5.h"
const char InfoRAR5::signature[8] {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07, 0x1, 0x0};

InfoRAR5::InfoRAR5(std::vector<char> &data_) : BaseRAR(data_){
    seek = 8;
}

bool InfoRAR5::readMainHead() {
    pos = 4;
    sizeHeader = getVInteger();
    seek += pos;
    pos = 0;
    std::cout << "main head size = " << sizeHeader << std::endl;

    if(!setStateHeader()) {
        std::cout << "main head type not found = " << std::endl;
    }
    if(state != STATE_MAIN_HEADER) {
        std::cout << "this is not main header" << std::endl;
    }
    flagHeader = getVInteger();
    return true;
}

bool InfoRAR5::setStateHeader()
{
    typeHeader = getVInteger();
    std::cout << "type header = " << typeHeader << std::endl;
    switch (typeHeader) {
    case 1:
        state = STATE_MAIN_HEADER;
        return true;;
    case 2:
        state = STATE_FILE_HEADER;
        return true;;
    case 3:
        return true;;
    case 4:
        return true;;
    case 5:
        return true;;

    }
    return false;
}

InfoRAR5::~InfoRAR5(){}

/*
 ****************************** NEXT HEADER BLOCK ********************************
 */
bool InfoRAR5::readNextBlock() {
    seek += sizeHeader;
    std::cout << std::endl;

    pos = 4;
    sizeHeader = getVInteger();
    seek += pos;
    pos = 0;
    std::cout << "next block head size =" << sizeHeader << std::endl;

    Block block;
    block.data.resize(sizeHeader);
    for(int i = 0; i < sizeHeader; i++ ) {
        block.data[i] = data->at(seek + i);
        //            std::cout << std::dec << "i=" << i << ":" << std::hex << (0xFF&data[head.seek +i]) << " " << std::flush;
    }
    std::for_each(block.data.begin(), block.data.end(), CHAR_OUT_LAMBDA);
    std::cout << std::endl;
    if(!setStateHeader())
        return false;
    if(state == STATE_FILE_HEADER) {
        std::cout << "this is file block" << std::endl;
        vint_t header_flag = getVInteger();
        vint_t header_area_size = getVInteger();
        vint_t header_data_size = getVInteger();
        vint_t header_file_flags = getVInteger();
        vint_t header_unpacket_size = getVInteger();
        vint_t header_attributes = getVInteger();
        if(header_file_flags & 0x02) {
            uint32_t header_mtime = *reinterpret_cast<const uint32_t*>(data->at(pos));
            pos+=4;
        }
        uint32_t header_CRC = *reinterpret_cast<const uint32_t*>(&data->data()[seek+pos]);
        pos+=4;
        vint_t header_compression_info = getVInteger();
        vint_t header_host_os = getVInteger();
        vint_t header_name_length = getVInteger();

//        getVInteger(&head.data[0], header_area_size, pos, head.data.size());
//        getVInteger(&head.data[0], header_data_size, pos, head.data.size());
//        getVInteger(&head.data[0], header_file_flags, pos, head.data.size());
//        getVInteger(&head.data[0], header_unpacket_size, pos, head.data.size());
//        getVInteger(&head.data[0], header_attributes, pos, head.data.size());

//        getVInteger(&head.data[0], header_compression_info, pos, head.data.size());
//        getVInteger(&head.data[0], header_host_os, pos, head.data.size());
//        getVInteger(&head.data[0], header_name_length, pos, head.data.size());
        for(int i = pos+seek; i < seek+pos+header_name_length; i++)
            std::cout << data->at(i);
        pos+=header_name_length;

//        if(header_file_flags & 0x01) {
//            std::cout << "attr = r" << std::endl;
//        }
//        if(header_file_flags & 0x02) {
//            std::cout << "attr = h" << std::endl;
//        }
//        if(header_file_flags & 0x04) {
//            std::cout << "attr = s" << std::endl;
//        }
//        if(header_file_flags & 0x10 || *head.attribut & 0x4000) {
//            std::cout << "attr = d" << std::endl;
//        }
//        if(header_file_flags & 0x20) {
//            std::cout << "attr = a" << std::endl;
//        }
        std::cout << std::endl;
    }
    return true;

}

vint_t InfoRAR5::getVInteger() {
    vint_t result=0;
    const char *ptr_ch = data->data() + seek;
    for (uint shift=0; pos<data->size(); shift+=7)
    {
        uint8_t curByte=(ptr_ch)[pos++];
        result+=uint64_t(curByte & 0x7f)<<shift;
        if ((curByte & 0x80)==0) {
//            seek += pos;
            return result;
        }
    }
    throw ;
}
