#include "rarmetadata.h"

RARMetaData::RARMetaData()
{
    rar_version = "nan";

}

bool RARMetaData::open(std::string a_sFileName) {
    m_sFileName = a_sFileName;
    file.open(a_sFileName, std::ios::in | std::ios::out | std::ios::binary);
    if(!file.is_open()) {
        std::cout << "error open file " << a_sFileName << std::endl;
        return false;
    }
    std::streampos begin = file.tellg();
    file.seekg(0, std::ios::end);
    std::streampos end = file.tellg();
    size = end-begin;
    file.seekg(std::ios::beg);
    data = new char[size];
    file.read(data, size);

    std::cout << "\nsize = " << size << std::endl;
    file.close();
    return true;
}

bool RARMetaData::getHeadCRC() {

    marker.crc = reinterpret_cast<uint16_t*>(&data[0]);
    //        marker.crc <<= 8;
    //        marker.crc |= data[1];

    if(*marker.crc != HEAD_CRC) {
        std::cout << "this file not contains CRC HEAD signature " << std::endl;
        return false;
    }
    std::cout << "signature CRC found " << std::endl;

    return true;
}

bool RARMetaData::getHeadType() {
    marker.type = reinterpret_cast<uint8_t*>(&data[2]);
    //            rar_version = "RAR 5.0";
    //        else

    //        std::cout << rar_version << std::endl;
    return true;
}

bool RARMetaData::getHeadFlags() {
    marker.flags = reinterpret_cast<uint16_t*>(&data[3]);
    //        marker.flags <<=8;
    //        marker.flags |= data[4];
    if(*marker.flags & 0x8000) {
        add_size = true;
    }
    return true;
}

unsigned int RARMetaData::getSizeForRead() {
    if(add_size) {
        //            marker.fullSize = data[7];
        //            marker.fullSize <<=8;
        //            marker.fullSize |= data[8];
        //            marker.fullSize <<= 8;
        //            marker.fullSize |= data[9];
        //            marker.fullSize <<= 8;
        //            marker.fullSize |= data[10];
        std::cout << "full_size = " << marker.fullSize << std::endl;
    }
    return 0;
}

void RARMetaData::readMetaData() {

    for(int i = 0; i < size; i++) {
        std::cout << std::hex << (0xFF&data[i]) << " ";
    }
    std::cout << std::endl;
    int save_int = 0;
    for(int i = 0; i < size; i++) {
        std::cout << std::hex << data[i] ;
        if(data[i] == 't')
            if(data[i+1] == 'e')
                save_int = i;
    }
    std::cout << std::dec << std::endl << "save" << save_int << std::endl;
    std::cout << std::endl;




    if(!getHeadCRC()) {
        return;
    }
    if(!getHeadType()) {
        return;
    }
    if(!getHeadFlags()) {
        return;
    }
    getSizeForRead();



    //        size_header = getVInteger();
    //        header_type = getVInteger();
}

vint_t RARMetaData::getVInteger() {
    int flag = 0;
    vint_t result = 0;
    for(int i = 0; i < 10; i++) {
        char byte = data[cur_index];
        result |= 0xFF & byte;
        cur_index++;
        if((byte & 0x80) == 0) {
            break;
        }
        result &= 0xFFFFFFFFFFFFFF7F;
        result <<= 7;
    }
    std::cout << std::hex << (0xFF&data[cur_index]) << " vint = " << result << std::endl;
    return result;
}
