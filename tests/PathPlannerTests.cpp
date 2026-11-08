#include "PathPlanner.h"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>

using std::vector;

static vector<vector<int> > emptyGrid(int rows, int cols)
{
    return vector<vector<int> >(rows, vector<int>(cols, 0));
}

static void assertValidRoute(
    const PathPlanner& planner,
    const PathResult& result,
    const Position& start,
    const Position& goal,
    const vector<Position>& occupied = vector<Position>())
{
    assert(result.found());
    assert(!result.path.empty());
    assert(result.path.front() == start);
    assert(result.path.back() == goal);
    for (std::size_t i = 0; i < result.path.size(); ++i)
    {
        assert(planner.isWalkable(result.path[i]));
        for (std::size_t j = 0; j < occupied.size(); ++j)
            assert(result.path[i] == start || result.path[i] != occupied[j]);
        if (i > 0)
        {
            const int distance = std::abs(result.path[i].getRow() - result.path[i-1].getRow())
                + std::abs(result.path[i].getCol() - result.path[i-1].getCol());
            assert(distance == 1); // No teleporting or diagonal movement.
        }
    }
}

int main()
{
    int passed = 0;
    {
        PathPlanner planner(emptyGrid(5, 5));
        const PathResult route = planner.findPath(Position(0, 0), Position(4, 4));
        assertValidRoute(planner, route, Position(0, 0), Position(4, 4));
        assert(route.path.size() == 9); // 8 moves, shortest possible.
        ++passed;
    }
    {
        vector<vector<int> > map = emptyGrid(5, 5);
        map[1][0] = 1;
        map[1][1] = 1;
        map[1][2] = 1;
        map[1][3] = 1;
        PathPlanner planner(map);
        PathResult route = planner.findPath(Position(0, 0), Position(4, 0));
        assertValidRoute(planner, route, Position(0, 0), Position(4, 0));
        assert(route.path.size() == 13); // 12 moves around barrier.
        ++passed;
    }
    {
        PathPlanner planner(vector<vector<int> >{{0, 0, 0}, {1, 1, 1}, {0, 0, 0}});
        PathResult route = planner.findPath(Position(0, 0), Position(2, 2));
        assert(route.status == PathStatus::Unreachable && route.path.empty());
        ++passed;
    }
    {
        PathPlanner planner(emptyGrid(2, 2));
        assert(planner.findPath(Position(-1, 0), Position(1, 1)).status == PathStatus::InvalidStart);
        assert(planner.findPath(Position(0, 0), Position(2, 0)).status == PathStatus::InvalidGoal);
        ++passed;
    }
    {
        PathPlanner planner(vector<vector<int> >{{1, 0}, {0, 1}});
        assert(planner.findPath(Position(0, 0), Position(1, 0)).status == PathStatus::BlockedStart);
        assert(planner.findPath(Position(1, 0), Position(1, 1)).status == PathStatus::BlockedGoal);
        ++passed;
    }
    {
        PathPlanner planner(emptyGrid(3, 3));
        PathResult route = planner.findPath(Position(2, 2), Position(2, 2));
        assert(route.found() && route.path.size() == 1 && route.path[0] == Position(2, 2));
        ++passed;
    }
    {
        PathPlanner planner(emptyGrid(3, 3));
        vector<Position> occupied(1, Position(1, 1));
        PathResult route = planner.findPath(Position(1, 0), Position(1, 2), occupied);
        assertValidRoute(planner, route, Position(1, 0), Position(1, 2), occupied);
        assert(route.path.size() == 5); // Routes around another robot.
        ++passed;
    }
    {
        PathPlanner planner(emptyGrid(3, 3));
        vector<Position> occupied(1, Position(1, 2));
        assert(planner.findPath(Position(1, 0), Position(1, 2), occupied).status == PathStatus::BlockedGoal);
        ++passed;
    }
    {
        PathPlanner planner(emptyGrid(3, 3));
        vector<Position> occupied(1, Position(1, 1));
        PathResult route = planner.findPath(Position(1, 1), Position(1, 1), occupied);
        assert(route.found() && route.path.size() == 1);
        ++passed;
    }
    {
        PathPlanner planner(emptyGrid(3, 3));
        assert(!planner.isInside(Position(0, -1)));
        assert(!planner.isInside(Position(3, 0)));
        assert(!planner.isWalkable(Position(-1, 0)));
        assert(planner.getRows() == 3 && planner.getCols() == 3);
        ++passed;
    }
    {
        bool caughtEmpty = false;
        try { PathPlanner planner((vector<vector<int> >())); }
        catch (const std::invalid_argument&) { caughtEmpty = true; }
        assert(caughtEmpty);
        bool caughtRagged = false;
        try { PathPlanner planner(vector<vector<int> >{{0, 0}, {0}}); }
        catch (const std::invalid_argument&) { caughtRagged = true; }
        assert(caughtRagged);
        ++passed;
    }
    {
        PathPlanner planner(emptyGrid(2, 2));
        vector<vector<int> > updated{{0, 1}, {0, 0}};
        planner.setGrid(updated);
        assert(!planner.isWalkable(Position(0, 1)));
        bool caught = false;
        try { planner.setGrid(vector<vector<int> >{{0}, {0, 0}}); }
        catch (const std::invalid_argument&) { caught = true; }
        assert(caught && planner.getCols() == 2 && !planner.isWalkable(Position(0, 1)));
        ++passed;
    }
    {
        PathPlanner planner(emptyGrid(40, 40));
        PathResult route = planner.findPath(Position(0, 0), Position(39, 39));
        assertValidRoute(planner, route, Position(0, 0), Position(39, 39));
        assert(route.path.size() == 79);
        ++passed;
    }
    {
        // If only one doorway connects two rooms and another robot occupies it,
        // the destination is unreachable (without multi-robot coordination).
        PathPlanner planner(vector<vector<int> >{{0, 1, 0}, {0, 0, 0}, {0, 1, 0}});
        const vector<Position> occupied(1, Position(1, 1));
        PathResult route = planner.findPath(Position(0, 0), Position(0, 2), occupied);
        assert(route.status == PathStatus::Unreachable);
        ++passed;
    }
    std::cout << "PASS: " << passed << " PathPlanner test cases\n";
}
