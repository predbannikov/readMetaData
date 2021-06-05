#include <iostream>

#include "InfoRAR.h"


int main(int argc, char* argv[])
{
    InfoRAR rar;
    if(!rar.open("./test.rar")) {
        std::cerr << "exit program" << std::endl;
    }
    rar.readMetaData();


    std::cout << std::endl;
    return 0;
}
