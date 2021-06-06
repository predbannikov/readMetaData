#ifndef INFORAR5_H
#define INFORAR5_H
#include "baserar.h"

typedef uint64_t vint_t;

class InfoRAR5 : public BaseRAR{
    vint_t sizeHeader;
    vint_t typeHeader;
    vint_t flagHeader;
    vint_t extraAreaSize;
    vint_t archiveFlags;
    vint_t volumeNumber;
public:
    static const char signature[8];

    InfoRAR5(std::vector<char> &data);
    ~InfoRAR5();


    bool readMainHead() override;
    bool setStateHeader() override;
    bool readNextBlock() override;

    size_t pos;
    vint_t getVInteger();
    std::string getStrSizeOfExtraArea(vint_t);
    std::string getStrSizeOfData(vint_t);
};


#endif  // INFORAR5_H
