#include "InfoRAR5.h"
const char InfoRAR5::signature[LENGTH_SIGNATURE_FOR_5_X_VERSION_RAR] {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07, 0x1, 0x0};

InfoRAR5::InfoRAR5(std::vector<char> &data_) : BaseRAR(data_){ // èíèöèàëèçèðóåì BaseRAR
    pos = data->begin(); 	// â íà÷àëî àðõèâà
    std::advance(pos, 8); 	//ïðîäâèæåíèå íà 8 áàéòîâ, òê óæå çíàåì âåðñèþ
}



bool InfoRAR5::setStateHeader()
{
    typeHeader = getVInteger();
    switch (typeHeader) {
    case 1:
        state = STATE_MAIN_HEADER;
        return true;
    case 2:
        state = STATE_FILE_HEADER;
        return true;
    case 3:
        std::cout << "this is service header" << std::endl;
        return false;
    case 4:
        std::cout << "this is encryption header" << std::endl;
        return false;
    case 5:
        state = STATE_END_OF_ARCHIVE;
        return true;
    default:
        std::cout << "îøèáêà, ñëó÷èëîñü íåïðåäâèäåííîå" << std::endl;
        throw;
        return false;

    }
    return false;
}



/*
 *   READ HEADER BLOCK
 */
bool InfoRAR5::readNextBlock() {
    try {
        uint32_t CRC_HEADER = *reinterpret_cast<const uint32_t*>(&*pos);//reinterpret_cast òê pos ðàçìåðà char
        std::advance(pos,4);
        size_header = getVInteger(); // ýòà ôóíêöèÿ âûäàåò ðàçìåð (êîììåíò èç InfoRAR5.h)

        if(!setStateHeader()) // åñëè íå îäèí èç òèïîâ, òî åñòü ïîïàëîñü èíîðîäíîå
            return false;



        switch (state) {
        case STATE_MARKER_HEADER:
            break;
        case STATE_MAIN_HEADER:{
            std::cout << "BLOCK MAIN HEAD size = " << size_header << std::endl;
            auto begin_header_pos = pos;
            vint_t header_flags = getVInteger();
            vint_t header_extra_area_sizye;
            if(header_flags & 0x01) {
                std::cout << "archive is part of a multivolume" << std::endl;
                header_extra_area_sizye = getVInteger();
            }
            vint_t archive_flags = getVInteger();
            if(header_flags & 0x02) {
                std::cout << "Volume number field is present" << std::endl;
            }
            if(header_flags & 0x04) {
                std::cout << "Solid archive." << std::endl;
            }
            if(header_flags & 0x08) {
                std::cout << "Recovery record is present." << std::endl;
            }
            if(header_flags & 0x10) {
                std::cout << "Lecked archive" << std::endl;
            }
            // ----------
            if(header_flags & 0x01) {
                std::cout << "extra area: diff " << begin_header_pos - 1 + size_header - pos << std::endl;
                std::cout << "size of record data starting from TYPE " << getVInteger() << std::endl;
                int type_main_header_extra = getVInteger();
                if(type_main_header_extra & 0x01) {
                    std::cout << "locator record: type " << type_main_header_extra << std::endl;

                }
                int record_flag = getVInteger();
                std::cout << "record flag. " << record_flag << std::endl;
                if(record_flag & 0x01) {
                    std::cout << "Quick open record offset is present. " << getVInteger() << std::endl;
                }
                if(record_flag & 0x02) {
                    std::cout << "Recovery record offset is present." << std::endl;
                }

            }
            if(archive_flags & 0x02) {
                std::cout << "number volume " << getVInteger() << std::endl;
            }
            std::cout << std::endl;
            int diff_pos = begin_header_pos - 1 + size_header - pos;
            std::cout << "diff " << diff_pos << std::endl;
            std::cout << "size_header = " << size_header << std::endl;
            std::cout << std::endl;

            break;
        }
        case STATE_FILE_HEADER: {
#define EMPTY_SPACE_LEFT                5           ////ïðîñòî äëÿ âûðàâíèâàíèÿ ïðè ïå÷àòè
#define EMPTY_SPACE_AFTER_LEFT          45
#define EMPTY_SPACE_RIGHT_NUMBER        7
            std::cout << "BLOCK FILE HEAD size = " << size_header << std::endl;
            vint_t header_flag = getVInteger();
            vint_t header_extra_area_size = getVInteger();
            vint_t header_data_size = getVInteger();
            vint_t header_file_flags = getVInteger();
            vint_t header_unpacket_size = getVInteger();
            vint_t header_attributes = getVInteger();

            uint32_t header_CRC = *reinterpret_cast<const uint32_t*>(&*pos);
            std::advance(pos, 4);

            vint_t header_compression_info = getVInteger();
            vint_t header_host_os = getVInteger();
          /*Flags specific for file header type:
            0x0001   Directory file system object (file header only).
            0x0002   Time field in Unix format is present.
            0x0004   CRC32 field is present.
            0x0008   Unpacked size is unknown.*/
            if(header_file_flags & 0x1) std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Directory file system object." << std::endl;
            if(header_file_flags & 0x2) std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Time field in Unix format is present." << std::endl;
            if(header_file_flags & 0x4) std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "CRC32 field is present. " << std::endl;
            if(header_file_flags & 0x8) std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Unpacked size is unknown. " << std::endl;
            std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Unpacked file or service data size:" << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header_unpacket_size) << std::endl;
            std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Operating system specific file attributes:" << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header_attributes) << std::endl;
            if(header_file_flags & 0x02)  {
                uint32_t header_mtime = *reinterpret_cast<const uint32_t*>(&*pos);
                std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Modification time in Unix time format:" << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header_mtime) << std::endl;
                std::advance(pos, 4);
            }

            std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "CRC32 of unpacked file or service data:" << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header_CRC) << std::endl;
            std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Compression information:" << std::endl;
            std::cout << std::setw(EMPTY_SPACE_LEFT*2) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT-EMPTY_SPACE_LEFT) << "method: " << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header_compression_info & 0x0380) << std::endl;
            if(header_host_os & 0x0)
                std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "UNIX used to create this archive." << std::endl;
            else
                std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "WINDOWS used to create this archive." << std::endl;



            vint_t length_name = getVInteger();
            std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Name:";
            std::copy(pos, pos+length_name, std::ostream_iterator<char>(std::cout));

            std::advance(pos, length_name);


            std::cout << std::endl;

            auto lambda = [](const char &byte) {
                    return byte>31; // âñ¸ êðîìå íåïå÷àòàåìûõ ñèìâîëîâ (èç ASCII äî 31)
                };

            if(header_flag & 0x01) {
                std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Extra area:" << std::endl;
                std::cout << std::setw(EMPTY_SPACE_LEFT*2) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT-EMPTY_SPACE_LEFT) << "size: " << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header_extra_area_size) << std::endl;
                std::cout << std::setw(EMPTY_SPACE_LEFT*2) << " " << std::left << "\"" ;
                auto area_it_loc = pos + header_extra_area_size;
                std::copy_if(pos, area_it_loc, std::ostream_iterator<char>(std::cout), lambda);
                std::cout << "\"" << std::endl;
                pos = area_it_loc;
            }
            if(header_flag & 0x02) {
                std::cout << std::setw(EMPTY_SPACE_LEFT) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT) << "Data area:" << std::endl;
                std::cout << std::setw(EMPTY_SPACE_LEFT*2) << " " << std::left << std::setw(EMPTY_SPACE_AFTER_LEFT-EMPTY_SPACE_LEFT) << "size: " << std::setw(EMPTY_SPACE_RIGHT_NUMBER) << std::to_string(header_data_size) << std::endl;
                std::cout << std::setw(EMPTY_SPACE_LEFT*2) << " " << std::left << "\"" ;
                auto data_size_it_loc = pos + header_data_size;
                std::copy_if(pos, pos+MAX_SHOW_NUMBER_DATA_HEADER, std::ostream_iterator<char>(std::cout), lambda);
                std::cout << "\"" << std::endl;
                pos = data_size_it_loc;
            }

            std::cout << std::endl;
            break;
        }
        case STATE_END_OF_ARCHIVE:
            return false;
        }
    }  catch (...) {
        std::cerr << "ERROR: [" << errno << "]" << "... êàêàÿ-òî îøèáêà" << std::endl;
        return false;
    }

    return true;
}

vint_t InfoRAR5::getVInteger() {
    vint_t result=0;
    for (uint shift=0; pos != data->end(); shift+=7)
    {
        uint8_t curByte = *pos;
        pos++; 										// ïðèáàâëÿåì ðàçìåð char
        result+=uint64_t(curByte & 0x7f)<<shift; 	//0x7f = 0111 1111
        if ((curByte & 0x80)==0) { 					// ïðîâåðêà íà ñòàðøèé áèò "highest bit in every byte is the continuation flag. If highest bit is 0, this is the last byte in sequence. So first byte contains 7 least significant bits of integer and continuation flag"
            return result;
        }
    }
    throw ;
}
