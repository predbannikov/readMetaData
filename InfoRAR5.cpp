#include "InfoRAR5.h"
#include <ctime>
#define EMPTY_SPACE_LEFT                5           ////просто для выравнивания при печати
#define EMPTY_SPACE_AFTER_LEFT          45
#define EMPTY_SPACE_RIGHT_NUMBER        7
const char InfoRAR5::signature[LENGTH_SIGNATURE_FOR_5_X_VERSION_RAR] {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07, 0x1, 0x0};
const char* InfoRAR5::digits = "0123456789ABCDEF";

InfoRAR5::InfoRAR5(std::fstream &file) : BaseRAR(file){ // инициализируем BaseRAR

}

InfoRAR5::~InfoRAR5()
{
    auto it_tmp = headers.end();
    --it_tmp;
    headers.erase(it_tmp);
    auto l = headers.back()->service_data_area.sub_headers;
    for(auto it = l.begin(); it != l.end(); it++)
        delete *it;

    for(auto it = headers.begin(); it != headers.end(); it++)
        delete *it;
}


//void InfoRAR5::printCRCData()
//{
//    if(header->flags_specific.number & 0x04)
//        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "CRC32 of unpacked file or service data:"
//            << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::hex << header->unpacked_crc.number << std::dec << std::endl;
//}

//void InfoRAR5::printCompresMethod()
//{
//    std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Compression information:" << std::endl;
//    std::cout << std::setw(EMPTY_SPACE_LEFT*2) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT-EMPTY_SPACE_LEFT) << "method: "
//            << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header->compres_info.number & 0x0380) << std::endl;
//}

void InfoRAR5::parseDataArea()
{
    auto lambda = [](const char &byte) {
        return byte>31; // всё кроме непечатаемых символов (из ASCII до 31)
    };
    if(header->flags_common.number & 0x02) {
        switch (header->state) {
        case STATE_FILE_HEADER:
//            std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Data area:" << std::endl;
//            std::cout << std::setw(EMPTY_SPACE_LEFT*2) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT-EMPTY_SPACE_LEFT) << "size: "
//                << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header->size_data.number) << std::endl;
//            std::cout << std::setw(EMPTY_SPACE_LEFT*2) << " " << std::left << "\"" ;
            extractData(header->package_data, header->size_data.number);
//            if(MAX_SHOW_NUMBER_DATA_HEADER < header->size_data.number) {
//                std::copy_if(header->package_data.buff.begin(), header->package_data.buff.begin()+MAX_SHOW_NUMBER_DATA_HEADER, std::ostream_iterator<char>(std::cout), lambda);
//                std::cout << "...\"" << std::endl;
//            } else {
//                std::copy_if(header->package_data.buff.begin(), header->package_data.buff.end(), std::ostream_iterator<char>(std::cout), lambda);
//                std::cout << "\"" << std::endl;
//            }
            break;
        case STATE_SERVICE_HEADER:
            std::streampos begin_pos = file->tellg();
            while(header->size_data.number != file->tellg()-begin_pos) {
                QuickOpenHeader *a = new QuickOpenHeader;
                header->service_data_area.sub_headers.push_back(a);
                extractInt32(a->crc32);
//                std::cout << a->crc32.number << " crc32" << std::endl;
                extractVInteger(a->size);
//                std::cout << a->size.number << " struct size" << std::endl;
                extractVInteger(a->flags);
//                std::cout << a->flags.number << " flags" << std::endl;
                extractVInteger(a->offset);
//                std::cout << a->offset.number << " offset" << std::endl;
                extractVInteger(a->data_size);
//                std::cout << a->data_size.number << " size archive data" << std::endl;
                std::streampos next = file->tellg();
                next += a->data_size.number;
                file->seekg(next);
                if(file->eof()) {
                    std::cout << std::endl << "eof" << std::endl;
                    throw std::runtime_error("unexpected eof");
                }
            }
            break;
        }
    }
}

