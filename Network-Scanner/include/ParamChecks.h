#ifndef PARAMCHECKS_H
#define PARAMCHECKS_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <ifaddrs.h>

bool helpFunction();  
bool checkSubnet(std::string subnet); 
bool checkInterface(std::string interface); 
bool paramChecker(int argc, char* argv[], std::string& interface, std::string& subnet); 

#endif
