#include <iostream>
#include <iomanip>
#include <cstdint>

using namespace std;

string convertToHex(const std::string &numberStr)
{
    // Convert the string to a double
    double number;
    std::istringstream iss(numberStr);
    iss >> number;

    union
    {
        double d;
        uint64_t i;
    } converter;

    converter.d = number;
    // Convert the binary representation to hexadecimal
    std::ostringstream oss;
    oss << "0x" << std::hex << std::uppercase << converter.i;

    return oss.str();
}