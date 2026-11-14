
#include "PathPlanner.h"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>

using std::vector;

static vector<vector<int> > emptyGrid(int rows, int cols)
{
    return vector<vector<int> >(
        rows, vector<int>(cols, 0)
    );
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
        {
            assert(result.path[i] == start ||
                   result.path[i] != occupied[j]);
        }

        if (i > 0)
        {
            const int distance =
                std::abs(result.path[i].getRow() -
                         result.path[i-1].getRow()) +
                std::abs(result.path[i].getCol() -
                         result.path[i-1].getCol());

            assert(distance == 1);
        }
    }
}

int main()
{
    int passed = 0;

    // 1. Shortest path on open grid.
    {
        PathPlanner planner(emptyGrid(5, 5));
        PathResult route =
            planner.findPath(Position(0, 0), Position(4, 4));

        assertValidRoute(
            planner, route, Position(0, 0), Position(4, 4)
        );
        assert(route.path.size() == 9);
        ++passed;
    }

    // 2. Navigate around obstacles.
    {
        vector<vector<int> > map = emptyGrid(5, 5);
        map[1][0] = 1;
        map[1][1] = 1;
        map[1][2] = 1;
        map[1][3] = 1;

        PathPlanner planner(map);
        PathResult route =
            planner.findPath(Position(0, 0), Position(4, 0));

        assertValidRoute(
            planner, route, Position(0, 0), Position(4, 0)
        );
        assert(route.path.size() == 13);
        ++passed;
    }

    // 3. Unreachable destination.
    {
        PathPlanner planner(
            vector<vector<int> >{
                {0, 0, 0},
                {1, 1, 1},
                {0, 0, 0}
            }
        );

        PathResult route =
            planner.findPath(Position(0, 0), Position(2, 2));

        assert(route.status == PathStatus::Unreachable);
        assert(route.path.empty());
        ++passed;
    }

    // 4. Invalid coordinates.
    {
        PathPlanner planner(emptyGrid(2, 2));

        assert(planner.findPath(
            Position(-1, 0), Position(1, 1)
        ).status == PathStatus::InvalidStart);

        assert(planner.findPath(
            Position(0, 0), Position(2, 0)
        ).status == PathStatus::InvalidGoal);
        ++passed;
    }

    // 5. Blocked start and goal.
    {
        PathPlanner planner(
            vector<vector<int> >{
                {1, 0},
                {0, 1}
            }
        );

        assert(planner.findPath(
            Position(0, 0), Position(1, 0)
        ).status == PathStatus::BlockedStart);

        assert(planner.findPath(
            Position(1, 0), Position(1, 1)
        ).status == PathStatus::BlockedGoal);
        ++passed;
    }

    // 6. Already at destination.
    {
        PathPlanner planner(emptyGrid(3, 3));
        PathResult route =
            planner.findPath(Position(2, 2), Position(2, 2));

        assert(route.found());
        assert(route.path.size() == 1);
        ++passed;
    }

    // 7. Avoid another robot.
    {
        PathPlanner planner(emptyGrid(3, 3));
        vector<Position> occupied(1, Position(1, 1));

        PathResult route = planner.findPath(
            Position(1, 0), Position(1, 2), occupied
        );

        assertValidRoute(
            planner, route,
            Position(1, 0), Position(1, 2), occupied
        );

        assert(route.path.size() == 5);
        ++passed;
    }

    // 8. Goal occupied by another robot.
    {
        PathPlanner planner(emptyGrid(3, 3));
        vector<Position> occupied(1, Position(1, 2));

        assert(planner.findPath(
            Position(1, 0), Position(1, 2), occupied
        ).status == PathStatus::BlockedGoal);
        ++passed;
    }

    // 9. Robot already occupies its destination.
    {
        PathPlanner planner(emptyGrid(3, 3));
        vector<Position> occupied(1, Position(1, 1));

        PathResult route = planner.findPath(
            Position(1, 1), Position(1, 1), occupied
        );

        assert(route.found());
        assert(route.path.size() == 1);
        ++passed;
    }

    // 10. Boundary checking.
    {
        PathPlanner planner(emptyGrid(3, 3));

        assert(!planner.isInside(Position(0, -1)));
        assert(!planner.isInside(Position(3, 0)));
        assert(!planner.isWalkable(Position(-1, 0)));
        assert(planner.getRows() == 3);
        assert(planner.getCols() == 3);
        ++passed;
    }

    // 11. Invalid integer-grid construction.
    {
        bool caughtEmpty = false;
        try
        {
            PathPlanner planner((vector<vector<int> >()));
        }
        catch (const std::invalid_argument&)
        {
            caughtEmpty = true;
        }
        assert(caughtEmpty);

        bool caughtRagged = false;
        try
        {
            PathPlanner planner(
                vector<vector<int> >{{0, 0}, {0}}
            );
        }
        catch (const std::invalid_argument&)
        {
            caughtRagged = true;
        }
        assert(caughtRagged);
        ++passed;
    }

    // 12. Update the integer grid.
    {
        PathPlanner planner(emptyGrid(2, 2));

        planner.setGrid(
            vector<vector<int> >{{0, 1}, {0, 0}}
        );

        assert(!planner.isWalkable(Position(0, 1)));

        bool caught = false;
        try
        {
            planner.setGrid(
                vector<vector<int> >{{0}, {0, 0}}
            );
        }
        catch (const std::invalid_argument&)
        {
            caught = true;
        }

        assert(caught);
        assert(planner.getCols() == 2);
        assert(!planner.isWalkable(Position(0, 1)));
        ++passed;
    }

    // 13. Larger grid.
    {
        PathPlanner planner(emptyGrid(40, 40));

        PathResult route =
            planner.findPath(Position(0, 0), Position(39, 39));

        assertValidRoute(
            planner, route, Position(0, 0), Position(39, 39)
        );

        assert(route.path.size() == 79);
        ++passed;
    }

    // 14. Another robot blocks the only doorway.
    {
        PathPlanner planner(
            vector<vector<int> >{
                {0, 1, 0},
                {0, 0, 0},
                {0, 1, 0}
            }
        );

        vector<Position> occupied(1, Position(1, 1));

        PathResult route = planner.findPath(
            Position(0, 0), Position(0, 2), occupied
        );

        assert(route.status == PathStatus::Unreachable);
        ++passed;
    }

    // 15. Character grid with unreachable destination.
    {
        const vector<vector<char> > map = {
            {'.', '|', '.', '.'},
            {'P', '|', '.', 'D'},
            {'.', '|', '.', '.'}
        };

        PathPlanner planner(map, '|');

        assert(planner.getRows() == 3);
        assert(planner.getCols() == 4);
        assert(!planner.isWalkable(Position(0, 1)));
        assert(planner.isWalkable(Position(1, 0)));
        assert(planner.isWalkable(Position(1, 3)));

        PathResult route =
            planner.findPath(Position(1, 0), Position(1, 3));

        assert(route.status == PathStatus::Unreachable);
        assert(route.path.empty());
        ++passed;
    }

    // 16. Reachable character-grid path.
    {
        const vector<vector<char> > map = {
            {'P', '.', '.'},
            {'|', '|', '.'},
            {'.', '.', 'D'}
        };

        PathPlanner planner(map, '|');

        PathResult route =
            planner.findPath(Position(0, 0), Position(2, 2));

        assertValidRoute(
            planner, route, Position(0, 0), Position(2, 2)
        );

        assert(route.path.size() == 5);
        ++passed;
    }

    // 17. Update using character grid.
    {
        PathPlanner planner(emptyGrid(2, 2));

        const vector<vector<char> > updated = {
            {'.', '|'},
            {'P', 'D'}
        };

        planner.setGrid(updated, '|');

        assert(!planner.isWalkable(Position(0, 1)));
        assert(planner.isWalkable(Position(1, 0)));

        assert(planner.findPath(
            Position(1, 0), Position(1, 1)
        ).found());

        ++passed;
    }

    // 18. Invalid character-grid update.
    {
        PathPlanner planner(emptyGrid(2, 2));

        bool caught = false;
        try
        {
            planner.setGrid(
                vector<vector<char> >{
                    {'.', '.'},
                    {'.'}
                },
                '|'
            );
        }
        catch (const std::invalid_argument&)
        {
            caught = true;
        }

        assert(caught);
        assert(planner.getCols() == 2);
        assert(planner.isWalkable(Position(0, 1)));
        ++passed;
    }

    // 19. Empty character grid.
    {
        bool caughtEmpty = false;

        try
        {
            PathPlanner planner(
                vector<vector<char> >(), '|'
            );
        }
        catch (const std::invalid_argument&)
        {
            caughtEmpty = true;
        }

        assert(caughtEmpty);
        ++passed;
    }

    // 20. Blocked cells in a character grid.
    {
        const vector<vector<char> > map = {
            {'|', '.'},
            {'.', '|'}
        };

        PathPlanner planner(map, '|');

        assert(planner.findPath(
            Position(0, 0), Position(1, 0)
        ).status == PathStatus::BlockedStart);

        assert(planner.findPath(
            Position(1, 0), Position(1, 1)
        ).status == PathStatus::BlockedGoal);

        ++passed;
    }

    std::cout << "PASS: " << passed
              << " PathPlanner test cases\n";
}
