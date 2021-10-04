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

void InfoRAR5::debug_write(std::streampos beg, char *buff, int size)
{
    std::fstream f;
    f.open("test2.rar", std::ios::in | std::ios::binary);
    if(!f.is_open()) {
        std::cout << "file not opening to write" << std::endl;
        return;
    }
    f.seekg(beg);
    f.read(buff, size);
    f.close();
}

void InfoRAR5::deleteHeader(int index) {
    // Проверить индекс и можно ли этот хеадер удалить
    int i = 0;
    index_to_delete = index;
    mode = 1;
    to_file = new std::fstream;
    to_file->open("test2.rar", std::ios::out | std::ios::trunc | std::ios::ate | std::ios::binary);
    if(!to_file->is_open()) {
        std::cout << "file not open" << std::endl;
        throw;
    }
    headers.clear();
    file->seekg(std::ios::beg + 8);
    to_file->write(signature, sizeof (signature));
    std::streampos save_pos = to_file->tellp();
    std::streampos file_pos = file->tellg();
    to_file->sync();
    while(readNextBlock()) { }
    to_file->close();
}

Header *InfoRAR5::getHeaderOfIndex(int index)
{
    for(auto it = headers.begin(); it != headers.end(); it++) {
        if((*it)->index == index)
            return *it;
    }
    return nullptr;
}

void InfoRAR5::parseDataArea()
{
//    auto lambda = [](const char &byte) { return byte>31; }; // всё кроме непечатаемых символов (из ASCII до 31)
    if(header->flags_common.number & 0x02) {
        switch (header->state) {
        case STATE_FILE_HEADER:
            extractData(header->package_data, header->size_data.number);
            break;
        case STATE_SERVICE_HEADER:
            std::cout << "launch parse quick open offset" << std::endl;
            std::streampos begin_pos_to_file_;
            if(mode == 1) {

                to_file->sync();
                changeMainHeader();
                to_file->sync();

                begin_pos_to_file_ = to_file->tellp();
                std::cout << begin_pos_to_file_ << std::endl;
                std::cout << std::endl;
            }
            std::streampos begin_pos = file->tellg();
            int counter = 0;
            while(header->size_data.number != file->tellg() - begin_pos) {
                counter++;
                QuickOpenHeader *a = new QuickOpenHeader;
                header->service_data_area.sub_headers.push_back(a);
                if(counter == index_to_delete)
                    mode = 0;
                getQuickOpenHeader(a, counter);
                if(counter == index_to_delete)
                    mode = 1;
            }
            if(mode == 1) {
                int size_data = to_file->tellp() - begin_pos_to_file_;
                writeVIntOffset(header->size_data, size_data);
                writeVIntOffset(header->unpack_size, size_data);
                writeInt32Offset(header->crc.beg, getCRC(header->crc.end, header->name.end));
            }
            break;
        }
    }
}

void InfoRAR5::getQuickOpenHeader(QuickOpenHeader *a, int index)
{
    extractInt32(a->crc32);
    extractVInteger(a->size);
    extractVInteger(a->flags);
    extractVInteger(a->offset);
    extractVInteger(a->data_size);
    extractData(a->data, a->data_size.number);
    std::cout << std::string(a->data.buff.begin(), a->data.buff.end()) << std::endl;
    if(file->eof()) {
        std::cout << std::endl << "eof" << std::endl;
        throw std::runtime_error("unexpected eof");
    }

//    std::streampos save_pos = file->tellg();
//    std::streampos test_offset = header->pos.beg;
//    test_offset -= a->offset.number;
//    file->seekg(test_offset);

//    Header *head_tmp = header;
//    append_to_list = false;
//    readNextBlock();
//    append_to_list = true;
//    header = head_tmp;
//    file->seekg(save_pos);

    if(mode == 1) {
        Header *h = getHeaderOfIndex(index);
        int offset = service_pos_to_file - h->pos.beg;


//        std::streampos save_pos = file->tellg();
//        file->close();
//        file->open("test2.rar", std::ios::in | std::ios::binary);
//        if(!file->is_open()) {
//            std::cout << "file not found" << std::endl;
//            return ;
//        }
//        test_offset = service_pos_to_file;
//        test_offset -= offset;
//        file->seekg(test_offset);
//        std::cout << std::endl;
////        readNextBlock();

//        file->close();
//        file->open("test.rar", std::ios::in | std::ios::binary);
//        if(!file->is_open()) {
//            std::cout << "file not found" << std::endl;
//            return ;
//        }
//        file->seekg(save_pos);

        writeVIntOffset(a->offset, offset);
        writeInt32Offset(a->crc32.beg, getCRC(a->crc32.end, a->data_size.end));
    }
}

