#include "Simulation.h"
#include "StandardRobot.h"
#include "HeavyRobot.h"
#include <iostream>
#include <stdexcept>
using namespace std;

//======================================
// Helpers that turn a status into words
//======================================
static string robotStatusText(RobotStatus status)
{
    switch (status)
    {
        case RobotStatus::IDLE:           return "Idle";
        case RobotStatus::MOVING:         return "Moving";
        case RobotStatus::OUT_OF_BATTERY: return "Out of battery";
    }
    return "Unknown";
}

static string taskStatusText(TaskStatus status)
{
    switch (status)
    {
        case TaskStatus::PENDING:   return "Pending";
        case TaskStatus::ASSIGNED:  return "Assigned";
        case TaskStatus::PICKED_UP: return "Picked up";
        case TaskStatus::COMPLETED: return "Completed";
        case TaskStatus::FAILED:    return "Failed";
    }
    return "Unknown";
}

//======================================
// Constructor
// Just sets everything up with reset()
//======================================
Simulation::Simulation()
{
    reset();
}

//======================================
// Starts over: removes all tasks and
// puts 3 fresh robots at the starting spots
//======================================
void Simulation::reset()
{
    robots.clear();
    dispatcher.clear();
    stepCount = 0;
    nextTaskId = 1;

    robots.push_back(make_unique<StandardRobot>(1, Position(0, 0)));
    robots.push_back(make_unique<StandardRobot>(2, Position(0, 1)));
    robots.push_back(make_unique<HeavyRobot>(3, Position(0, 2)));
}

//======================================
// Adds a new task
// Checks the spots and weight first so
// bad input doesn't break anything
//======================================
void Simulation::addTask(const Position& pickup, const Position& dropoff, int weight)
{
    if (!warehouse.isWalkable(pickup))
        throw invalid_argument("Pickup spot is a wall or outside the warehouse.");

    if (!warehouse.isWalkable(dropoff))
        throw invalid_argument("Drop-off spot is a wall or outside the warehouse.");

    if (weight <= 0)
        throw invalid_argument("Weight has to be more than 0.");

    dispatcher.addTask(Task(nextTaskId, pickup, dropoff, weight));
    nextTaskId++;
}

//======================================
// One step of the simulation:
// 1. Give pending tasks to free robots
// 2. Move every busy robot one square
//======================================
void Simulation::step()
{
    stepCount++;
    cout << "--- Step " << stepCount << " ---" << endl;

    dispatcher.assignTasks(robots);

    for (auto& robot : robots)
    {
        updateRobot(*robot);
    }
}

//======================================
// Handles one robot for one step
//======================================
void Simulation::updateRobot(Robot& robot)
{
    // No task? Nothing to do.
    Task* task = dispatcher.findTask(robot.getTaskId());
    if (task == nullptr)
        return;

    // Going to the pickup spot or the drop-off spot?
    Position target;
    if (task->getStatus() == TaskStatus::ASSIGNED)
        target = task->getPickup();
    else
        target = task->getDropoff();

    // Already there: pick up or deliver
    if (robot.getPosition() == target)
    {
        if (task->getStatus() == TaskStatus::ASSIGNED)
        {
            task->setStatus(TaskStatus::PICKED_UP);
            cout << "Robot " << robot.getId() << " picked up task "
                 << task->getId() << endl;
        }
        else
        {
            task->setStatus(TaskStatus::COMPLETED);
            robot.clearTask();
            robot.setStatus(RobotStatus::IDLE);
            cout << "Robot " << robot.getId() << " delivered task "
                 << task->getId() << "!" << endl;
        }
        return;
    }

    // Ask the planner for a route
    vector<Position> path = planner.findPath(warehouse, robot.getPosition(), target);

    // No route means walls block the way
    if (path.empty())
    {
        task->setStatus(TaskStatus::FAILED);
        robot.clearTask();
        robot.setStatus(RobotStatus::IDLE);
        cout << "Task " << task->getId() << " can't be reached. Marked as failed." << endl;
        return;
    }

    // Try to move one square. moveTo() returns false if the battery is too low.
    if (!robot.moveTo(path[0]))
    {
        robot.setStatus(RobotStatus::OUT_OF_BATTERY);
        robot.clearTask();

        // Not picked up yet: put it back so another robot can do it.
        // Already carrying it: the delivery fails.
        if (task->getStatus() == TaskStatus::ASSIGNED)
        {
            task->setStatus(TaskStatus::PENDING);
            cout << "Robot " << robot.getId() << " ran out of battery. Task "
                 << task->getId() << " goes back to pending." << endl;
        }
        else
        {
            task->setStatus(TaskStatus::FAILED);
            cout << "Robot " << robot.getId() << " ran out of battery while carrying task "
                 << task->getId() << ". Task failed." << endl;
        }
        return;
    }

    robot.setStatus(RobotStatus::MOVING);
}

//======================================
// Shows the map
//======================================
void Simulation::displayWarehouse() const
{
    warehouse.display();
}

//======================================
// Shows every robot's spot, battery, and status
//======================================
void Simulation::displayRobots() const
{
    for (const auto& robot : robots)
    {
        cout << "Robot " << robot->getId() << " at ";
        robot->getPosition().display();
        cout << " | Battery: " << robot->getBattery()
             << " | " << robotStatusText(robot->getStatus()) << endl;
    }
}

//======================================
// Shows every task and where it's at
//======================================
void Simulation::displayTasks() const
{
    const vector<Task>& tasks = dispatcher.getTasks();

    if (tasks.empty())
    {
        cout << "No tasks yet." << endl;
        return;
    }

    for (const Task& task : tasks)
    {
        cout << "Task " << task.getId() << ": ";
        task.getPickup().display();
        cout << " -> ";
        task.getDropoff().display();
        cout << " | Weight: " << task.getWeight()
             << " | " << taskStatusText(task.getStatus()) << endl;
    }
}

int Simulation::getStepCount() const
{
    return stepCount;
}