void InfoRAR5::extractVInteger(TypeVInt &vint_var) {
    uint64_t result=0;
    std::streampos pos= file->tellg();
    vint_var.begin=pos;
    for (uint8_t shift=0; pos != end; shift+=7)
    {
        if(file->eof())
            break;
        char curByte;
        file->read(&curByte, 1);
        result+=uint64_t(curByte & 0x7f)<<shift; 	// 0x7f = 0111 1111
        if ((curByte & 0x80)==0) { 					// проверка на старший бит "highest bit in every byte is the continuation flag. If highest bit is 0, this is the last byte in sequence. So first byte contains 7 least significant bits of integer and continuation flag"
            vint_var.number=result;
            vint_var.end=file->tellg();
            return;
        }
    }
    throw std::runtime_error("the size vint exceeded bounds");
}

void InfoRAR5::extractData(TypeData &data_var, size_t length)
{
    data_var.begin = file->tellg();
    data_var.buff.resize(length);
    file->read(data_var.buff.data(), length);
    data_var.end = file->tellg();
    data_var.length = length;
}

void InfoRAR5::extractInt32(TypeInt32 &var)
{
    var.begin=file->tellg();
    char buff[4];
    //char tmp;
    for (int i = 0; i < 4; i++)
    {
        if(file->eof())
            throw std::runtime_error("extractInt32 error");
        file->read(&buff[i], 1);
        // file->read(&tmp, 1);
        // tmp2 |= (static_cast<uint32_t>(0xFF&tmp)) << 8*i;

    }
    var.number = *reinterpret_cast<const uint32_t*>(buff);
    var.end = file->tellg();
}

uint32_t InfoRAR5::extract32Int_()
{
    char buff[4];
    for (int i=0; i<4; i++)
    {
        if(file->eof())
            throw std::runtime_error("extract32Int_ error");
        file->read(&buff[i], 1);

    }
    return *reinterpret_cast<const uint32_t*>(buff);
}

uint64_t InfoRAR5::extract64Int_()
{
    char buff[8];
    for (int i=0; i<8; i++)
    {
        if(file->eof())
            throw std::runtime_error("extract64Int error");
        file->read(&buff[i], 1);
    }
    return *reinterpret_cast<const uint64_t*>(buff);
}


unsigned int InfoRAR5::CRC32_function(unsigned char *buf, unsigned long len) // для будущего удаления
{
    unsigned long crc_table[256];
    unsigned long crc;
    for (int i = 0; i < 256; i++)
    {
        crc = i;
        for (int j = 0; j < 8; j++)
            crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;
        crc_table[i] = crc;
    };
    crc = 0xFFFFFFFFUL;
    while (len--)
        crc = crc_table[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);
    return crc ^ 0xFFFFFFFFUL;
}

#define WINDOWS_TICK 10000000
#define SEC_TO_UNIX_EPOCH 11644473600LL
unsigned WindowsTickToUnixSeconds(long long windowsTicks)
{
     return (unsigned)(windowsTicks / WINDOWS_TICK - SEC_TO_UNIX_EPOCH);
}