void InfoRAR5::redirectToFile(TypePos &var)
{
    if(index_to_delete == header->index)
        return;
    if(mode == 1) {
        TypePos new_pos;
        new_pos.beg = to_file->tellp();
        int size = var.end - var.beg;
        char *buff = new char[size];
        file->seekg(var.beg);
        file->read(buff, size);
        to_file->write(buff, size);
        new_pos.end = to_file->tellp();
        if(new_pos.beg == -1) {
            std::cout << "stop";
        }
        delete []buff;
        file->seekg(var.end);
        var.beg = new_pos.beg;
        var.end = new_pos.end;
    }
}

void InfoRAR5::extractVInteger(TypeVInt &var) {

    uint64_t result=0;
    std::streampos pos= file->tellg();
    var.beg=pos;
    for (uint8_t shift=0; pos != end; shift+=7)
    {
        if(file->eof())
            break;
        char curByte;
        file->read(&curByte, 1);
        result|=uint64_t(curByte & 0x7f)<<shift; 	// 0x7f = 0111 1111
        if ((curByte & 0x80)==0) { 					// проверка на старший бит "highest bit in every byte is the continuation flag. If highest bit is 0, this is the last byte in sequence. So first byte contains 7 least significant bits of integer and continuation flag"
            var.number=result;
            var.end=file->tellg();

            redirectToFile(var);

            return;
        }
    }
    throw std::runtime_error("the size vint exceeded bounds");
}

void InfoRAR5::extractData(TypeData &data_var, size_t length)
{
    data_var.beg = file->tellg();
    data_var.buff.resize(length);
    file->read(data_var.buff.data(), length);
    data_var.end = file->tellg();
    data_var.length = length;
    redirectToFile(data_var);
}

void InfoRAR5::extractInt32(TypeInt32 &var)
{
    var.beg=file->tellg();
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
    redirectToFile(var);
}

void InfoRAR5::extractInt64(TypeInt64 &var)
{
    var.beg=file->tellg();
    char buff[8];
    for (int i = 0; i < 8; i++)
    {
        if(file->eof())
            throw std::runtime_error("extractInt64 error");
        file->read(&buff[i], 1);
    }
    var.number = *reinterpret_cast<const uint64_t*>(buff);
    var.end = file->tellg();
    redirectToFile(var);
}

void InfoRAR5::changeMainHeader()
{
    std::streampos save_pos = to_file->tellp();

//    char buff1[18];
//    debug_write(main_header->pos.beg, buff1, 18);
//    std::cout << "1: ";
//    for(int i = 0; i < 18; i++)
//        std::cout << std::hex << (0xFF&buff1[i]);
//    std::cout << std::endl;

    writeVIntOffset(main_header->extra.locator.quick_open_offset, header->pos.beg - main_header->pos.beg);
    writeInt32Offset(main_header->crc.beg, getCRC(main_header->crc.end, main_header->pos.end));
}

void InfoRAR5::expandVInt(std::vector<char> &v, int size_vint)
{
    if(v.empty())
        return;
    if(v.size() < size_vint) {
        v[v.size()-1] |= 0x80;
        for(int i = v.size(); i < size_vint; i++)
            if(i == size_vint-1)
                v.push_back(0);
            else
                v.push_back(0x80);
    }
}

std::vector<char> InfoRAR5::packVInt(uint64_t vint)
{
    std::vector<char> result;
    int counter = 0;
    while(vint > 0x7F) {
        uint8_t tmp = 0x7F & vint;
        vint >>= 7;
        tmp |= 0x80;
        result.push_back(tmp);
    }
    if(vint > 0)
        result.push_back(0xFF&vint);
    return result;
}

