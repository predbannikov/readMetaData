#include "extractor.h"
#include <stdexcept>

Name::Name() {}

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
