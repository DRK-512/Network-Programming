#include "functions.h"

int main(int argc, char* argv[]) {
    
    std::string subnet;
    std::string interface; 
       
    if(paramChecker(argc, argv, interface, subnet))
        std::cout << "We have valid parameters" << std::endl; 

    return 0; 
}

