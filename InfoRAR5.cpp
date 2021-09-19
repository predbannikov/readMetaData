#include "InfoRAR5.h"
#define EMPTY_SPACE_LEFT                5           ////просто для выравнивания при печати
#define EMPTY_SPACE_AFTER_LEFT          45
#define EMPTY_SPACE_RIGHT_NUMBER        7
const char InfoRAR5::signature[LENGTH_SIGNATURE_FOR_5_X_VERSION_RAR] {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07, 0x1, 0x0};

InfoRAR5::InfoRAR5(std::vector<char> &data_) : BaseRAR(data_){ // инициализируем BaseRAR
    pos = data->begin(); 	// в начало архива
    std::advance(pos, 8); 	// продвижение на 8 байтов, тк уже знаем версию
}
/*Flags specific for file header type:
            0x0001   Directory file system object (file header only).
            0x0002   Time field in Unix format is present.
            0x0004   CRC32 field is present.
            0x0008   Unpacked size is unknown.*/
void InfoRAR5::printFlagSpec()
{
    if(header->flags_common & 0x1)
        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Directory file system object." << std::endl;
    if(header->flags_common & 0x2)
        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Time field in Unix format is present." << std::endl;
    if(header->flags_common & 0x4)
        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "CRC32 field is present. " << std::endl;
    if(header->flags_common & 0x8)
        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Unpacked size is unknown. " << std::endl;
}

void InfoRAR5::printCRCData()
{
    if(header->flags_specific & 0x04)
        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "CRC32 of unpacked file or service data:"
            << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::hex << header->crc_data << std::dec << std::endl;
}

void InfoRAR5::printCompresMethod()
{
    std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Compression information:" << std::endl;
    std::cout << std::setw(EMPTY_SPACE_LEFT*2) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT-EMPTY_SPACE_LEFT) << "method: "
            << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header->compres_info & 0x0380) << std::endl;
}

void InfoRAR5::printDataArea()
{
    if(header->flags_common & 0x02) {
        auto lambda = [](const char &byte) {
            return byte>31; // всё кроме непечатаемых символов (из ASCII до 31)
        };
        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Data area:" << std::endl;
        std::cout << std::setw(EMPTY_SPACE_LEFT*2) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT-EMPTY_SPACE_LEFT) << "size: "
            << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header->size_data) << std::endl;
        std::cout << std::setw(EMPTY_SPACE_LEFT*2) << " " << std::left << "\"" ;
        auto data_size_it_loc = pos + header->size_data;
        if(MAX_SHOW_NUMBER_DATA_HEADER < header->size_data) {
            std::copy_if(pos, pos+MAX_SHOW_NUMBER_DATA_HEADER, std::ostream_iterator<char>(std::cout), lambda);
            std::cout << "...\"" << std::endl;
        } else {
            std::copy_if(pos, pos + header->size_data, std::ostream_iterator<char>(std::cout), lambda);
            std::cout << "\"" << std::endl;
        }
        pos = data_size_it_loc;
    }
}

void InfoRAR5::printName()
{
    std::string str(header->name.length, ' ');
    std::copy(header->name.begin, header->name.end, str.begin());	//;

    std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Name:" << str << std::endl;
    if(header->state == STATE_SERVICE_HEADER) {
        std::cout << std::setw(EMPTY_SPACE_LEFT*2) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT-EMPTY_SPACE_LEFT);
        if(str == "QO")
            std::cout << "Archive quick open data" << std::endl;
        else if(str == "CMT")
            std::cout << "Archive comment";
        else if(str == "ACL")
            std::cout << "NTFS file permissions";
        else if(str == "STM")
            std::cout << "NTFS alternate data stream";
        else if(str == "RR")
            std::cout << "Recovery record";
    }
}

