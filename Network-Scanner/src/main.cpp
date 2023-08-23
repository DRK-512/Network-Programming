#include "ParamChecks.h"
#include "NetScanner.h"

int main(int argc, char* argv[]) {
    
    std::string subnet;
    std::string interface; 
       
    if(paramChecker(argc, argv, interface, subnet))
        std::cout << "We have valid parameters" << std::endl; 

// now that we have valid parameters, scan for IPs using ICMP
// now that we have IPs, scan the IPs MAC address and hostname
    return 0; 
}