void InfoRAR5::parseExtraArea()
{
    if(header->flags_common.number & 0x01) {
//        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Extra area:" << std::endl;
//        std::cout << std::setw(EMPTY_SPACE_LEFT*2) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT-EMPTY_SPACE_LEFT) << "size: "
//            << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header->size_extra_area.number) << std::endl;

        extractVInteger(header->extra.size);
//        std::cout << "size of record data starting from TYPE " << header->extra.size.number << std::endl;
        extractVInteger(header->extra.type);
//        std::cout << "type_extra " << header->extra.type.number << std::endl;
        switch (header->state) {
        case STATE_MAIN_HEADER: {
//            if(header->extra.type.number == 0x01)
//                std::cout << "locator record: type " << std::endl;
            extractVInteger(header->extra.locator.flags);
            if( header->extra.locator.flags.number & 0x01) {
                extractVInteger(header->extra.locator.quick_open_offset);
//                std::cout << "Quick open record offset is present. " << header->extra.locator.quick_open_offset.number << std::endl;
            }
            if( header->extra.locator.flags.number & 0x02) {
                extractVInteger(header->extra.locator.recovery_offset);
//                std::cout << "Recovery record offset is present. " << header->extra.locator.recovery_offset.number << std::endl;
            }
            break;
        }
        case STATE_FILE_HEADER:
        case STATE_SERVICE_HEADER:
            switch (header->extra.type.number) {
            case 0x01:
                break;
            case 0x02:
                break;
            case 0x03: {
                extractVInteger(header->extra.time.flag);
                uint64_t mtime;		// time modification
                uint64_t ctime;		// time creation
                uint64_t atime;		// last access
                if(header->extra.time.flag.number & 0x0002) {
                    mtime = header->extra.time.flag.number & 0x0010 ? extract32Int_() : extract64Int_();
                    time_t t = static_cast<time_t>(WindowsTickToUnixSeconds(mtime));
                    header->extra.time.mtime = std::asctime(std::localtime(&t));
                } else if(header->extra.time.flag.number & 0x0004) {
                    ctime = header->extra.time.flag.number & 0x0010 ? extract32Int_() : extract64Int_();
                    time_t t = static_cast<time_t>(WindowsTickToUnixSeconds(mtime));
                    header->extra.time.ctime = std::asctime(std::localtime(&t));
                } else if(header->extra.time.flag.number & 0x0008) {
                    atime = header->extra.time.flag.number & 0x0010 ? extract32Int_() : extract64Int_();
                    time_t t = static_cast<time_t>(WindowsTickToUnixSeconds(mtime));
                    header->extra.time.atime = std::asctime(std::localtime(&t));
                }
                break;
            }
            case 0x04:
                break;
            case 0x05:
                break;
            case 0x06:
                break;
            case 0x07:
                break;
            }
            break;
        }
    }
}

void InfoRAR5::getCRCUnpackData()
{
    if(header->flags_specific.number & 0x04) {
        extractInt32(header->unpacked_crc);
    }
}


void InfoRAR5::getFileModifTime()
{
    if(header->flags_specific.number & 0x02)  {
        uint32_t header_mtime = extract32Int_();

//        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Modification time in Unix time format:"
//            << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header_mtime) << std::endl;
    }
}

void InfoRAR5::getExtraAreaSize()
{
    if(header->flags_common.number & 0x01)
        extractVInteger(header->size_extra_area);
}

void InfoRAR5::getSizeData()
{
    if(header->flags_common.number & 0x02)
        extractVInteger(header->size_data);
}

void InfoRAR5::getUnpackSize()
{
    if(header->flags_specific.number & 0x08)
        header->ignorUnpackSize = true;
    else
        header->ignorUnpackSize = false;;
    extractVInteger(header->unpack_size);
}

void InfoRAR5::printFlagComm()
{
    if(header->flags_common.number & 0x01)
        std::cout << "Extra area is present in the end of header." << std::endl;
    else if(header->flags_common.number & 0x02)
        std::cout << "Data area is present in the end of header." << std::endl;
    else if(header->flags_common.number & 0x04)
        std::cout << "Blocks with unknown type and this flag must be skipped when updating an archive." << std::endl;
    else if(header->flags_common.number & 0x08)
        std::cout << "Data area is continuing from previous volume." << std::endl;
    else if(header->flags_common.number & 0x10)
        std::cout << "Data area is continuing in next volume." << std::endl;
    else if(header->flags_common.number & 0x20)
        std::cout << "Block depends on preceding file block." << std::endl;
    else if(header->flags_common.number & 0x40)
        std::cout << "Preserve a child block if host block is modified." << std::endl;
}

bool InfoRAR5::setStateHeader()
{
    extractVInteger(header->type);
    switch (header->type.number) {
    case 1:
        header->state = STATE_MAIN_HEADER;
//        std::cout << "BLOCK MAIN HEAD size = " << header->size_header.number << std::endl;
        return true;
    case 2:
        header->state = STATE_FILE_HEADER;
//        std::cout << "BLOCK FILE HEAD size = " << header->size_header.number << std::endl;
        return true;
    case 3:
        header->state = STATE_SERVICE_HEADER;
//        std::cout << "BLOCK SERVICE HEAD size = " << header->size_header.number << std::endl;
        return true;
    case 4:
//        std::cout << "this is encryption header" << std::endl;
        return false;
    case 5:
//        std::cout << "BLOCK END_OF_ARCHIVE HEAD size = " << header->size_header.number << std::endl;
        header->state = STATE_END_OF_ARCHIVE;
        return true;
    default:							// ошибка, случилось непредвиденное
        throw std::runtime_error("this value of typeHeder not match any type");
    }
    return false;
}

