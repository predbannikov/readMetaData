#ifndef INFORAR5_H
#define INFORAR5_H
#include "baserar.h"
#include <list>
#include <vector>
#include <bitset>
#include <functional>
#include <algorithm>
#include <numeric>
#include <thread>
#define LENGTH_SIGNATURE_FOR_5_X_VERSION_RAR    8
#define MAX_SHOW_NUMBER_DATA_HEADER 			0x3F

struct TypePos {
    std::streampos beg;
    std::streampos end;
};

struct TypeData : TypePos
{
    size_t length;
    std::vector<char> buff;
};

struct TypeVInt : TypePos
{
    uint64_t number;
};

struct TypeInt32 : TypePos
{
    uint32_t number;
};

struct TypeInt64 : TypeInt32
{
    uint64_t number;
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
    int index;
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
            TypeInt64 mtime;
            TypeInt64 ctime;
            TypeInt64 atime;
            std::string smtime;
            std::string sctime;
            std::string satime;
        } time;
    } extra;

    struct ServiceDataArea {
        TypeVInt size;
        TypeVInt type;
        TypeData data;
        std::list<QuickOpenHeader *> sub_headers;
    } service_data_area;

    TypePos pos;
    TypeInt32 crc;
    TypeVInt size_data;
    TypeVInt size_header;    //Size of header data starting from Header type field and up to and including the optional extra area. This field must not be longer than 3 bytes in current implementation, resulting in 2 MB maximum header size
    TypeVInt type;
    TypeVInt flags_common;
    TypeVInt size_extra_area;
    TypeVInt flags_specific;
    TypeVInt volume_number;
    TypeVInt unpack_size;
    TypeVInt attributes;
    TypeInt32 mtime;
    TypeInt32 unpacked_crc;
    TypeVInt compres_info;
    TypeVInt host_os_creator;
    TypeVInt length_name;
    TypeData name;
    TypeData package_data;
    TypeVInt end_of_archive_flags;

    std::string sname;

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
    void getQuickOpenHeader(QuickOpenHeader *q, int index);
    std::string getTime(TypeInt64 &var);

    void extractVInteger(TypeVInt &vint_var);
    void extractData(TypeData &data_var, size_t length);
    void extractInt32(TypeInt32 &var);
    void extractInt64(TypeInt64 &var);

    void changeMainHeader();

    std::vector<char> packVInt(uint64_t vint);

    void printFlagComm();
    void printFlagSpec();
    void printSmthInfo();
    std::list<Header*> headers;
    Header *header = nullptr;
    Header *main_header = nullptr;
    Header *service_header = nullptr;
    std::streampos service_pos_to_file;
    std::string test;
    std::string hexStrFromDec(uint32_t d);
    std::string fillStrCol(std::string s, size_t len, char ch = ' ', char allign = 'c'); // impl c,l
    void printHeader(uint64_t type);
    void printLine(std::string first, uint64_t second, char format = 'd');
    void printLine(std::string first, std::string second, char ch = ' ');
    void printLine(std::string first, char ch = ' ');
    void printLine(TypeData &data);
    void printName(std::string &str, Keyboard &keyboard);

    void deleteHeader(int index) override;

//    std::streampos m_pos;
//    std::streampos m_pos_begin;

    void expandVInt(std::vector<char> &v, int size_vint);
    void debug_write(std::streampos beg, char *buff, int size);
    Header *getHeaderOfIndex(int index);
    uint32_t getCRC(std::streampos begin, std::streampos end);
    unsigned int CRC32_function(unsigned char *buf, unsigned long len);
    static const char* digits;

    void writeVIntOffset(const TypeVInt &vint, int num_vint);
    void writeInt32Offset(std::streampos target_pos, uint32_t num);
    int mode = 0;
    int index_to_delete = -1;
public:
    uint32_t parallel_crc(std::streampos beg, std::streampos end,uint32_t crc);
    void calcCRC(std::streampos beg, std::streampos end, uint32_t &result);

    std::fstream *to_file;
    static const char signature[LENGTH_SIGNATURE_FOR_5_X_VERSION_RAR];

    InfoRAR5(std::fstream &file);
    ~InfoRAR5();
    void printInfo(size_t index, Keyboard &keyboard) override;

// читать следующий блок одного из 5 Types of archive header
    bool append_to_list = true;
    bool readNextBlock() override;
    size_t getSizeHeaders() override;
    void redirectToFile(TypePos &var);


    bool crc_calc(std::streampos beg, std::streampos end, uint32_t real_CRC);
};


#endif
