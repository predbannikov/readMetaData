#ifndef RARMETADATA_H
#define RARMETADATA_H
#include <iostream>
#include <fstream>


#define BYTES_HEAD_SIGNATURE   6
#define HEAD_CRC    0x6152
typedef unsigned long long vint_t;

struct Marker {
    uint16_t *crc;
    uint16_t *flags;
    uint16_t *size;
    uint32_t *add_size;
    uint8_t *type;
    uint32_t *fullSize;
};

class RARMetaData {
    const char six_byte_signatur[BYTES_HEAD_SIGNATURE] {0x52, 0x61};
    std::string m_sFileName;
    std::ifstream file;
    char *data;
    unsigned int size;
    char byte;
    std::string rar_version;
    int cur_index = 0;
    int32_t size_header;
    int32_t header_type;
    Marker marker;
    bool add_size = false;
public:
    RARMetaData();
    bool open(std::string a_sFileName);
    bool getHeadCRC();
    bool getHeadType();
    bool getHeadFlags();
    unsigned int getSizeForRead();
    //**********************************************************************************
    void readMetaData();
    vint_t getVInteger();
};

#endif // RARMETADATA_H
