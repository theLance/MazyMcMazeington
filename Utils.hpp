#pragma once

#include <iostream>
#include <tuple>
#include <vector>


namespace utils {
int convertToInt(const char* s);

std::ostream& errorMsg(const std::string& msg);
}

struct Dimensions {
    unsigned int x;
    unsigned int y;
};

struct Coordinates : Dimensions {
    Coordinates() = default;
    Coordinates(unsigned int x, unsigned int y) : Dimensions{x, y} {}

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

    bool operator<(const Coordinates& b) const {
        return std::tie(x,y) < std::tie(b.x,b.y);
    }

    typedef Coordinates (Coordinates::*CoordFunc)() const;
    std::vector<CoordFunc> directionFunctions = {Coordinates::n, Coordinates::nw, Coordinates::ne,
                                                 Coordinates::s, Coordinates::sw, Coordinates::se,
                                                 Coordinates::w, Coordinates::e};
};

std::ostream& operator<<(std::ostream& os, const Coordinates& coord);
