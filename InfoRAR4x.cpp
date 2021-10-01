#include "InfoRAR4x.h"

const char InfoRAR4x::signature[7]  {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07, 0x0};


InfoRAR4x::InfoRAR4x(std::fstream &file_):BaseRAR(file_){

}


bool InfoRAR4x::readNextBlock(std::fstream *to_file)
{

    return false;
}

InfoRAR4x::~InfoRAR4x(){}
