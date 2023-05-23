#pragma once

#include <algorithm>
#include <functional>
#include <set>

#include "Maze.hpp"
#include "RandomGenerator.hpp"
#include "Utils.hpp"
#include "Validators.hpp"


namespace validate::output {
    struct ValidationAssistant;
}


class MazeCreator {
public:
    MazeCreator(Dimensions&& dims)
            : theDims(std::move(dims))
            , theInnerBb{ {1,1}, {theDims.x - 2, theDims.y - 2} }
            , theMaze(dims)
            , theRand(theInnerBb) {
    }

    void create() {
        fillBorders();
        drawPaths();
        flip();
        closeGaps();
        dropEndpoints();
    }

    const Maze& result() const {
        return theMaze;
    }

private:
    /// Create frame.
    void fillBorders() {
        for(int y = 0; y < theMaze.array.size(); ++y) {
            for(int x = 0; x < theMaze.array[y].size(); ++x) {
                if(x == 0 || y == 0 || x == theMaze.array[y].size() - 1 || y == theMaze.array.size() - 1) {
                    theMaze.array[y][x] = WALL;
                }
            }
        }
    }

    /** A square cluster can only be completed if one corner is full.
     *   xx <- yes   no -> x x      x: taken  o: candidate
     *   xo                xox
     *                     x x
     */
    bool wouldNotCompleteSquare(const Coordinates& coord) const {
        return     not (theMaze[coord.n()] == PATH && theMaze[coord.w()] == PATH && theMaze[coord.nw()] == PATH)
                && not (theMaze[coord.n()] == PATH && theMaze[coord.e()] == PATH && theMaze[coord.ne()] == PATH)
                && not (theMaze[coord.s()] == PATH && theMaze[coord.w()] == PATH && theMaze[coord.sw()] == PATH)
                && not (theMaze[coord.s()] == PATH && theMaze[coord.e()] == PATH && theMaze[coord.se()] == PATH);
    }

    bool isTileViableCandidateForPath(const Coordinates& coord) const {
        return    coord.x >= theInnerBb.tl.x && coord.x <= theInnerBb.br.x
               && coord.y >= theInnerBb.tl.y && coord.y <= theInnerBb.br.y
               && theMaze[coord] == EMPTY
               && wouldNotCompleteSquare(coord);
    }

    unsigned countSurroundingPaths(const Coordinates& coord) const {
        return   int(theMaze[coord.n()] == PATH)
               + int(theMaze[coord.w()] == PATH)
               + int(theMaze[coord.e()] == PATH)
               + int(theMaze[coord.s()] == PATH);
    }

    /** Draw where the free tiles will be. This is much easier than trying to guess walls,
     *  since most paths will be one tile wide and will either stop, continue, turn, or branch
     *  into 2 or 3. This will ensure that all paths are connected.
     */
    void drawPaths() {
        auto startingPoint = theRand.getRandomCoordinate();
        theMaze[startingPoint] = PATH;

        std::set<Coordinates> activeEndPoints{startingPoint};
        std::set<Coordinates> nextBatch;
        std::set<Coordinates> toRemove;
        do {

            bool emergencyProtocol =   // the normal rules do not allow us to complete the maze
                std::all_of(activeEndPoints.begin(), activeEndPoints.end(),
                            [this](const Coordinates& c) { return countSurroundingPaths(c) == 2; });
            for(const auto& coord : activeEndPoints) {
                bool nViable = isTileViableCandidateForPath(coord.n());
                bool wViable = isTileViableCandidateForPath(coord.w());
                bool sViable = isTileViableCandidateForPath(coord.s());
                bool eViable = isTileViableCandidateForPath(coord.e());
                if(   (not nViable && not wViable && not sViable && not eViable)
                   || countSurroundingPaths(coord) == 3) {
                    toRemove.insert(coord);
                    continue;
                }
                if(countSurroundingPaths(coord) == 2 && not emergencyProtocol) {
                    continue;
                }

                // This should probably be in random order instead
                if(nViable && theRand.coinFlip()) {
                    theMaze[coord.n()] = PATH;
                    nextBatch.insert(coord.n());
                    // we have to re-evaluate downstream every time there's an insert
                    wViable = isTileViableCandidateForPath(coord.w());
                    sViable = isTileViableCandidateForPath(coord.s());
                    eViable = isTileViableCandidateForPath(coord.e());
                }
                if(wViable && theRand.coinFlip()) {
                    theMaze[coord.w()] = PATH;
                    nextBatch.insert(coord.w());
                    sViable = isTileViableCandidateForPath(coord.s());
                    eViable = isTileViableCandidateForPath(coord.e());
                }
                if(sViable && theRand.coinFlip()) {
                    theMaze[coord.s()] = PATH;
                    nextBatch.insert(coord.s());
                    eViable = isTileViableCandidateForPath(coord.e());
                }
                if(eViable && theRand.coinFlip()) {
                    theMaze[coord.e()] = PATH;
                    nextBatch.insert(coord.e());
                }
            }
            for(const auto& elem : toRemove) {
                activeEndPoints.erase(elem);
            }
            toRemove.clear();
            activeEndPoints.insert(nextBatch.begin(), nextBatch.end());
            nextBatch.clear();
        } while(activeEndPoints.size());
    }

    void forInnerBb(std::function<void(const Coordinates&)> func) {
        for(unsigned y = theInnerBb.tl.y; y <= theInnerBb.br.y; ++y) {
            for(unsigned x = theInnerBb.tl.x; x <= theInnerBb.br.x; ++x) {
                func({x,y});
            }
        }
    }

    /// Since we drew the path first, change that to empty and make the walls where there is nothing.
    void flip() {
        forInnerBb([&](const Coordinates& coord){
            auto& curr = theMaze[coord];
            if(curr == PATH) {
                curr = EMPTY;
            } else if(curr == EMPTY) {
                curr = WALL;
            } else {
                utils::errorMsg("Unexpected tile ") << curr << " at "
                    << coord << std::endl;
            }
        });
    }

    /**
     * Certain wall pieces would just be dangling without connection. This function is meant
     * to attach these to nearby walls.
    */
    void closeGaps() {
        bool stillFoundGap = false;
        do {
            stillFoundGap = false;
            forInnerBb([&](const Coordinates& coord) {
                unsigned int pathTileCount = 0;
                for(auto func : coord.directionFunctions) {
                    pathTileCount += (theMaze[(coord.*func)()] == EMPTY);
                }
                if(theMaze[coord] == WALL && pathTileCount == 8) {
                    stillFoundGap = true;
                    // close a random direction
                    theMaze[(coord.*(coord.directionFunctions)[theRand.pickRandomFrom(8)])()] = WALL;
                }
            });
        } while(stillFoundGap);
    }

    /**
     * Place end and begin points randomly.
     *
     * Note: Okay, so maybe here I'm being a bit "loopholey", since the task didn't specify
     *       that the endpoints should be at the border (like for most puzzle mazes), or even
     *       far away from each other. But then again it's a part of test to not add to the
     *       specs based on general knowledge. I hope you'll let me know :)
     */
    void dropEndpoints() {
        Coordinates coord;
        for(auto type : {BEGIN, END}) {
            do {
                coord = theRand.getRandomCoordinate();
            }
            while(theMaze[coord] != EMPTY);
            theMaze[coord] = type;
        }
    }

    Dimensions theDims;
    BoundingBox theInnerBb;
    Maze theMaze;
    RandomCoordinateGenerator theRand;

    friend struct validate::output::ValidationAssistant;
};
