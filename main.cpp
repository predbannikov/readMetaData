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
    if(!archive.open(argv[1]))
    {
        std::cerr << "exit program" << std::endl;
    }


    return 0;
}
