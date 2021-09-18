#ifndef INFORAR5_H
#define INFORAR5_H
#include "baserar.h"

#define LENGTH_SIGNATURE_FOR_5_X_VERSION_RAR    8
#define MAX_SHOW_NUMBER_DATA_HEADER 			5

using vint_t = uint64_t;

class InfoRAR5 : public BaseRAR{
    vint_t size_header;
    vint_t typeHeader;
    vint_t flagHeader;
    vint_t extraAreaSize;
    vint_t archiveFlags;
    vint_t volumeNumber;
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