void InfoRAR5::printSmthInfo()
{
    std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Unpacked file or service data size:" << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header->unpack_size.number) << std::endl;
    std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Operating system specific file attributes:" << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header->attributes.number) << std::endl;
    std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Compression information:" << std::endl;
    std::cout << std::setw(EMPTY_SPACE_LEFT*2) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT-EMPTY_SPACE_LEFT) << "method: " << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header->compres_info.number & 0x0380) << std::endl;
}

bool InfoRAR5::readNextBlock() {
    static int index = -1;
    try {
        index++;

        header = new Header;
        headers.push_back(header);//вставить в конец
        extractInt32(header->crc);
        extractVInteger(header->size_header);

        if(!setStateHeader()) 			// если не один из типов, то есть попалось инородное
            return false;

        switch (header->state) {
        case STATE_MARKER_HEADER:
            break;
        case STATE_MAIN_HEADER:{
            auto begin_header_pos = file->tellg();
            extractVInteger(header->flags_common);
            getExtraAreaSize();
            extractVInteger(header->flags_specific);
            printFlagComm();
            parseExtraArea();
            if(header->flags_specific.number & 0x02) {
                extractVInteger(header->volume_number);
//                std::cout << "number volume " << header->volume_number.number << std::endl;
            }
//            std::cout << std::endl;
            break;
        }
        case STATE_FILE_HEADER:
        case STATE_SERVICE_HEADER:
            extractVInteger(header->flags_common);
            getExtraAreaSize();
            getSizeData();
            extractVInteger(header->flags_specific);
            getUnpackSize();
            extractVInteger(header->attributes);
            getFileModifTime();
            getCRCUnpackData();
            extractVInteger(header->compres_info);
            extractVInteger(header->host_os_creator);
            extractVInteger(header->length_name);
            extractData(header->name, header->length_name.number);
            // ------------ area print specific header data --------------------
//            printFlagSpec();
//            printCRCData();
//            printCompresMethod();
//            printFlagComm();
//            printSmthInfo();
//            printName();
            parseExtraArea();
            parseDataArea();
//            printInfo(index);
//            std::cout << std::endl;
            break;

        case STATE_END_OF_ARCHIVE:
            extractVInteger(header->flags_common);
            extractVInteger(header->end_of_archive_flags);
//            printInfo(index);
            if(header->end_of_archive_flags.number == 0x01)
                std::cout << "it is not last volume in the set"<< std::endl;
            else
                std::cout <<header->end_of_archive_flags.number<< "  that's all" << std::endl;
            return false;
        }
    } catch (const std::exception &e) {
        std::cerr << "ERROR: [" << errno << "] " << e.what() << std::endl;		// ... какая-то ошибка
        return false;
    }
    return true;
}

size_t InfoRAR5::getCountHeaders()
{
    return headers.size();
}


Header::Header() {

}

///*Flags specific for file header type:
//            0x0001   Directory file system object (file header only).
//            0x0002   Time field in Unix format is present.
//            0x0004   CRC32 field is present.
//            0x0008   Unpacked size is unknown.*/
//void InfoRAR5::printFlagSpec()
//{
//    if(header->flags_specific.number & 0x1)
//        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Directory file system object." << std::endl;
//    if(header->flags_specific.number & 0x2)
//        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Time field in Unix format is present." << std::endl;
//    if(header->flags_specific.number & 0x4)
//        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "CRC32 field is present. " << std::endl;
//    if(header->flags_specific.number & 0x8)
//        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Unpacked size is unknown. " << std::endl;
//}

#define FIRST_COLUMN_WIDTH	16
#define SECOND_COLUMN_WIDTH 24
#define THIRD_COLUMN_WIDTH	32

std::string InfoRAR5::hexStrFromDec(uint32_t d)
{
    std::string s;
    uint8_t *bt = reinterpret_cast<uint8_t*>(&d);
    for(int i = 3; i >= 0; i--) {
        s.push_back(digits[*(bt+i) >> 4]);
        s.push_back(digits[*(bt+i) & 0x0F]);
    }
    return s;
}