uint32_t InfoRAR5::getCRC(std::streampos begin, std::streampos end)
{
    std::ifstream f;
    f.open("test2.rar", std::ios::in | std::ios::binary);
    if(!f.is_open()) {
        std::cerr << "file not open" << std::endl;
        throw std::runtime_error("file not opeing");
    }
    int size_for_crc = end - begin;
    std::vector<char> buff(size_for_crc);
    f.seekg(begin);
    f.read(buff.data(), size_for_crc);
    f.close();
    std::vector<unsigned char>for_crc(size_for_crc);
    std::copy(buff.begin(), buff.end(), for_crc.begin());
    return CRC32_function(for_crc.data(), size_for_crc);
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

void InfoRAR5::writeVIntOffset(const TypeVInt &vint, int num_vint)
{
    std::fstream f;
    f.open("test2.rar", std::fstream::out | std::fstream::in | std::fstream::binary);
    if(!f.is_open()) {
        std::cout << "file not open" << std::endl;
        return ;
    }
    std::vector<char> vvint = packVInt(num_vint);
    int diff = vint.end - vint.beg;
    expandVInt(vvint, diff);
    f.seekp(vint.beg);
    f.write(vvint.data(), vvint.size());
    f.close();
}

void InfoRAR5::writeInt32Offset(std::streampos target_pos, uint32_t num)
{
    std::fstream f;
    f.open("test2.rar", std::fstream::out | std::fstream::in | std::fstream::binary);
    if(!f.is_open()) {
        std::cout << "file not open" << std::endl;
        return ;
    }
    f.seekp(target_pos);
    f.write(reinterpret_cast<char*>(&num), 4);
    f.close();
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
        extractVInteger(header->extra.size);
        extractVInteger(header->extra.type);
        switch (header->state) {
        case STATE_MAIN_HEADER:
//            if(header->extra.type.number == 0x01)                std::cout << "locator record: type " << std::endl;
            extractVInteger(header->extra.locator.flags);
            if( header->extra.locator.flags.number & 0x01) {
                extractVInteger(header->extra.locator.quick_open_offset);
//                file->seekg(header->extra.locator.quick_open_offset.number + header->pos.beg);
//                readNextBlock();
            }
            if( header->extra.locator.flags.number & 0x02)
                extractVInteger(header->extra.locator.recovery_offset);		//  std::cout << "Recovery record offset is present. " << header->extra.locator.recovery_offset.number << std::endl;
            break;

        case STATE_FILE_HEADER:
        case STATE_SERVICE_HEADER:
            switch (header->extra.type.number) {
            case 0x01:
                break;
            case 0x02:
                break;
            case 0x03:
                extractVInteger(header->extra.time.flag);
                if(header->extra.time.flag.number & 0x0002)
                    header->extra.time.smtime = getTime(header->extra.time.mtime);
                else if(header->extra.time.flag.number & 0x0004)
                    header->extra.time.sctime = getTime(header->extra.time.ctime);
                else if(header->extra.time.flag.number & 0x0008)
                    header->extra.time.satime = getTime(header->extra.time.atime);
                break;
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

std::string InfoRAR5::getTime(TypeInt64 &var)
{
    time_t t;
    if(header->extra.time.flag.number & 0x0010) {
        extractInt32(var);
        var.number = (*reinterpret_cast<TypeInt32*>(&var)).number;
        t = static_cast<time_t>(var.number);
    } else {
        extractInt64(var);
        t = static_cast<time_t>(WindowsTickToUnixSeconds(var.number));
    }
    return std::asctime(std::localtime(&t));
}

void InfoRAR5::getCRCUnpackData()
{
    if(header->flags_specific.number & 0x04)
        extractInt32(header->unpacked_crc);
}

void InfoRAR5::getFileModifTime()
{
    if(header->flags_specific.number & 0x02)
        extractInt32(header->mtime);
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

bool InfoRAR5::setStateHeader()
{
    extractVInteger(header->type);
    switch (header->type.number) {
    case 1:
        main_header = header;
        header->state = STATE_MAIN_HEADER;
        return true;
    case 2:
        header->state = STATE_FILE_HEADER;
        return true;
    case 3:
        service_header = header;
        if(mode == 1)
            service_pos_to_file = header->pos.beg;
        header->state = STATE_SERVICE_HEADER;
        return true;
    case 4:
        return false;
    case 5:
        header->state = STATE_END_OF_ARCHIVE;
        return true;
    default:							// ошибка, случилось непредвиденное
        throw std::runtime_error("this value of typeHeder not match any type");
    }
    return false;
}

bool InfoRAR5::readNextBlock() {
    bool status = true;

    try {
        header = new Header;
        header->index = headers.size();
        if(mode == 1)
            header->pos.beg = to_file->tellp();
        else
            header->pos.beg = file->tellg();
        if(append_to_list)
            headers.push_back(header);//вставить в конец
        extractInt32(header->crc);
        extractVInteger(header->size_header);
        if(!setStateHeader()) 			// если не один из типов, то есть попалось инородное
            status = false;

        switch (header->state) {
        case STATE_MARKER_HEADER:
            break;
        case STATE_MAIN_HEADER:{
            extractVInteger(header->flags_common);
            getExtraAreaSize();
            extractVInteger(header->flags_specific);
            printFlagComm();
            parseExtraArea();
            if(header->flags_specific.number & 0x02)
                extractVInteger(header->volume_number);
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

            header->sname = std::string(header->name.buff.begin(), header->name.buff.end());
            std::cout << header->sname << std::endl;
            // ------------ area print specific header data --------------------
            parseExtraArea();
            parseDataArea();
            break;

        case STATE_END_OF_ARCHIVE:
            extractVInteger(header->flags_common);
            extractVInteger(header->end_of_archive_flags);
            if(header->end_of_archive_flags.number == 0x01)
                std::cout << "it is not last volume in the set"<< std::endl;
            else
                std::cout <<header->end_of_archive_flags.number<< "  that's all" << std::endl;
            status = false;
        }
    } catch (const std::exception &e) {
        std::cerr << "ERROR: [" << errno << "] " << e.what() << std::endl;		// ... какая-то ошибка
        status = false;
    }
    if(mode == 1)
        header->pos.end = to_file->tellp();
    else
        header->pos.end = file->tellg();
    std::cout << "size header = " << header->pos.end - header->pos.beg << std::endl;
    return status;
}

size_t InfoRAR5::getSizeHeaders()
{
    return headers.size();
}

Header::Header() {

}

//**********************************************************************************************
//**
//**********************************************************************************************
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
    if(data.beg == data.end)
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
    file->seekg(data.beg);
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

void InfoRAR5::printSmthInfo()
{
    std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Unpacked file or service data size:" << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header->unpack_size.number) << std::endl;
    std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Operating system specific file attributes:" << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header->attributes.number) << std::endl;
    std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Compression information:" << std::endl;
    std::cout << std::setw(EMPTY_SPACE_LEFT*2) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT-EMPTY_SPACE_LEFT) << "method: " << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header->compres_info.number & 0x0380) << std::endl;
}

/* Flags specific for file header type:
 * 0x0001   Directory file system object (file header only).
 * 0x0002   Time field in Unix format is present.
 * 0x0004   CRC32 field is present.
 * 0x0008   Unpacked size is unknown.
*/
void InfoRAR5::printFlagSpec()
{
    if(header->flags_specific.number & 0x1)
        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Directory file system object." << std::endl;
    if(header->flags_specific.number & 0x2)
        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Time field in Unix format is present." << std::endl;
    if(header->flags_specific.number & 0x4)
        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "CRC32 field is present. " << std::endl;
    if(header->flags_specific.number & 0x8)
        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Unpacked size is unknown. " << std::endl;
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

void InfoRAR5::printName(std::string &str, Keyboard &keyboard)
{
    int width_term, heigth_term;
    keyboard.get_terminal_size(width_term, heigth_term);
    int x;
    if(str.empty())
        str.push_back(' ');
    int pos = keyboard.getCurPosCursor();
    x = 0xFFFF & pos;
    char unicode[4];
    unicode[0] = ' ';
    int counter = 0;
    std::string res;
    for(size_t i = 0; i < str.length(); i++) {
        res.push_back(str[i]);
        if(static_cast<uint8_t>(str[i]) < 0x7F)
            x++;
        else {
            if((str[i] & 0xF0) == 0xF0)
                x++;
            else if((str[i] & 0xE0) == 0xE0)
                x++;
            else if((str[i] & 0xC0) == 0xC0)
                x++;
        }
        if(x >= width_term)
            break;
    }
    x--;
    std::string empty_space(width_term - x - 1 , ' ');
    std::cout << res << empty_space << std::flush;
}


void InfoRAR5::printHeader(uint64_t type)
{
    std::cout << "+" << fillStrCol("-", FIRST_COLUMN_WIDTH + SECOND_COLUMN_WIDTH + 1, '-') << "+" << std::endl;
    std::cout << "|";
    switch (type) {
    case 1:
        std::cout << fillStrCol("MAIN HEADER", FIRST_COLUMN_WIDTH + SECOND_COLUMN_WIDTH + 1);
        break;
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
    int width_term, heigth_term;
    keyboard.get_terminal_size(width_term, heigth_term);
    auto it_ = headers.begin();
    auto it = it_;
    std::vector<std::string> names;
    for(int i = 0; it_ != headers.end(); it_++, i++) {
        if(i == index)
            it = it_;
        names.push_back(std::string((*it_)->name.buff.begin(), (*it_)->name.buff.end()));
    }
    Header *h = *it;
    home();
    clrscr();
    printHeader(h->type.number);

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

    if(!h->extra.time.smtime.empty())
        printLine("MODIFIC TIME", h->extra.time.smtime);

    if(!h->extra.time.sctime.empty())
        printLine("CREATE TIME", h->extra.time.sctime);

    if(!h->extra.time.satime.empty())
        printLine("LAST ACCESS TIME", h->extra.time.satime);

    printLine(h->package_data);
    for(int i = 0; i < names.size(); i++) {
        gotoxy(FIRST_COLUMN_WIDTH + SECOND_COLUMN_WIDTH + 5, i + 1);
        if(i == index) {
            set_display_atrib(F_BLACK);
            set_display_atrib(B_WHITE);
            printName(names[i], keyboard);
            resetcolor();
        } else {
            printName(names[i], keyboard);
        }
    }
}

