#pragma once

#include <iostream>
#include <vector>

#include "Utils.hpp"


const char WALL = 'x';
const char BEGIN = 'B';
const char END = 'E';
const char PATH = 'o';
const char EMPTY = ' ';


struct Maze {
    explicit Maze(const Dimensions& dims)
        : array{dims.y, decltype(array)::value_type(dims.x, EMPTY)} {
    }

    char& operator[](const Coordinates& coord) {
        return array[coord.y][coord.x];
    }

    const char& operator[](const Coordinates& coord) const {
        return array[coord.y][coord.x];
    }

    std::vector<std::vector<char>> array;
};


struct BoundingBox {
    Coordinates tl;
    Coordinates br;
};
