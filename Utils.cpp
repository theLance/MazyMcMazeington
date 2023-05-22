#include "Utils.hpp"

#include <iostream>
#include <sstream>
#include <tuple>


namespace utils {
int convertToInt(const char* s) {
    std::stringstream  ss;
    ss << s;
    int i;
    ss >> i;
    return i;
}

std::ostream& errorMsg(const std::string& msg) {
    std::cerr << "ERROR: " << msg << std::endl;
    return std::cerr;
}
}

std::ostream& operator<<(std::ostream& os, const Coordinates& coord) {
    os << "[" << coord.x << "-" << coord.y << "]";
    return os;
}
