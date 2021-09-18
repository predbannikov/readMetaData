#ifndef INFORAR4X_H
#define INFORAR4X_H
#include "baserar.h"



class InfoRAR4x : public BaseRAR{
public:
    static const char signature[7];
    InfoRAR4x(std::vector<char> &data_):BaseRAR(data_){};

    //************* MAIN HEADER *************//
    bool setStateHeader() override;
//    bool readMainHead() override;
    bool readNextBlock() override;
    ~InfoRAR4x();

//    void readNextBlock() {


//    }
};

#endif  // INFORAR4x_H
