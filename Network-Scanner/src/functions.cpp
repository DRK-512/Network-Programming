
#include "functions.h"

bool helpFunction() {
    std::cout << "-i, --interface    is the network interface you are scanning" << std::endl;
    std::cout << "-s, --subnet       subnet used to scan the network interface" << std::endl;
    std::cout << "-h, --help         display this help and exit" << std::endl;
    std::cout << "Example run: ./networkscanner -i eth0 -s 192.168.2.0" << std::endl; 
    return false; 
}

bool paramChecker(int argc, char* argv[], std::string& interface, std::string& subnet) {
    std::string input; 
    for (int i = 1; i < argc; ++i) {
        // convert char* -> string so we can compare
        input = argv[i]; 
        if (input == "-i" || input == "--interface") {
            if (i + 1 < argc) {
                interface = argv[i + 1];
                i++;
            } else {
                std::cerr << "ERROR: No value provided for " << input << " flag." << std::endl;
                return false;
            }
        } else if (input == "-s" || input == "--subnet") {
            if (i + 1 < argc) {
                subnet = argv[i + 1];
                i++;
            } else {
                std::cerr << "ERROR: No value provided for " << input << " flag." << std::endl;
                return false;
            }
        } else if (input == "-h" || input == "--help") {
            return helpFunction(); 
        }
    }

    // if they leave one out, I can assume they need help
    if (interface.empty() || subnet.empty()) 
        return helpFunction(); 

    if(checkInterface(interface) && checkSubnet(subnet)) 
        return true;
    else if(checkInterface(interface))
        std::cerr << "ERROR: Incorrect value for subnet\nEX: -s 192.168.1.0" << std::endl; 
    else 
        std::cerr << "ERROR: Incorrect value for network interface\nEX: -i eth0" << std::endl; 

    return false; 
}

bool checkInterface(std::string interface) {
    
    bool interFound=false; 
    struct ifaddrs *ifaddr, *ifa;

    // populate a linked-list of ifaddrs, and see if it is successful 
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return false; 
    }

    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr)
            continue;
        // gotta check if it is null before checking sa_family or else problems come
        if (ifa->ifa_addr->sa_family == AF_PACKET)
            if(ifa->ifa_name == interface)
                interFound=true; 
    }

    freeifaddrs(ifaddr);
    return interFound;
}

bool checkSubnet(std::string subnet) {
    
    std::string segment;
    std::istringstream iss(subnet);
    std::vector<std::string> segments;

    // Split the subnet segments
    while (std::getline(iss, segment, '.'))
        segments.push_back(segment);

    // Check if we have 4 segments
    if (segments.size() != 4)
        return false;

    // Check if each segment is a valid number between 0 and 255
    for (const std::string& segment : segments) {
        try {
            int num = std::stoi(segment);
            if (num < 0 || num > 255)
                return false;
        } catch (const std::invalid_argument& e) {
            return false; // Not a valid integer
        }
    }
    return true;
}

