
#include "functions.h"

void helpFunction()
{
    std::cout << "This function is used to scan ports from a given IP\n"; 
    std::cout << " -i, --ip-address   input the IP addresses you are looking for\n"; 
    std::cout << "                    Examples: \n";
    std::cout << "                              -i 127.0.0.1 will search IP 127.0.0.1\n";
    std::cout << "                              -i localhost will search the IP 127.0.0.1\n";
    std::cout << "                                 default IP is localhost\n";
    
    std::cout << "\n -p, --port       input the ports you are scanning"; 
    std::cout << "\n                  Examples: ";
    std::cout << "\n                            -p 80 will scan port 80"; 
    std::cout << "\n                            -p 0-65535 will scan ports 0-65535";
    std::cout << "\n                               default ports to scan are 0-65535\n";

    std::cout << "\n -s, --show       display ports that are closed as well as open\n";

    std::cout << "\n -h, --help       display this help and exit\n";
}

bool port_is_open(const std::string& address, int port)
{
    return (sf::TcpSocket().connect(address, port) == sf::Socket::Done);
}


int string_to_int(const std::string& string)
{
    std::stringstream sstream(string);
    int i;
    sstream >> i;
    return i;
}

std::vector<std::string> split(const std::string& string, char delimiter, bool allow_empty)
{
    std::vector<std::string> tokens;
    std::stringstream sstream(string);
    std::string token;
    while (std::getline(sstream, token, delimiter)) {
        if (allow_empty || token.size() > 0)
            tokens.push_back(token);
    }
    return tokens;
}

std::vector<int> parse_ports_list(const std::string& list)
{
    std::vector<int> ports;
    for (const std::string& token : split(list, ',')) {
        std::vector<std::string> strrange = split(token, '-');
        switch (strrange.size()) {
            case 0: ports.push_back(string_to_int(token));       break;
            case 1: ports.push_back(string_to_int(strrange[0])); break;
            case 2:
            {
                int min = string_to_int(strrange[0]),
                    max = string_to_int(strrange[1]);
                for (int port : range(min, max))
                    ports.push_back(port);
                break;
            }
            default:
                break;
        }
    }
    return ports;
}