std::string InfoRAR5::fillStrCol(std::string s, size_t len, char ch, char allign)
{
    if(s.length() <= len) {
        if(allign == 'c') {
            int d = len - s.length();
            int m = d % 2;
            int part = d / 2;
            s.insert(s.begin(), part, ch);
            s.resize(part + s.length() + m, ch);
        } else if(allign == 'l'){
            s.resize(len, ch);
        }
    } else {
        s.resize(len);
    }
    return s;
}

void InfoRAR5::printLine(std::string first, uint64_t second, char format)
{
    if(second == 0)
        return;
    if(format == 'h')
        std::cout << "|" << fillStrCol(first, FIRST_COLUMN_WIDTH) << "|" << fillStrCol(hexStrFromDec(second), SECOND_COLUMN_WIDTH) << "|" << std::endl;
    else
        std::cout << "|" << fillStrCol(first, FIRST_COLUMN_WIDTH) << "|" << fillStrCol(std::to_string(second), SECOND_COLUMN_WIDTH) << "|" << std::endl;
    std::cout << "|" << fillStrCol("-", FIRST_COLUMN_WIDTH + SECOND_COLUMN_WIDTH + 1, '-') << "|" << std::endl;
}

void InfoRAR5::printLine(std::string first, std::string second, char ch)
{
    std::cout <<  "|" << fillStrCol(first, FIRST_COLUMN_WIDTH, ch) << "|" << fillStrCol(second, SECOND_COLUMN_WIDTH, ch) << "|" << std::endl;
    std::cout << "|" << fillStrCol("-", FIRST_COLUMN_WIDTH + SECOND_COLUMN_WIDTH + 1, '-') << "|" << std::endl;
}

void InfoRAR5::printLine(std::string first, char ch)
{
    std::cout <<  "|" << fillStrCol(first, FIRST_COLUMN_WIDTH + SECOND_COLUMN_WIDTH + 1, ch) << "|" << std::endl;
    std::cout << "|" << fillStrCol("-", FIRST_COLUMN_WIDTH + SECOND_COLUMN_WIDTH + 1, '-') << "|" << std::endl;
}

void InfoRAR5::printLine(TypeData &data)
{
    if(data.begin == data.end)
        return;
    auto lambda = [](const char &byte) {
        return byte>31; // всё кроме непечатаемых символов (из ASCII до 31)
    };
    std::streampos save_pos = file->tellg();
    size_t size_buff;
    if(MAX_SHOW_NUMBER_DATA_HEADER < data.length)
        size_buff = MAX_SHOW_NUMBER_DATA_HEADER;
    else
        size_buff = data.length;
    file->seekg(data.begin);
    char ch;
    std::string str;
    for(size_t i = 0; i < size_buff; i++) {
        file->read(&ch, 1);
        str.push_back(digits[(0xFF&ch) >> 4]);
        str.push_back(digits[ch & 0x0F]);
        str.push_back(' ');
    }
    size_t width = FIRST_COLUMN_WIDTH + SECOND_COLUMN_WIDTH + 1;
    std::cout << "|" << fillStrCol("DATA", width) << std::endl;
    while(width < str.length()) {
        std::cout << "|" << str.substr(0, width) << "|" << std::endl;
        str.erase(0, width);
    }
    std::cout << "|" << fillStrCol(str, width);
    std::cout << "|" << std::endl << "|" << fillStrCol("-", FIRST_COLUMN_WIDTH + SECOND_COLUMN_WIDTH + 1, '-') << "|" << std::endl;
    file->seekg(save_pos);
}

