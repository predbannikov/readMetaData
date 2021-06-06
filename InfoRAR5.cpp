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
#define EMPTY_SPACE_LEFT    5
#define EMPTY_SPACE_AFTER_LEFT    45
#define EMPTY_SPACE_RIGHT_NUMBER    7
        std::cout << "this is file block" << std::endl;
        vint_t header_flag = getVInteger();
//        std::string sHeaderFlags = std::string("Flags header: " + std::to_string(header_flag));
        vint_t header_extra_area_size = getVInteger();
        if(header_flag & 0x01) {
            std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Extra area size: " << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header_extra_area_size) << std::endl;
        }
        vint_t header_data_size = getVInteger();
        if(header_flag & 0x02) {
            std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Data area size: " << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header_data_size) << std::endl;
        }
        vint_t header_file_flags = getVInteger();
        if(header_file_flags & 0x1) std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Directory file system object." << std::endl;
        if(header_file_flags & 0x2) std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Time field in Unix format is present." << std::endl;
        if(header_file_flags & 0x4) std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "CRC32 field is present. " << std::endl;
        if(header_file_flags & 0x8) std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Unpacked size is unknown. " << std::endl;

        vint_t header_unpacket_size = getVInteger();
        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Unpacked file or service data size:" << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header_unpacket_size) << std::endl;
        vint_t header_attributes = getVInteger();
        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Operating system specific file attributes:" << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header_attributes) << std::endl;
        if(header_file_flags & 0x02) {
            uint32_t header_mtime = *reinterpret_cast<const uint32_t*>(data->at(pos));
            std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Modification time in Unix time format:" << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header_mtime) << std::endl;

            pos+=4;
        }
        uint32_t header_CRC = *reinterpret_cast<const uint32_t*>(&data->data()[seek+pos]);
        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "CRC32 of unpacked file or service data:" << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header_CRC) << std::endl;        pos+=4;
        vint_t header_compression_info = getVInteger();
        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Compression information:" << std::endl;
        std::cout << std::setw(EMPTY_SPACE_LEFT*2) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT-EMPTY_SPACE_LEFT) << "method: " << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header_compression_info & 0x0380) << std::endl;
        vint_t header_host_os = getVInteger();
        if(header_host_os & 0x0)
            std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "UNIX used to create this archive" << std::endl;
        else
            std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "WINDOWS used to create this archive" << std::endl;

        vint_t header_name_length = getVInteger();
        std::string name_file;
        for(int i = pos+seek; i < seek+pos+header_name_length; i++)
            name_file += data->at(i);
        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Name:" << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << name_file << std::endl;
        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Extra area:" << std::endl;
        std::cout << std::setw(EMPTY_SPACE_LEFT*2) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT-EMPTY_SPACE_LEFT) << "\"" ;
        pos+=header_name_length;

        if(header_flag & 0x01) {
            std::string extra_area_data;

            for(int i = pos+seek; i < seek+pos+header_extra_area_size; i++) {
                if(data->at(i) > 31)
                    extra_area_data += data->at(i);
            }
            pos+=header_extra_area_size;
            std::cout << extra_area_data << "\"" << std::endl;
        }
        if(header_flag & 0x02) {
            std::string data_file;
            std::copy(data->begin()+pos+seek, data->end()+pos+seek+header_data_size, data_file);
//            for(int i = pos+seek; i < seek+pos+header_data_size; i++) {
//                if(data->at(i) > 31)
//                    std::cout << data->at(i) << std::flush;
//            }
            std::cout << data_file << std::endl;
        }

//        pos+=header_name_length;
        std::cout << std::endl << name_file << std::endl;

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

std::string InfoRAR5::getStrSizeOfExtraArea(vint_t dgt)
{
    std::string str;
    return str;
}

std::string InfoRAR5::getStrSizeOfData(vint_t dgt)
{
    std::string str;
    str = "\tData area size: " + std::to_string(dgt);
    return str;
}
