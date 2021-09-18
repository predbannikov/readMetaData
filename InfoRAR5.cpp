#include "InfoRAR5.h"
const char InfoRAR5::signature[8] {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07, 0x1, 0x0};

InfoRAR5::InfoRAR5(std::vector<char> &data_) : BaseRAR(data_){
    pos = data->begin();
    std::advance(pos, 8);
}



bool InfoRAR5::setStateHeader()
{
    typeHeader = getVInteger();
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
        state = STATE_END_OF_ARCHIVE_HEADER;
        return true;;

    }
    return false;
}


InfoRAR5::~InfoRAR5(){}

/*
 *   READ HEADER BLOCK
 */
bool InfoRAR5::readNextBlock() {

    uint32_t CRC_HEADER = *reinterpret_cast<const uint32_t*>(&*pos);
    std::advance(pos,4);
    sizeHeader = getVInteger();
//    std::cout << std::endl << "read next block size =" << sizeHeader << std::endl;

    Block block;
    block.data.resize(sizeHeader);
    for(int i = 0; i < sizeHeader; i++ ) {
        block.data[i] = data->at(seek + i);
    }
//    std::for_each(block.data.begin(), block.data.end(), CHAR_OUT_LAMBDA);
//    std::cout << std::endl;
    if(!setStateHeader())
        return false;
    switch (state) {
    case STATE_MARKER_HEADER:
        break;
    case STATE_MAIN_HEADER:
        std::cout << "BLOCK MAIN HEAD size = " << sizeHeader << std::endl;

        std::advance(pos,sizeHeader-1);
        break;
    case STATE_FILE_HEADER: {
#define EMPTY_SPACE_LEFT    5
#define EMPTY_SPACE_AFTER_LEFT    45
#define EMPTY_SPACE_RIGHT_NUMBER    7
        std::cout << "BLOCK FILE HEAD size = " << sizeHeader << std::endl;
        vint_t header_flag = getVInteger();
        vint_t header_extra_area_size = getVInteger();
        vint_t header_data_size = getVInteger();
        vint_t header_file_flags = getVInteger();
        vint_t header_unpacket_size = getVInteger();
        vint_t header_attributes = getVInteger();

        uint32_t header_CRC = *reinterpret_cast<const uint32_t*>(&*pos);
        std::advance(pos, 4);

        vint_t header_compression_info = getVInteger();
        vint_t header_host_os = getVInteger();

        if(header_file_flags & 0x1) std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Directory file system object." << std::endl;
        if(header_file_flags & 0x2) std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Time field in Unix format is present." << std::endl;
        if(header_file_flags & 0x4) std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "CRC32 field is present. " << std::endl;
        if(header_file_flags & 0x8) std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Unpacked size is unknown. " << std::endl;
        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Unpacked file or service data size:" << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header_unpacket_size) << std::endl;
        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Operating system specific file attributes:" << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header_attributes) << std::endl;
        if(header_file_flags & 0x02) {
            uint32_t header_mtime = *reinterpret_cast<const uint32_t*>(&*pos);
            std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Modification time in Unix time format:" << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header_mtime) << std::endl;
            std::advance(pos, 4);
        }
        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "CRC32 of unpacked file or service data:" << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header_CRC) << std::endl;
        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Compression information:" << std::endl;
        std::cout << std::setw(EMPTY_SPACE_LEFT*2) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT-EMPTY_SPACE_LEFT) << "method: " << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header_compression_info & 0x0380) << std::endl;
        if(header_host_os & 0x0)
            std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "UNIX used to create this archive." << std::endl;
        else
            std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "WINDOWS used to create this archive." << std::endl;

        int *__restrict__ t;

        auto name_it_loc= getVIntegerIt();
        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Name:";
        std::copy(pos, name_it_loc, std::ostream_iterator<char>(std::cout));
        pos = name_it_loc;
        std::cout << std::endl;

        if(header_flag & 0x01) {
            std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Extra area:" << std::endl;
            std::cout << std::setw(EMPTY_SPACE_LEFT*2) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT-EMPTY_SPACE_LEFT) << "size: " << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header_extra_area_size) << std::endl;
            std::cout << std::setw(EMPTY_SPACE_LEFT*2) << " " << std::left << "\"" ;
            auto area_it_loc = pos + header_extra_area_size;
            std::copy_if(pos, area_it_loc, std::ostream_iterator<char>(std::cout), [](const char &byte) {
                return byte>31;
            });
            std::cout << "\"" << std::endl;
            pos = area_it_loc;
        }
        if(header_flag & 0x02) {
            std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Data area:" << std::endl;
            std::cout << std::setw(EMPTY_SPACE_LEFT*2) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT-EMPTY_SPACE_LEFT) << "size: " << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header_data_size) << std::endl;
            std::cout << std::setw(EMPTY_SPACE_LEFT*2) << " " << std::left << "\"" ;
            auto data_size_it_loc = pos + header_data_size;
            std::copy_if(pos, pos+5, std::ostream_iterator<char>(std::cout), [](const char &byte) {
                return byte>31;
            });
            std::cout << "\"" << std::endl;
            pos = data_size_it_loc;
        }
        /*std::copy_if(pos, pos+10, std::ostream_iterator<uint16_t>(std::cout<<std::showbase<<std::hex, " "), [](const char &byte) {
            return byte>31;
        });*/

        std::cout << std::endl;
        break;
    }
    case STATE_END_OF_ARCHIVE_HEADER:
            return false;
    }

    return true;

}

std::vector<char>::const_iterator InfoRAR5::getVIntegerIt()
{
    vint_t result=0;
    std::vector<char>::const_iterator it;
    for (uint shift=0; pos != data->end(); shift+=7)
    {
        uint8_t curByte = *pos;
        pos++;
        result+=uint64_t(curByte & 0x7f)<<shift;
        if ((curByte & 0x80)==0) {
            auto it = pos;
            std::advance(it, result);
            return it;
        }
    }
    throw ;
}

vint_t InfoRAR5::getVInteger() {
    vint_t result=0;
    for (uint shift=0; pos != data->end(); shift+=7)
    {
        uint8_t curByte = *pos;
        pos++;
        result+=uint64_t(curByte & 0x7f)<<shift;
        if ((curByte & 0x80)==0) {
            return result;
        }
    }
    throw ;
}

std::string InfoRAR5::getStrSizeOfData(vint_t dgt)
{
    std::string str;
    str = "\tData area size: " + std::to_string(dgt);
    return str;
}