void InfoRAR5::printName(std::string &str, Keyboard &keyboard)
{
//    std::string str(data.buff.begin(), data.buff.end());    int y = 0xFFFF & keyboard.getCurPosCursor();

//    std::cout << fillStrCol(str, THIRD_COLUMN_WIDTH, ' ', 'l');
    int width_term, heigth_term;
    keyboard.get_terminal_size(width_term, heigth_term);
    for(size_t i = 0; i < str.length(); i+=2) {
        std::cout << str[i] << str[i+1];
        int pos = keyboard.getCurPosCursor();
        int x = 0xFFFF & pos;
        int y = pos >> 16;
        if(x == width_term-1)
            break;
    }
//    std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Name:" << str << std::endl;
//    if(header->state == STATE_SERVICE_HEADER) {
//        std::cout << std::setw(EMPTY_SPACE_LEFT*2) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT-EMPTY_SPACE_LEFT);
//        if(str == "QO")
//            std::cout << "Archive quick open data" << std::endl;
//        else if(str == "CMT")
//            std::cout << "Archive comment";
//        else if(str == "ACL")
//            std::cout << "NTFS file permissions";
//        else if(str == "STM")
//            std::cout << "NTFS alternate data stream";
//        else if(str == "RR")
//            std::cout << "Recovery record";
//    }
}

void InfoRAR5::printHeader(uint64_t type)
{
    std::cout << "+" << fillStrCol("-", FIRST_COLUMN_WIDTH + SECOND_COLUMN_WIDTH + 1, '-') << "+" << std::endl;
    std::cout << "|";
    switch (type) {
    std::cout << "MAIN HEADER" << std::endl;
    case 2:
        std::cout << fillStrCol("FILE HEADER", FIRST_COLUMN_WIDTH + SECOND_COLUMN_WIDTH + 1);
        break;
    case 3:
        std::cout << fillStrCol("SERVICE HEADER", FIRST_COLUMN_WIDTH + SECOND_COLUMN_WIDTH + 1);
        break;
    case 4:
        std::cout << fillStrCol("this is encryption header", FIRST_COLUMN_WIDTH + SECOND_COLUMN_WIDTH + 1);
        break;
    case 5:
        std::cout << fillStrCol("END_OF_ARCHIVE HEADER", FIRST_COLUMN_WIDTH + SECOND_COLUMN_WIDTH + 1);
        break;
    }
    std::cout << "|" << std::endl << "|" << fillStrCol("-", FIRST_COLUMN_WIDTH + SECOND_COLUMN_WIDTH + 1, '-') << "|" << std::endl;
}



void InfoRAR5::printInfo(size_t index, Keyboard &keyboard)
{
    int i = 0;
    auto it_ = headers.begin();
    auto it = it_;
    std::vector<std::string> names;
    for( ; it_ != headers.end(); it_++, i++) {
        if(i == index)
            it = it_;
        names.push_back(std::string((*it_)->name.buff.begin(), (*it_)->name.buff.end()));
    }
    Header *h = *it;
    home();
    clrscr();
    printHeader(header->type.number);

    printLine("CRC", h->crc.number, 'h');

    printLine("SIZE HEADER", h->size_header.number);

    if(h->flags_specific.number & 0x1)
        printLine("Directory file system object");

    if(h->flags_specific.number & 0x04)
        printLine("CRC UNPACK DATA", h->unpacked_crc.number, 'h');

    if(h->flags_specific.number & 0x08)
        printLine("UNPACK DATA SIZE", "unknown");
    else
        printLine("UNPACK DATA SIZE", h->unpack_size.number);

    printLine("CREATOR", header->host_os_creator.number & 0x01 ? "UNIX" : "WINDOWS");

    if(!h->extra.time.mtime.empty())
        printLine("MODIFIC TIME", h->extra.time.mtime);

    if(!h->extra.time.ctime.empty())
        printLine("CREATE TIME", h->extra.time.ctime);

    if(!h->extra.time.atime.empty())
        printLine("LAST ACCESS TIME", h->extra.time.atime);

    printLine(h->package_data);
    for(int i = 0; i < names.size(); i++) {
        gotoxy(FIRST_COLUMN_WIDTH + SECOND_COLUMN_WIDTH + 5, i + 1);
        if(i == index) {
            set_display_atrib(F_BLACK);
            set_display_atrib(B_WHITE);
            printName(names[i], keyboard);
            resetcolor();
        } else
            printName(names[i], keyboard);
    }
//    std::cout << std::endl;
//    printf(ESC"[6n");

//    std::cout << "cur pos = " << keyboard.getXPosCurrentCursor() << std::endl;

//    std::cout << std::endl;
//    std::cout << std::endl;
}

