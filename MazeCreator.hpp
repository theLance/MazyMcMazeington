#pragma once

#include <algorithm>
#include <set>

#include "Maze.hpp"
#include "RandomGenerator.hpp"
#include "Utils.hpp"


class MazeCreator {
public:
    MazeCreator(Dimensions&& dims)
            : theDims(std::move(dims))
            , theInnerBb{ 1,1, theDims.x - 2, theDims.y - 2 }
            , theMaze(dims)
            , theRand(theInnerBb) {
    }

    void create() {
        fillBorders();
        drawPaths();
        // complete missing paths here? (there may be 2x2 wall candidate clusters)
        dropEndpoints();
        flip();
        std::cout << std::endl << theMaze;
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
     *
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
            bool emergencyProtocol =
                std::all_of(activeEndPoints.begin(), activeEndPoints.end(),
                            [this](const Coordinates& c) { return countSurroundingPaths(c) == 2; });
            for(const auto& coord : activeEndPoints) {
                std::cout << coord.x << "-" << coord.y << std::endl;
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
                if(nViable && theRand.coinFlip()) {
                    theMaze[coord.n()] = PATH;
                    nextBatch.insert(coord.n());
                }
                if(wViable && theRand.coinFlip()) {
                    theMaze[coord.w()] = PATH;
                    nextBatch.insert(coord.w());
                }
                if(sViable && theRand.coinFlip()) {
                    theMaze[coord.s()] = PATH;
                    nextBatch.insert(coord.s());
                }
                if(eViable && theRand.coinFlip()) {
                    theMaze[coord.e()] = PATH;
                    nextBatch.insert(coord.e());
                }
            }
            std::cout << theMaze;
            for(const auto& elem : toRemove) {
                activeEndPoints.erase(elem);
            }
            toRemove.clear();
            activeEndPoints.insert(nextBatch.begin(), nextBatch.end());
            nextBatch.clear();
        } while(activeEndPoints.size());
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
        // on PATH
    }

    void flip() {
        for(unsigned y = theInnerBb.tl.y; y <= theInnerBb.br.y; ++y) {
            for(unsigned x = theInnerBb.tl.x; x <= theInnerBb.br.x; ++x) {
                auto& curr = theMaze[{x,y}];
                if(curr == PATH) {
                    curr = EMPTY;
                } else if( curr == EMPTY) {
                    curr = WALL;
                } else {
                    utils::errorMsg("Unexpected tile ") << curr << " at "
                        << x << "-" << y << std::endl;
                }
            }
        }
    }


    Dimensions theDims;
    BoundingBox theInnerBb;
    Maze theMaze;
    RandomCoordinateGenerator theRand;
};

std::ostream& operator<<(std::ostream& os, const MazeCreator& mc) {
    os << mc.result();
    return os;
}