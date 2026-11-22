
#include "PathPlanner.h"

#include <algorithm>
#include <queue>
#include <stdexcept>
#include <vector>

namespace
{
    // Convert a character-based warehouse map into integers.
    // Walls become 1; every other cell becomes 0.
    std::vector<std::vector<int> > toIntegerGrid(
        const std::vector<std::vector<char> >& charGrid,
        char wallCharacter)
    {
        std::vector<std::vector<int> > result;
        result.reserve(charGrid.size());

        for (std::size_t row = 0; row < charGrid.size(); ++row)
        {
            std::vector<int> convertedRow;
            convertedRow.reserve(charGrid[row].size());

            for (std::size_t col = 0;
                 col < charGrid[row].size(); ++col)
            {
                convertedRow.push_back(
                    charGrid[row][col] == wallCharacter ? 1 : 0
                );
            }

            result.push_back(convertedRow);
        }

        return result;
    }

    bool isTemporarilyBlocked(
        const Position& position,
        const std::vector<Position>& blocked)
    {
        for (std::size_t i = 0; i < blocked.size(); ++i)
        {
            if (position == blocked[i])
                return true;
        }

        return false;
    }
}

// Constructor for integer grids.
PathPlanner::PathPlanner(
    const std::vector<std::vector<int> >& grid)
    : rows_(0), cols_(0)
{
    setGrid(grid);
}

// Constructor for character grids.
PathPlanner::PathPlanner(
    const std::vector<std::vector<char> >& grid,
    char wallCharacter)
    : rows_(0), cols_(0)
{
    setGrid(grid, wallCharacter);
}

// Validate and store an integer grid.
void PathPlanner::setGrid(
    const std::vector<std::vector<int> >& grid)
{
    if (grid.empty() || grid[0].empty())
    {
        throw std::invalid_argument(
            "PathPlanner grid cannot be empty"
        );
    }

    const std::size_t width = grid[0].size();

    for (std::size_t row = 0; row < grid.size(); ++row)
    {
        if (grid[row].size() != width)
        {
            throw std::invalid_argument(
                "PathPlanner grid must be rectangular"
            );
        }
    }

    grid_ = grid;
    rows_ = static_cast<int>(grid_.size());
    cols_ = static_cast<int>(width);
}

// Convert a character grid and pass it to the integer version.
void PathPlanner::setGrid(
    const std::vector<std::vector<char> >& grid,
    char wallCharacter)
{
    setGrid(toIntegerGrid(grid, wallCharacter));
}

int PathPlanner::getRows() const
{
    return rows_;
}

int PathPlanner::getCols() const
{
    return cols_;
}

// Check if a position is inside warehouse boundaries.
bool PathPlanner::isInside(const Position& position) const
{
    return position.getRow() >= 0 &&
           position.getRow() < rows_ &&
           position.getCol() >= 0 &&
           position.getCol() < cols_;
}

// Check if a cell is accessible.
bool PathPlanner::isWalkable(const Position& position) const
{
    return isInside(position) &&
           grid_[position.getRow()][position.getCol()] == 0;
}

// Find the shortest path between two positions using BFS.
PathResult PathPlanner::findPath(
    const Position& start,
    const Position& goal,
    const std::vector<Position>& temporarilyBlocked) const
{
    // Validate starting and ending positions.
    if (!isInside(start))
    {
        return PathResult{
            PathStatus::InvalidStart,
            std::vector<Position>()
        };
    }

    if (!isInside(goal))
    {
        return PathResult{
            PathStatus::InvalidGoal,
            std::vector<Position>()
        };
    }

    if (!isWalkable(start))
    {
        return PathResult{
            PathStatus::BlockedStart,
            std::vector<Position>()
        };
    }

    if (!isWalkable(goal) ||
        (goal != start &&
         isTemporarilyBlocked(goal, temporarilyBlocked)))
    {
        return PathResult{
            PathStatus::BlockedGoal,
            std::vector<Position>()
        };
    }

    // Already at the destination.
    if (start == goal)
    {
        return PathResult{
            PathStatus::Success,
            std::vector<Position>(1, start)
        };
    }

    std::queue<Position> frontier;

    std::vector<std::vector<bool> > visited(
        rows_, std::vector<bool>(cols_, false)
    );

    std::vector<std::vector<Position> > parent(
        rows_, std::vector<Position>(cols_, Position())
    );

    frontier.push(start);
    visited[start.getRow()][start.getCol()] = true;

    // Movement directions: up, right, down, left.
    const int dRow[4] = {-1, 0, 1, 0};
    const int dCol[4] = {0, 1, 0, -1};

    bool reached = false;

    while (!frontier.empty() && !reached)
    {
        Position current = frontier.front();
        frontier.pop();

        for (int direction = 0; direction < 4; ++direction)
        {
            Position next(
                current.getRow() + dRow[direction],
                current.getCol() + dCol[direction]
            );

            if (!isWalkable(next) ||
                visited[next.getRow()][next.getCol()] ||
                isTemporarilyBlocked(next, temporarilyBlocked))
            {
                continue;
            }

            visited[next.getRow()][next.getCol()] = true;

            parent[next.getRow()][next.getCol()] = current;

            if (next == goal)
            {
                reached = true;
                break;
            }

            frontier.push(next);
        }
    }

    // The destination cannot be reached.
    if (!reached)
    {
        return PathResult{
            PathStatus::Unreachable,
            std::vector<Position>()
        };
    }

    // Reconstruct the shortest path, working backward.
    std::vector<Position> reversePath;

    for (Position current = goal;
         current != start;
         current = parent[current.getRow()][current.getCol()])
    {
        reversePath.push_back(current);
    }

    reversePath.push_back(start);

    std::reverse(reversePath.begin(), reversePath.end());

    return PathResult{
        PathStatus::Success,
        reversePath
    };
}