void InfoRAR5::parseExtraArea()
{
    std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Extra area:" << std::endl;
    std::cout << std::setw(EMPTY_SPACE_LEFT*2) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT-EMPTY_SPACE_LEFT) << "size: "
        << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header->size_extra_area) << std::endl;

    if(header->flags_common & 0x01) {
        int size_extra = getVInteger();
        std::cout << "size of record data starting from TYPE " << size_extra << std::endl;
        auto pos_save = pos;
        int type_extra = getVInteger();
        std::cout << "type_extra " << type_extra << std::endl;
        switch (header->state) {
        case STATE_MAIN_HEADER: {
            if(type_extra == 0x01)
                std::cout << "locator record: type " << std::endl;
            int record_flag = getVInteger();
            std::cout << "record flag. " << record_flag << std::endl;
            if(record_flag & 0x01) {
                std::cout << "Quick open record offset is present. " << getVInteger() << std::endl;
            }
            if(record_flag & 0x02) {
                std::cout << "Recovery record offset is present. " << getVInteger() << std::endl;
            }
            break;
        }
        case STATE_FILE_HEADER:
        case STATE_SERVICE_HEADER:
            switch (type_extra) {
            case 0x01:
                break;
            case 0x02:
                break;
            case 0x03: {
                vint_t flag = getVInteger();
                    uint64_t mtime;		// time modification
                    uint64_t ctime;		// time creation
                    uint64_t atime;		// last access
                    if(flag & 0x0002) {
                        mtime = flag&0x0010 ? *reinterpret_cast<const uint32_t*>(&*pos) : *reinterpret_cast<const uint64_t*>(&*pos);
                        std::advance(pos, 8);
                        std::cout << "mtime = " << mtime << std::endl;
                    } else if(flag & 0x0004) {
                        ctime = flag&0x0010 ? *reinterpret_cast<const uint32_t*>(&*pos) : *reinterpret_cast<const uint64_t*>(&*pos);
                        std::advance(pos, 8);
                        std::cout << "ctime = " << ctime << std::endl;
                    } else if(flag & 0x0008) {
                        atime = flag&0x0010 ? *reinterpret_cast<const uint32_t*>(&*pos) : *reinterpret_cast<const uint64_t*>(&*pos);
                        std::advance(pos, 8);
                        std::cout << "atime = " << atime << std::endl;

                    }
                    int diff = pos - pos_save;
                    if(diff == size_extra)
                        std::cout << "ok" << std::endl;
                    else {
                        throw std::runtime_error("In file time record: time determine error");
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

void InfoRAR5::getName()
{
//    header->name.insert(header->name.end(), pos, pos+header->length_name);
//    header->name.it = pos;
    header->name.extract(pos);
//    header->name.data.insert(header->name.data.end(), pos, pos + header->name.length);
//    std::advance(pos, header->length_name);
//    std::advance(pos, header->name.length);
}

void InfoRAR5::getGetCRCDate()
{
    if(header->flags_specific & 0x04) {
        header->crc_data = *reinterpret_cast<const uint32_t*>(&*pos);
        std::advance(pos, 4);
    }
}

void InfoRAR5::getFileModifTime()
{
    if(header->flags_specific & 0x02)  {
        uint32_t header_mtime = *reinterpret_cast<const uint32_t*>(&*pos);
        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Modification time in Unix time format:"
            << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header_mtime) << std::endl;
        std::advance(pos, 4);
    }
}

void InfoRAR5::getExtraAreaSize()
{
    if(header->flags_common & 0x01)
        header->size_extra_area = getVInteger();
}

void InfoRAR5::getSizeData()
{
    if(header->flags_common & 0x02)
        header->size_data = getVInteger();
}

void InfoRAR5::getUnpackSize()
{
    if(header->flags_specific & 0x08)
        header->ignorUnpackSize = true;
    else
        header->ignorUnpackSize = false;
    header->unpack_size = getVInteger();
}

void InfoRAR5::printHostCreator()
{
    if(header->host_os_creator & 0x0)
        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "UNIX used to create this archive." << std::endl;
    else
        std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "WINDOWS used to create this archive." << std::endl;
}

void InfoRAR5::printFlagComm()
{
    if(header->flags_common & 0x01)
        std::cout << "Extra area is present in the end of header." << std::endl;
    else if(header->flags_common & 0x02)
        std::cout << "Data area is present in the end of header." << std::endl;
    else if(header->flags_common & 0x04)
        std::cout << "Blocks with unknown type and this flag must be skipped when updating an archive." << std::endl;
    else if(header->flags_common & 0x08)
        std::cout << "Data area is continuing from previous volume." << std::endl;
    else if(header->flags_common & 0x10)
        std::cout << "Data area is continuing in next volume." << std::endl;
    else if(header->flags_common & 0x20)
        std::cout << "Block depends on preceding file block." << std::endl;
    else if(header->flags_common & 0x40)
        std::cout << "Preserve a child block if host block is modified." << std::endl;
}

bool InfoRAR5::setStateHeader()
{
    header->type = getVInteger();
    switch (header->type) {
    case 1:
        header->state = STATE_MAIN_HEADER;
        std::cout << "BLOCK MAIN HEAD size = " << header->size_header<< std::endl;
        return true;
    case 2:
        header->state = STATE_FILE_HEADER;
        std::cout << "BLOCK FILE HEAD size = " << header->size_header << std::endl;
        return true;
    case 3:
        header->state = STATE_SERVICE_HEADER;
        std::cout << "BLOCK SERVICE HEAD size = " << header->size_header << std::endl;
        return true;
    case 4:
        std::cout << "this is encryption header" << std::endl;
        return false;
    case 5:
        header->state = STATE_END_OF_ARCHIVE;
        return true;
    default:							// ошибка, случилось непредвиденное
        throw std::runtime_error("this value of typeHeder not match any type");
    }
    return false;
}

/*
 *   READ HEADER BLOCK
 */
bool InfoRAR5::readNextBlock() {
    try {

        header = new Header;
        map[*reinterpret_cast<const uint32_t*>(&*pos)] = header;	// reinterpret_cast  тк pos размера char  CRC
        std::advance(pos,4);
        header->size_header = getVInteger(); 	// эта функция выдает размер (коммент из InfoRAR5.h)

        if(!setStateHeader()) 			// если не один из типов, то есть попалось инородное
            return false;

        switch (header->state) {
        case STATE_MARKER_HEADER:
            break;
        case STATE_MAIN_HEADER:{
            auto begin_header_pos = pos;
            header->flags_common = getVInteger();
            if(header->flags_common & 0x01) {
                std::cout << "archive is part of a multivolume" << std::endl;
                header->size_extra_area = getVInteger();
            }
            vint_t archive_flags = getVInteger();
            if(header->flags_common & 0x02) {
                std::cout << "Volume number field is present" << std::endl;
            }
            if(header->flags_common & 0x04) {
                std::cout << "Solid archive." << std::endl;
            }
            if(header->flags_common & 0x08) {
                std::cout << "Recovery record is present." << std::endl;
            }
            if(header->flags_common & 0x10) {
                std::cout << "Lecked archive" << std::endl;
            }
            // ----------
            printFlagComm();
            parseExtraArea();
            if(archive_flags & 0x02) {
                std::cout << "number volume " << getVInteger() << std::endl;
            }
            std::cout << std::endl;
            int diff_pos = begin_header_pos - 1 + header->size_header- pos;
            std::cout << "diff " << diff_pos << std::endl;
            std::cout << std::endl;

            break;
        }
        case STATE_FILE_HEADER:            // Этот заголовок не использует метот сжатия
        case STATE_SERVICE_HEADER:
            header->flags_common = getVInteger();
            getExtraAreaSize();
            getSizeData();
            header->flags_specific = getVInteger();
            getUnpackSize();
            header->attributes = getVInteger();
            getFileModifTime();
            getGetCRCDate();
            header->compres_info = getVInteger();
            header->host_os_creator = getVInteger();
//            header->name.length = getVInteger();
            getName();		// перенести на извлечение данных выше TODO
//            header->length_name = getVInteger();
            // ------------ area print specific header data --------------------
            printHostCreator();
            printFlagSpec();
            printCRCData();
            printCompresMethod();
            printFlagComm();
            std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Unpacked file or service data size:" << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header->unpack_size) << std::endl;
            std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Operating system specific file attributes:" << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header->attributes) << std::endl;
            std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Compression information:" << std::endl;
            std::cout << std::setw(EMPTY_SPACE_LEFT*2) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT-EMPTY_SPACE_LEFT) << "method: " << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header->compres_info & 0x0380) << std::endl;

           //----------------------------
            printName();
          //-------------------------------------------------
            parseExtraArea();
            printDataArea();
            std::cout << std::endl;
            break;


        case STATE_END_OF_ARCHIVE:
            return false;
        }
    } catch (const std::exception &e) {
        std::cerr << "ERROR: [" << errno << "] " << e.what() << std::endl;		// ... какая-то ошибка
        return false;
    }
    return true;
}

vint_t InfoRAR5::getVInteger() {
    vint_t result=0;
    for (uint shift=0; pos != data->end(); shift+=7)
    {
        uint8_t curByte = *pos;
        pos++; 										// прибавляем размер char
        result+=uint64_t(curByte & 0x7f)<<shift; 	// 0x7f = 0111 1111
        if ((curByte & 0x80)==0) { 					// проверка на старший бит "highest bit in every byte is the continuation flag. If highest bit is 0, this is the last byte in sequence. So first byte contains 7 least significant bits of integer and continuation flag"
            return result;
        }
    }
    throw std::runtime_error("the size vint exceeded bounds");
}
