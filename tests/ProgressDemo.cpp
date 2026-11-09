// ProgressDemo.cpp
// Temporary demo for the progress check-in.
// Connects the pieces that are finished so far:
//   Position, Robot / StandardRobot / HeavyRobot, Task, Dispatcher, PathPlanner
// Warehouse and Simulation aren't done yet, so this file does a simple
// version of their job. It will be replaced by the real Simulation later.

#include "StandardRobot.h"
#include "HeavyRobot.h"
#include "Dispatcher.h"
#include "PathPlanner.h"
#include <iostream>
#include <memory>
#include <vector>
#include <string>
using namespace std;

// Map: 0 = open floor, 1 = wall (obstacle)
// Cell (4, 6) is boxed in by walls, so it's unreachable on purpose.
vector<vector<int>> makeGrid()
{
    return {
        {0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 0, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 1, 0, 1, 1},
        {0, 0, 0, 1, 0, 1, 0}
    };
}

// Turns a status into words for printing
string statusText(RobotStatus s)
{
    switch (s)
    {
        case RobotStatus::IDLE:           return "Idle";
        case RobotStatus::MOVING:         return "Moving";
        case RobotStatus::OUT_OF_BATTERY: return "Out of battery";
    }
    return "Unknown";
}

string taskText(TaskStatus s)
{
    switch (s)
    {
        case TaskStatus::PENDING:   return "Pending";
        case TaskStatus::ASSIGNED:  return "Assigned";
        case TaskStatus::PICKED_UP: return "Picked up";
        case TaskStatus::COMPLETED: return "Completed";
        case TaskStatus::FAILED:    return "Failed";
    }
    return "Unknown";
}

// Prints the map: # = wall, . = floor, number = robot id
void printMap(const vector<vector<int>>& grid, const vector<unique_ptr<Robot>>& robots)
{
    for (int r = 0; r < (int)grid.size(); r++)
    {
        for (int c = 0; c < (int)grid[r].size(); c++)
        {
            char symbol = (grid[r][c] == 0) ? '.' : '#';
            for (const auto& robot : robots)
            {
                if (robot->getPosition() == Position(r, c))
                    symbol = '0' + robot->getId();
            }
            cout << symbol << ' ';
        }
        cout << endl;
    }
}

void printStatus(const vector<unique_ptr<Robot>>& robots, const Dispatcher& dispatcher)
{
    for (const auto& robot : robots)
    {
        cout << "  Robot " << robot->getId() << " at ";
        robot->getPosition().display();
        cout << " | Battery: " << robot->getBattery()
             << " | " << statusText(robot->getStatus());
        if (robot->getTaskId() != Robot::NO_TASK)
            cout << " | Task " << robot->getTaskId();
        cout << endl;
    }
    for (const Task& task : dispatcher.getTasks())
    {
        cout << "  Task " << task.getId() << " (weight " << task.getWeight() << "): "
             << taskText(task.getStatus()) << endl;
    }
}

// One robot, one step: pick up, deliver, or move one square toward the target
void updateRobot(Robot& robot, Dispatcher& dispatcher, const PathPlanner& planner)
{
    Task* task = dispatcher.findTask(robot.getTaskId());
    if (task == nullptr)
        return;   // no task, nothing to do

    // Heading to the pickup spot first, then the drop-off spot
    Position target = (task->getStatus() == TaskStatus::ASSIGNED)
                      ? task->getPickup() : task->getDropoff();

    if (robot.getPosition() == target)
    {
        if (task->getStatus() == TaskStatus::ASSIGNED)
        {
            task->setStatus(TaskStatus::PICKED_UP);
            cout << "  -> Robot " << robot.getId() << " picked up task " << task->getId() << endl;
        }
        else
        {
            task->setStatus(TaskStatus::COMPLETED);
            robot.clearTask();
            robot.setStatus(RobotStatus::IDLE);
            cout << "  -> Robot " << robot.getId() << " delivered task " << task->getId() << "!" << endl;
        }
        return;
    }

    PathResult result = planner.findPath(robot.getPosition(), target);

    // Edge case: no route (walls in the way)
    if (!result.found())
    {
        task->setStatus(TaskStatus::FAILED);
        robot.clearTask();
        robot.setStatus(RobotStatus::IDLE);
        cout << "  -> Task " << task->getId() << " can't be reached. Marked as failed." << endl;
        return;
    }

    // path[0] is where the robot is now, so path[1] is the next square
    if (!robot.moveTo(result.path[1]))
    {
        // Edge case: battery too low. Robot stops; task goes back or fails.
        robot.setStatus(RobotStatus::OUT_OF_BATTERY);
        robot.clearTask();
        if (task->getStatus() == TaskStatus::ASSIGNED)
            task->setStatus(TaskStatus::PENDING);
        else
            task->setStatus(TaskStatus::FAILED);
        cout << "  -> Robot " << robot.getId() << " ran out of battery." << endl;
        return;
    }

    robot.setStatus(RobotStatus::MOVING);
}

int main()
{
    vector<vector<int>> grid = makeGrid();
    PathPlanner planner(grid);

    // 3 robots, stored exactly like the real Simulation will store them
    vector<unique_ptr<Robot>> robots;
    robots.push_back(make_unique<StandardRobot>(1, Position(0, 0)));
    robots.push_back(make_unique<StandardRobot>(2, Position(2, 0)));
    robots.push_back(make_unique<HeavyRobot>(3, Position(4, 0)));

    Dispatcher dispatcher;
    dispatcher.addTask(Task(1, Position(0, 3), Position(2, 6), 5));    // light: a Standard robot takes it
    dispatcher.addTask(Task(2, Position(2, 2), Position(4, 4), 30));   // heavy: only the HeavyRobot can carry it
    dispatcher.addTask(Task(3, Position(0, 6), Position(4, 6), 4));    // drop-off is walled in: unreachable
    dispatcher.addTask(Task(4, Position(2, 3), Position(0, 0), 80));   // too heavy for every robot: stays pending

    cout << "===== Start =====" << endl;
    printMap(grid, robots);
    printStatus(robots, dispatcher);

    const int MAX_STEPS = 15;
    for (int step = 1; step <= MAX_STEPS; step++)
    {
        cout << endl << "===== Step " << step << " =====" << endl;
        dispatcher.assignTasks(robots);      // Jeremy's Dispatcher hands out tasks
        for (auto& robot : robots)
            updateRobot(*robot, dispatcher, planner);
        printMap(grid, robots);
        printStatus(robots, dispatcher);

        // Stop early once no robot has work left
        bool anyBusy = false;
        for (const auto& robot : robots)
            if (robot->getTaskId() != Robot::NO_TASK)
                anyBusy = true;
        if (!anyBusy && step > 1)
            break;
    }
    return 0;
}
