
#ifndef PATH_PLANNER_H
#define PATH_PLANNER_H

#include "Position.h"
#include <vector>

// A grid uses 0 for a walkable cell and any other value for a fixed obstacle.
// Coordinates use Position(row, col), with (0, 0) at the top-left.

enum class PathStatus
{
    Success,
    InvalidStart,
    InvalidGoal,
    BlockedStart,
    BlockedGoal,
    Unreachable
};

struct PathResult
{
    PathStatus status;
    std::vector<Position> path; // Includes start and goal on success.

    bool found() const { return status == PathStatus::Success; }
};

class PathPlanner
{
public:
    // Integer grid: 0 = open, nonzero = obstacle.
    explicit PathPlanner(const std::vector<std::vector<int> >& grid);

    // Character grid: wallCharacter is blocked; others are walkable.
    PathPlanner(const std::vector<std::vector<char> >& grid,
                char wallCharacter);

    // Update the warehouse layout.
    void setGrid(const std::vector<std::vector<int> >& grid);

    void setGrid(const std::vector<std::vector<char> >& grid,
                 char wallCharacter);

    int getRows() const;
    int getCols() const;

    bool isInside(const Position& position) const;
    bool isWalkable(const Position& position) const;

    // Find the shortest path using BFS.
    // Optional occupied positions can represent other robots.
    PathResult findPath(
        const Position& start,
        const Position& goal,
        const std::vector<Position>& temporarilyBlocked =
            std::vector<Position>()
    ) const;

private:
    std::vector<std::vector<int> > grid_;
    int rows_;
    int cols_;
};

#endif // PATH_PLANNER_H
