#include "functions.h"

int main(int argc, char* argv[])
{
    std::string address;
    std::vector<int> ports;
    bool showClosed = false; 
    
    /* get command line arguments */
    if(argc>=2)
    {   /* switch statements don't support strings in C++, so that is why I used this */
        for(uint8_t i=0; i<argc; i++) {
            if((std::string(argv[i])=="-i") || (std::string(argv[i])=="--ip-address"))
                address = argv[i+1]; 
            else if((std::string(argv[i])=="-p") || (std::string(argv[i])=="--port"))
                /* here we convert the ports input into the correct format needed */
                ports=parse_ports_list(std::string(argv[i+1])); 
            else if((std::string(argv[i])=="-s") || (std::string(argv[i])=="--show"))
                showClosed=true; 
            else if((std::string(argv[i])=="-h") || (std::string(argv[i])=="--help"))
            {
                helpFunction(); 
                return 0;  
            }
            
        }
    } 

    /* fill in defualt values if needed */
    if(address=="") 
        address = "127.0.0.1";
    if (ports==parse_ports_list(""))
        ports = parse_ports_list("0-65535");    

    /* scan ports with respect to the address given */
    std::cout << "Showing open ports on " << address << "...\n";
    /* this formats the number of spaces needed for the ports */
    size_t width = num_digits(maximum(ports));
    for (int port : ports) 
    {
        if(showClosed)
        {
            /* This will check if the socket is open or not */
            if ((sf::TcpSocket().connect(address, port) == sf::Socket::Done))
                std::cout << "Port " << std::setw(width) << port << " : OPEN\n";
            else
                std::cout << "Port " << std::setw(width) << port << " : CLOSED\n";
        }
        else 
        {
            if ((sf::TcpSocket().connect(address, port) == sf::Socket::Done))
                std::cout << "Port " << std::setw(width) << port << " : OPEN\n";
        }
    }
    std::cout << std::flush;
    return 0;
}
