#pragma once

#include "Maze.hpp"
#include "Utils.hpp"


namespace validate {
enum Result {
    OK = 0,
    NOK
};

namespace input {
    Result commandLineArguments(int argc) {
        if(argc != 3) {
            std::cout << "Usage:\n  exec x y\n\n  x = width of maze\n  y = height of maze"
                      << std::endl;
            return Result::NOK;
        }
        return Result::OK;
    }

    namespace {
    unsigned int mazeArea(const Dimensions& dims) {
        return (dims.x - 2) * (dims.y - 2);
    }
    }

    Result widthHeightMinimum(int x, int y) {
        if(x < 3 || y < 3) {
            utils::errorMsg("width/height have to be at least 3, because of the border!");
            return Result::NOK;
        }
        return Result::OK;
    }

    Result dimensions(const Dimensions& dims) {
        if(mazeArea(dims) < 2) {
            utils::errorMsg("Area of maze (w/o border) has to be at least 2 to accomodate"
                            " beginning and end.");
            return Result::NOK;
        }
        return Result::OK;
    }
}

namespace output {
    /// Check whether there are no 2x2 space tile clusters.
    bool maze(const Maze& maze) {
        for(unsigned int y = 1; y < maze.array.size() - 2; ++y) {
            for(unsigned int x = 1; x < maze.array[y].size() - 2; ++x) {
                if(maze.array[y][x] == ' '   && maze.array[y][x+1] == ' ' &&
                   maze.array[y+1][x] == ' ' && maze.array[y+1][x+1] == ' ') {
                    return false;
                }
            }
        }
        return true;
    }
}
}