#pragma once

#include <chrono>
#include <random>

#include "Maze.hpp"

class RandomCoordinateGenerator {
public:
    RandomCoordinateGenerator(const BoundingBox& bb)
            : seed(std::chrono::system_clock::now().time_since_epoch().count()
                   % reinterpret_cast<unsigned long long>(this)
                   + std::random_device{}())
            , gen{static_cast<unsigned int>(seed)}
            , xGen(bb.tl.x, bb.br.x)
            , yGen(bb.tl.y, bb.br.y)
            , pathChoice(0, 3) {
    }

    Coordinates getRandomCoordinate() {
        return { xGen(gen), yGen(gen) };
    }

    bool coinFlip() {
        return pathChoice(gen) % 2;
    }

    unsigned pickRandomFrom(unsigned int number) {
        return std::uniform_int_distribution<unsigned int>(0, number - 1)(gen);
    }

private:
    unsigned long long seed;
    std::mt19937 gen;

    std::uniform_int_distribution<unsigned int> xGen;
    std::uniform_int_distribution<unsigned int> yGen;
    std::uniform_int_distribution<unsigned int> pathChoice;
};