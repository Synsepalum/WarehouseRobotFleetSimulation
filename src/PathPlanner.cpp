#include "PathPlanner.h"

#include <algorithm>
#include <queue>
#include <stdexcept>
#include <vector>

namespace
{
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

PathPlanner::PathPlanner(const std::vector<std::vector<int> >& grid)
    : rows_(0), cols_(0)
{
    setGrid(grid);
}

void PathPlanner::setGrid(const std::vector<std::vector<int> >& grid)
{
    if (grid.empty() || grid[0].empty())
        throw std::invalid_argument("PathPlanner grid cannot be empty");

    const std::size_t width = grid[0].size();
    for (std::size_t row = 0; row < grid.size(); ++row)
    {
        if (grid[row].size() != width)
            throw std::invalid_argument("PathPlanner grid must be rectangular");
    }

    // Copy only after validation, so an invalid update leaves the old map intact.
    grid_ = grid;
    rows_ = static_cast<int>(grid_.size());
    cols_ = static_cast<int>(width);
}

int PathPlanner::getRows() const
{
    return rows_;
}

int PathPlanner::getCols() const
{
    return cols_;
}

bool PathPlanner::isInside(const Position& position) const
{
    return position.getRow() >= 0 && position.getRow() < rows_
        && position.getCol() >= 0 && position.getCol() < cols_;
}

bool PathPlanner::isWalkable(const Position& position) const
{
    return isInside(position)
        && grid_[position.getRow()][position.getCol()] == 0;
}

PathResult PathPlanner::findPath(
    const Position& start,
    const Position& goal,
    const std::vector<Position>& temporarilyBlocked) const
{
    if (!isInside(start))
        return PathResult{PathStatus::InvalidStart, std::vector<Position>()};
    if (!isInside(goal))
        return PathResult{PathStatus::InvalidGoal, std::vector<Position>()};
    if (!isWalkable(start))
        return PathResult{PathStatus::BlockedStart, std::vector<Position>()};
    if (!isWalkable(goal) || (goal != start && isTemporarilyBlocked(goal, temporarilyBlocked)))
        return PathResult{PathStatus::BlockedGoal, std::vector<Position>()};

    if (start == goal)
        return PathResult{PathStatus::Success, std::vector<Position>(1, start)};

    std::queue<Position> frontier;
    std::vector<std::vector<bool> > visited(
        rows_, std::vector<bool>(cols_, false));
    std::vector<std::vector<Position> > parent(
        rows_, std::vector<Position>(cols_, Position()));

    visited[start.getRow()][start.getCol()] = true;
    frontier.push(start);

    // Fixed order makes equal-length routes deterministic: up, right, down, left.
    const int dRow[4] = {-1, 0, 1, 0};
    const int dCol[4] = {0, 1, 0, -1};
    bool reached = false;

    while (!frontier.empty() && !reached)
    {
        const Position current = frontier.front();
        frontier.pop();

        for (int direction = 0; direction < 4; ++direction)
        {
            const Position next(
                current.getRow() + dRow[direction],
                current.getCol() + dCol[direction]);

            if (!isWalkable(next)
                || visited[next.getRow()][next.getCol()]
                || isTemporarilyBlocked(next, temporarilyBlocked))
                continue;

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

    if (!reached)
        return PathResult{PathStatus::Unreachable, std::vector<Position>()};

    std::vector<Position> reversePath;
    for (Position current = goal; current != start;
         current = parent[current.getRow()][current.getCol()])
    {
        reversePath.push_back(current);
    }
    reversePath.push_back(start);
    std::reverse(reversePath.begin(), reversePath.end());

    return PathResult{PathStatus::Success, reversePath};
}
