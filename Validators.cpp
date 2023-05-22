#include "Validators.hpp"

#include <iostream>

#include "Maze.hpp"
#include "MazeCreator.hpp"
#include "Utils.hpp"


std::ostream& operator<<(std::ostream& os, const Maze& maze) {
    for(const auto& line : maze.array) {
        for(char c : line) {
            os << c;
        }
        os << std::endl;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const MazeCreator& mc) {
    os << mc.result();
    return os;
}


namespace validate {
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
    Result noFreeClusters(const Maze& maze) {
        for(unsigned int y = 1; y < maze.array.size() - 2; ++y) {
            for(unsigned int x = 1; x < maze.array[y].size() - 2; ++x) {
                if(maze.array[y][x] == ' '   && maze.array[y][x+1] == ' ' &&
                   maze.array[y+1][x] == ' ' && maze.array[y+1][x+1] == ' ') {
                    return Result::NOK;
                }
            }
        }
        return Result::OK;
    }

    // Using the creator to utilize some functionalities that make traversal easier
    Result fullyTraversable(MazeCreator& mc) {
        // copy before modification

        // mc.forInnerBb([&mc](const Coordinates& c) {
        //     auto& tile = mc.theMaze[c];
        //     if(tile == BEGIN || tile == END) {
        //         tile = EMPTY;
        //     }
        // });

        // auto start = mc.theRand.getRandomCoordinate();
        // while(mc.theMaze[start] != EMPTY) {
        //     mc.theRand.getRandomCoordinate();
        // }

        std::set<Coordinates> activeSeekers;

        return Result::OK;
    }
}
}
