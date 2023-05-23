#include <iostream>

#include "MazeCreator.hpp"
#include "Utils.hpp"
#include "Validators.hpp"

#include "tests.hpp"



int xFromArgs(char** argv) {
    return utils::convertToInt(const_cast<const char*>(argv[1]));
}

int yFromArgs(char** argv) {
    return utils::convertToInt(const_cast<const char*>(argv[2]));
}

int main(int argc, char** argv) {
    if(validate::input::commandLineArguments(argc) == Result::NOK) {
        return 1;
    }

    int x = xFromArgs(argv);
    int y = yFromArgs(argv);
    if(validate::input::widthHeightMinimum(x, y)) {
        return 1;
    }

    Dimensions dims{x, y};

    if(validate::tests() == validate::Result::NOK)
    {
        return -1;
    }

    MazeCreator mc(std::move(dims));
    mc.create();

    if(validate::output::noFreeClusters(mc.result()) == validate::Result::NOK) {
        utils::errorMsg("Large free cluster in map!") << std::endl << mc.result();
        return 1;
    }
    if(validate::output::fullyTraversable(mc) == validate::Result::NOK) {
        utils::errorMsg("Maze not fully traversible!") << std::endl << mc.result();
        return 1;
    }
    std::cout << mc.result();

    return 0;
}


