#ifndef INFORAR5_H
#define INFORAR5_H
#include "baserar.h"
#include "extractor.h"
#include <map>

#define LENGTH_SIGNATURE_FOR_5_X_VERSION_RAR    8
#define MAX_SHOW_NUMBER_DATA_HEADER 			0x3F


//struct Name {
//    std::vector<char>::const_iterator it;
//    std::string data;
//    vint_t length;
//};

struct ExtraArea {
    std::vector<char>::const_iterator it;
    vint_t size;
};

class Header {
public:
    Header();
    void update();
    struct ExtraData {
        vint_t offset;
        vint_t size_data;
    };
    vint_t size_data;
    vint_t size_header;		//Size of header data starting from Header type field and up to and including the optional extra area. This field must not be longer than 3 bytes in current implementation, resulting in 2 MB maximum header size
    vint_t type;
    vint_t flags_common;
    vint_t size_extra_area;
    vint_t flags_specific;
    vint_t volume_number;
    vint_t unpack_size;;
    vint_t attributes;
    vint_t offset;
    uint32_t crc_data;
    vint_t compres_info;
    vint_t host_os_creator;

//    vint_t length_name;
//    std::string name;
    Name name;


    STATE_HEADER state = STATE_MARKER_HEADER;
    /* If flag 0x0008 is set, unpacked size field is still present,
     * but must be ignored and extraction must be performed until reaching the end of compression stream.
     * This flag can be set if actual file size is larger than reported by OS or if file size is unknown
     * such as for all volumes except last when archiving from stdin to multivolume archive.
     */
    bool ignorUnpackSize = false;
};

class InfoRAR5 : public BaseRAR{
    void parseExtraArea();
    void getGetCRCDate();
    void getFileModifTime();
    void getExtraAreaSize();
    void getSizeData();
    void getUnpackSize();
    void getName();
    void printHostCreator();
    void printFlagComm();
    void printFlagSpec();
    void printCRCData();
    void printCompresMethod();
    void printDataArea();
    void printName();
    std::map<uint32_t, Header*> map;
    Header *header = nullptr;
public:
    static const char signature[LENGTH_SIGNATURE_FOR_5_X_VERSION_RAR];

    InfoRAR5(std::vector<char> &data);
    ~InfoRAR5();

    // выставляет состоянение переменной STATE_HEADER state
/* Type of archive header. Possible values are:
  1   Main archive header.
  2   File header.
  3   Service header.
  4   Archive encryption header.
  5   End of archive header.*/
    bool setStateHeader() override; //override чтоб была гарантия, что произойдет ошибка, если не виртуальная ф-ия


// читать следующий блок одного из 5 Types of archive header
    bool readNextBlock() override;

    /* vint из спецификации
Variable length integer.
Can include one or more bytes, where lower 7 bits of every byte contain integer data and highest bit in every byte is the continuation flag. If highest bit is 0, this is the last byte in sequence. So first byte contains 7 least significant bits of integer and continuation flag.
Second byte, if present, contains next 7 bits and so on.*/
    vint_t getVInteger(); 	// эта функция выдает размер

};


#endif
