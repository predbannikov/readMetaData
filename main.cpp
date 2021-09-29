#include <iostream>
#include "InfoArch.h"


int main(int argc, char* argv[])
{
    InfoArch archive;
    if( argc < 2 )
    {
        std::cout <<"Need a file!" << std::endl;
        return -1;
    }
    try {
        if(!archive.open(argv[1]))
        {
            std::cerr << "exit program" << std::endl;
        }
        archive.work();
    }  catch (...) {
        std::cerr << "unknown error" << std::endl;
        return -1;
    }
    return 0;
}
