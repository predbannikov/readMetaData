#ifndef INFORAR5_H
#define INFORAR5_H
#include "baserar.h"

typedef uint64_t vint_t;
typedef unsigned int uint;

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


    bool setStateHeader() override;
    bool readNextBlock() override;

    std::vector<char>::const_iterator getVIntegerIt();
    vint_t getVInteger();
    std::string getStrSizeOfData(vint_t);
};


#endif  // INFORAR5_H
