#pragma once

#include <iosfwd>

class Dimensions;
class Maze;
class MazeCreator;


std::ostream& operator<<(std::ostream& os, const Maze& maze);
std::ostream& operator<<(std::ostream& os, const MazeCreator& mc);


namespace validate {
enum Result {
    OK = 0,
    NOK
};


namespace input {
    Result commandLineArguments(int argc);

    Result widthHeightMinimum(int x, int y);

    Result dimensions(const Dimensions& dims);
}

namespace output {
    /// Check whether there are no 2x2 space tile clusters.
    Result noFreeClusters(const Maze& maze);

    // Using the creator to utilize some functionalities that make traversal easier
    Result fullyTraversable(MazeCreator& mc);
}
}
