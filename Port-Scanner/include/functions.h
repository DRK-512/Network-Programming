#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <SFML/Network.hpp>
#include <sstream>
#include <string>
#include <vector>

void helpFunction(); 
bool port_is_open(const std::string& address, int port);
int string_to_int(const std::string& string); 

std::vector<int> parse_ports_list(const std::string& list); 
std::vector<std::string> split(const std::string& string, char delimiter = ' ', bool allow_empty = false); 

/* template functions must be in the header file because the compiler must see it as a part of main.cpp */
template <typename T> T maximum(const std::vector<T>& values)
{
    T max = values[0];
    for (T value : values) 
    {
        if (value > max)
            max = value;
    }
    return max;
}

template <typename T> size_t num_digits(T value)
{
    size_t count = (value < 0) ? 1 : 0;
    if (value == 0)
        return 0;
    while (value) 
    {
        value /= 10;
        ++count;
    };
    return count;
}

template <typename T> std::vector<T> range(T min, T max)
{
    /* if someone calls the function wrong, we fix it */
    if (min > max)
    {
        T temp = min; 
        min = max; 
        max = temp; 
    }
    if (min == max)
        return std::vector<T>(1, min);
    std::vector<T> values;
    for (; min <= max; ++min)
        values.push_back(min);
    return values;
}

#endif
