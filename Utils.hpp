#pragma once

#include <iostream>
#include <sstream>

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

struct Dimensions {
    unsigned int x;
    unsigned int y;
};

struct Coordinates : Dimensions {
    Coordinates n() const {
        return {x, y-1};
    }
    Coordinates nw() const {
        return {x-1, y-1};
    }
    Coordinates ne() const {
        return {x+1, y-1};
    }
    Coordinates w() const {
        return {x-1, y};
    }
    Coordinates e() const {
        return {x+1, y};
    }
    Coordinates s() const {
        return {x, y+1};
    }
    Coordinates sw() const {
        return {x-1, y+1};
    }
    Coordinates se() const {
        return {x+1, y+1};
    }
};

typedef Coordinates (Coordinates::*CoordFunc)() const;

std::ostream& operator<<(std::ostream& os, const Coordinates& coord) {
    os << "[" << coord.x << "-" << coord.y << "]";
    return os;
}
