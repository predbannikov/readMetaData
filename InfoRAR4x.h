#ifndef INFORAR4X_H
#define INFORAR4X_H
#include "baserar.h"



class InfoRAR4x : public BaseRAR{
public:
    static const char signature[7];
    InfoRAR4x(std::fstream &file_);

    //************* MAIN HEADER *************//
    //bool readMainHead() override;
    bool readNextBlock() override;
    ~InfoRAR4x();

};

#endif
