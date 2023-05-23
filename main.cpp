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
    if(validate::input::commandLineArguments(argc) == validate::Result::NOK) {
        return 1;
    }

    int x = xFromArgs(argv);
    int y = yFromArgs(argv);
    if(validate::input::widthHeightMinimum(x, y) == validate::Result::NOK) {
        return 1;
    }

    // since input has been validated to be larger than 0, this cast is safe
    Dimensions dims{static_cast<unsigned int>(x), static_cast<unsigned int>(y)};

    if(validate::tests() == validate::Result::NOK)
    {
        return -1;
    }

    std::cout << std::endl << "Generating " << x << "x" << y << " maze" << std::endl;

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


