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
    std::vector<Position> path; // Includes the start and goal on success.

    bool found() const { return status == PathStatus::Success; }
};

class PathPlanner
{
public:
    // The planner owns a copy of the grid, so the caller can modify its grid safely.
    // Throws std::invalid_argument for an empty or nonrectangular grid.
    explicit PathPlanner(const std::vector<std::vector<int> >& grid);

    // Refresh the map when fixed obstacles change.
    // Throws std::invalid_argument for an empty or nonrectangular grid.
    void setGrid(const std::vector<std::vector<int> >& grid);

    int getRows() const;
    int getCols() const;
    bool isInside(const Position& position) const;
    bool isWalkable(const Position& position) const;

    // Breadth-first search: shortest route in number of 4-direction moves.
    // temporarilyBlocked can represent other robots' CURRENT positions.
    // The start is exempt from temporary blocking because a robot occupies it.
    // A temporarily blocked goal is treated as blocked.
    // Never moves diagonally and never mutates the map.
    PathResult findPath(
        const Position& start,
        const Position& goal,
        const std::vector<Position>& temporarilyBlocked = std::vector<Position>()
    ) const;

private:
    std::vector<std::vector<int> > grid_;
    int rows_;
    int cols_;
};

#endif // PATH_PLANNER_H
