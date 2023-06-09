#pragma once

#include "RandomGenerator.hpp"
#include "Validators.hpp"


#include <algorithm>
#include <cmath>
#include <map>
#include <numeric>
#include <set>
#include <thread>


namespace validate {
Result randDistribution() {
    std::cout << "Testing rand...";
    // rnd
    BoundingBox bb{ {1,1}, {100,100} };
    RandomCoordinateGenerator rnd(bb);

    const unsigned int SAMPLE_SIZE = 10000;
    std::map<Coordinates, unsigned int> frequency;
    for(unsigned int i = 0; i < SAMPLE_SIZE; ++i) {
        ++frequency[rnd.getRandomCoordinate()];
    }
    std::cout << "generated...";

    auto reoccurances =
        std::accumulate(frequency.begin(), frequency.end(), std::multiset<unsigned int>(),
                        [](std::multiset<unsigned int>& occs, const decltype(frequency)::value_type& pair) {
                            if(pair.second) { occs.insert(pair.second); };
                            return occs;
                        });
    std::cout << "transformed...\n";

    if(reoccurances.size() < SAMPLE_SIZE / 3) {
        utils::errorMsg("Randomizer doesn't seem to uniformly distribute!");
        std::cout << "Top reoccurences:  ";
        auto it = reoccurances.rbegin();
        for(unsigned int i = 0; i < 20 && it != reoccurances.rend(); ++i) {
            std::cout << *it++ << " ";
        }
        return Result::NOK;
    }

    return Result::OK;

}

Result subsequentRandomization()     {
    std::cout << "Testing subsequent rand runs...";
    BoundingBox bb{ {1,1}, {100,100} };
    const unsigned int SAMPLE_SIZE = 10000;

    RandomCoordinateGenerator rnd1(bb);
    std::set<Coordinates> numbers1;
    for(unsigned int i = 0; i < SAMPLE_SIZE; ++i) {
        auto coord = rnd1.getRandomCoordinate();
        numbers1.insert(coord);

    }
    std::cout << "generated 1...";
    std::this_thread::sleep_for(std::chrono::seconds(2));

    RandomCoordinateGenerator rnd2(bb);
    std::set<Coordinates> numbers2;
    for(unsigned int i = 0; i < SAMPLE_SIZE; ++i) {
        auto coord = rnd2.getRandomCoordinate();
        numbers2.insert(coord);
    }
    std::cout << "generated 2...comparing...\n";

    auto larger = std::max(numbers1.size(), numbers2.size());
    auto smaller = std::min(numbers1.size(), numbers2.size());
    auto differencePercentage = ((larger - smaller) * 100.0) / larger;
    if(differencePercentage < 50.0) {
        std::cout << "  Sizes: " << smaller << " " << larger << std::endl;
        std::vector<Coordinates> common(smaller);
        auto newEnd = std::set_intersection(numbers1.begin(), numbers1.end(),
                                            numbers2.begin(), numbers2.end(),
                                            common.begin());
        common.erase(newEnd, common.end());
        if(common.size() * 100.0 / smaller > 80.0) {
            utils::errorMsg("Number reoccurence too high!");
            std::cout << "  " << common.size() << " common numbers.\n";
            return Result::NOK;
        }
    }

    return Result::OK;
}

Result commandLineArgs() {
    std::cout << "Testing cmd line args (ignore subsequent error msgs)\n";
    if(not input::commandLineArguments(1) && not input::commandLineArguments(2) &
       not input::commandLineArguments(4) && not input::commandLineArguments(5)) {
        utils::errorMsg("cmd validator error for error cases!");
        return Result::NOK;
    }

    if(input::commandLineArguments(3)) {
        utils::errorMsg("cmd validator error for success case!");
        return Result::NOK;
    }

    std::cout << "Done!" << std::endl;
    return Result::OK;
}

Result dimensionTests()  {
    std::cout << "Testing dimensions checker (ignore subsequent error msgs)\n";
    auto failCase = [&](unsigned int x, unsigned int y) {
        auto result = input::dimensions({x,y});
        if(result != Result::NOK)
            utils::errorMsg("Fail case did not fail for ") << x << "-" << y << std::endl;
        return result == Result::NOK;
    };
    auto successCase = [&](unsigned int x, unsigned int y) {
        auto result = input::dimensions({x,y});
        if(result != Result::OK)
            utils::errorMsg("Success case failed for ") << x << "-" << y << std::endl;
        return result == Result::OK;
    };

    auto success = failCase(0,0) && failCase(1,1) && failCase(2,2) && failCase(3,3);
    if(not success) {
        utils::errorMsg("Too small dimensions failed!");
        return Result::NOK;
    }

    success = successCase(3,4) && successCase(3,40) && successCase(4,4) && successCase(340,4111);
    if(not success) {
        utils::errorMsg("Dimensions success cases failed!");
        return Result::NOK;
    }

    std::cout << "Done!" << std::endl;
    return Result::OK;
}

Result runOneHundredMazes() {
    std::cout << "Running 100 mazes...";
    for(int i = 0; i < 100; ++i) {
        std::cout << (i+1) << "...";
        MazeCreator mc({20,20});
        mc.create();
        if(output::noFreeClusters(mc.result()) == Result::NOK) {
            utils::errorMsg("Large free cluster in map!") << std::endl << mc.result();
            return Result::NOK;
        }
        if(output::fullyTraversable(mc) == Result::NOK) {
            utils::errorMsg("Maze not fully traversible!") << std::endl << mc.result();
            return Result::NOK;
        }
    }
    std::cout << "Done!" << std::endl;
    return Result::OK;
}


Result tests() {
    return (   dimensionTests() == Result::OK
            && commandLineArgs() == Result::OK
            && runOneHundredMazes() == Result::OK
            && subsequentRandomization() == Result::OK
            && randDistribution() == Result::OK)
            ? Result::OK : Result::NOK;
}
}

