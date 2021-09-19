#include "extractor.h"
#include <stdexcept>
#include <iomanip>
#include <iostream>

Name::Name() {

}


void Name::extract(std::vector<char>::const_iterator &it, size_t len)
{
    if(len == 0)
        length = getVInteger(it);
    else
        length = len;
    begin = it;
    std::advance(it, length);
    end = it;
}



void Name::update()
{
    std::string str(length, ' ');
    std::copy(begin, end, str.begin());
    std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Name:" << str << std::endl;
    std::cout << std::setw(EMPTY_SPACE_LEFT*2) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT-EMPTY_SPACE_LEFT);
    if(str == "QO")
        std::cout << "Archive quick open data" << std::endl;
    else if(str == "CMT")
        std::cout << "Archive comment";
    else if(str == "ACL")
        std::cout << "NTFS file permissions";
    else if(str == "STM")
        std::cout << "NTFS alternate data stream";
    else if(str == "RR")
        std::cout << "Recovery record";
}

vint_t Extractor::getVInteger(std::vector<char>::const_iterator &pos) {
    try {
        vint_t result=0;
        std::vector<char>::const_iterator end = pos + 10;
        for (uint shift=0; pos != end; shift+=7)
        {
            uint8_t curByte = *pos;
            pos++; 										// прибавляем размер char
            result+=uint64_t(curByte & 0x7f)<<shift; 	// 0x7f = 0111 1111
            if ((curByte & 0x80)==0) { 					// проверка на старший бит "highest bit in every byte is the continuation flag. If highest bit is 0, this is the last byte in sequence. So first byte contains 7 least significant bits of integer and continuation flag"
                return result;
            }
        }
        throw std::runtime_error("vint_t > 10 bytes");
    }  catch (...) {
        throw std::runtime_error("the size vint exceeded bounds");
    }
}
