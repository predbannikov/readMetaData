#ifndef INFORAR5_H
#define INFORAR5_H
#include "baserar.h"
#include <list>
#include <vector>
#define LENGTH_SIGNATURE_FOR_5_X_VERSION_RAR    8
#define MAX_SHOW_NUMBER_DATA_HEADER 			0x3F

struct TypeData
{
    std::streampos begin;
    std::streampos end;
    size_t length;
    std::vector<char> buff;
};
struct TypeVInt
{
    std::streampos begin;
    std::streampos end;
    uint64_t number;
};
struct TypeInt32
{
    std::streampos begin;
    std::streampos end;
    uint32_t number;
};

struct QuickOpenHeader{
    TypeInt32 crc32;
    TypeVInt size;
    TypeVInt flags;
    TypeVInt offset;
    TypeVInt data_size;
    TypeData data;
};

class Header {
public:
    Header();
    void display();
    struct GeneralExtraArea {
        TypeVInt size;
        TypeVInt type;
        TypeData data;
        struct LocatorRecord { //только у main header
            TypeVInt flags;
            TypeVInt quick_open_offset;
            TypeVInt recovery_offset;
        } locator;
        struct FileTime {
            TypeVInt flag;

        } time;
    } extra;

    struct ServiceDataArea {
        TypeVInt size;
        TypeVInt type;
        TypeData data;
        std::list<QuickOpenHeader *> sub_headers;
    } service_data_area;

    TypeVInt size_data;
    TypeVInt size_header;    //Size of header data starting from Header type field and up to and including the optional extra area. This field must not be longer than 3 bytes in current implementation, resulting in 2 MB maximum header size
    TypeVInt type;
    TypeVInt flags_common;
    TypeVInt size_extra_area;
    TypeVInt flags_specific;
    TypeVInt volume_number;
    TypeVInt unpack_size;
    TypeVInt attributes;
    TypeInt32 unpacked_crc;
    TypeVInt compres_info;
    TypeVInt host_os_creator;
    TypeVInt length_name;
    TypeData name;
    TypeData package_data;
    TypeVInt end_of_archive_flags;

    STATE_HEADER state = STATE_MARKER_HEADER;
    /* If flag 0x0008 is set, unpacked size field is still present,
     * but must be ignored and extraction must be performed until reaching the end of compression stream.
     * This flag can be set if actual file size is larger than reported by OS or if file size is unknown
     * such as for all volumes except last when archiving from stdin to multivolume archive.
     */
    bool ignorUnpackSize = false;
};






class InfoRAR5 : public BaseRAR{
    // выставляет состоянение переменной STATE_HEADER state
/* Type of archive header. Possible values are:
  1   Main archive header.
  2   File header.
  3   Service header.
  4   Archive encryption header.
  5   End of archive header.*/
    bool setStateHeader() override; //override чтоб была гарантия, что произойдет ошибка, если не виртуальная ф-ия

    void parseExtraArea();
    void parseDataArea();
    void getFileModifTime();
    void getExtraAreaSize();
    void getSizeData();
    void getUnpackSize();
    void getCRCUnpackData();
    void printHostCreator();
    void printFlagComm();
    void printFlagSpec();
    void printCRCData();
    void printCompresMethod();
    void printName();
    void printSmthInfo();
    void extractVInteger(TypeVInt &vint_var);
    void extractData(TypeData &data_var, size_t length);
    void extractInt32(TypeInt32 &crc_var);
    uint32_t extract32Int_();
    uint64_t extract64Int_();
    std::list<Header*> headers;
    Header *header = nullptr;

    unsigned int CRC32_function(unsigned char *buf, unsigned long len);
public:
    static const char signature[LENGTH_SIGNATURE_FOR_5_X_VERSION_RAR];

    InfoRAR5(std::fstream &file);
    ~InfoRAR5();

// читать следующий блок одного из 5 Types of archive header
    bool readNextBlock() override;
};


#endif
