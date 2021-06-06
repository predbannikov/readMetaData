#include <iostream>

#include "InfoArch.h"


int main(int argc, char* argv[])
{
    InfoArch archive;
    if(!archive.open("./test.rar")) {
        std::cerr << "exit program" << std::endl;
    }
    archive.readMetaData();


    std::cout << std::endl;
    return 0;
}
