#include <iostream>
#include "InfoArch.h"


int main(int argc, char* argv[])
{
#if defined (_WIN32)
    SetConsoleOutputCP( CP_UTF8 );
#elif defined (__linux__)
#endif
    InfoArch archive;
    if( argc < 2 )
    {
        std::cout <<"Need a file!" << std::endl;
        return -1;
    }
    try {
        if(!archive.open(argv[1]))
            std::cerr << "exit program" << std::endl;
        else
            archive.work();
    }  catch (...) {
        std::cerr << "unknown error" << std::endl;
        return -1;
    }
    return 0;
}
