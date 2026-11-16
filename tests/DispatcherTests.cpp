#include "Dispatcher.h"
#include "StandardRobot.h"
#include "HeavyRobot.h"
#include <iostream>
#include <memory>
#include <vector>
#include <string>
using namespace std;

// Simple test counter: prints PASS or FAIL for each check
int passed = 0;
int failed = 0;

void check(bool condition, const string& testName)
{
    if (condition)
    {
        cout << "[PASS] " << testName << endl;
        passed++;
    }
    else
    {
        cout << "[FAIL] " << testName << endl;
        failed++;
    }
}

// Adding a task and finding it by ID
void testAddAndFind()
{
    Dispatcher d;
    d.addTask(Task(1, Position(0, 0), Position(0, 3), 5));
    d.addTask(Task(2, Position(1, 0), Position(1, 3), 5));

    check(d.getTasks().size() == 2, "Two tasks were added");
    check(d.findTask(2) != nullptr, "findTask finds task 2");
    check(d.findTask(2)->getId() == 2, "findTask returns the right task");
    check(d.findTask(99) == nullptr, "findTask returns nullptr for a bad ID");
    check(d.findTask(Robot::NO_TASK) == nullptr, "findTask returns nullptr for -1 (no task)");
}

// A new task starts pending, then becomes assigned
void testBasicAssign()
{
    Dispatcher d;
    vector<unique_ptr<Robot>> robots;
    robots.push_back(make_unique<StandardRobot>(1, Position(0, 0)));

    d.addTask(Task(1, Position(0, 1), Position(0, 3), 5));
    check(d.findTask(1)->getStatus() == TaskStatus::PENDING, "New task starts PENDING");

    d.assignTasks(robots);
    check(d.findTask(1)->getStatus() == TaskStatus::ASSIGNED, "Task becomes ASSIGNED");
    check(robots[0]->getTaskId() == 1, "Robot now has task 1");
}

// Polymorphism: a heavy task skips the StandardRobot and goes to the HeavyRobot
void testHeavyTask()
{
    Dispatcher d;
    vector<unique_ptr<Robot>> robots;
    robots.push_back(make_unique<StandardRobot>(1, Position(0, 0)));
    robots.push_back(make_unique<HeavyRobot>(2, Position(0, 1)));

    d.addTask(Task(1, Position(0, 2), Position(0, 4), 30));   // too heavy for Standard (max 10)
    d.assignTasks(robots);

    check(robots[0]->getTaskId() == Robot::NO_TASK, "StandardRobot did not get the heavy task");
    check(robots[1]->getTaskId() == 1, "HeavyRobot got the heavy task");
}

// Edge case: no robot can carry it, so it stays pending
void testTooHeavyForEveryone()
{
    Dispatcher d;
    vector<unique_ptr<Robot>> robots;
    robots.push_back(make_unique<StandardRobot>(1, Position(0, 0)));

    d.addTask(Task(1, Position(0, 1), Position(0, 3), 30));
    d.assignTasks(robots);

    check(d.findTask(1)->getStatus() == TaskStatus::PENDING, "Too-heavy task stays PENDING");
    check(robots[0]->getTaskId() == Robot::NO_TASK, "Robot stays without a task");
}

// One robot should never get two tasks
void testOneTaskPerRobot()
{
    Dispatcher d;
    vector<unique_ptr<Robot>> robots;
    robots.push_back(make_unique<StandardRobot>(1, Position(0, 0)));

    d.addTask(Task(1, Position(0, 1), Position(0, 3), 5));
    d.addTask(Task(2, Position(1, 1), Position(1, 3), 5));
    d.assignTasks(robots);

    check(d.findTask(1)->getStatus() == TaskStatus::ASSIGNED, "First task is ASSIGNED");
    check(d.findTask(2)->getStatus() == TaskStatus::PENDING, "Second task stays PENDING (robot is busy)");
    check(robots[0]->getTaskId() == 1, "Robot kept its first task");
}

// Tasks are given out in the order they were added
void testOrder()
{
    Dispatcher d;
    vector<unique_ptr<Robot>> robots;
    robots.push_back(make_unique<StandardRobot>(1, Position(0, 0)));
    robots.push_back(make_unique<StandardRobot>(2, Position(0, 1)));

    d.addTask(Task(1, Position(0, 2), Position(0, 4), 5));
    d.addTask(Task(2, Position(1, 2), Position(1, 4), 5));
    d.assignTasks(robots);

    check(robots[0]->getTaskId() == 1, "Robot 1 gets task 1");
    check(robots[1]->getTaskId() == 2, "Robot 2 gets task 2");
}

// Battery edge case: a robot without enough battery for the trip is skipped
void testLowBattery()
{
    Dispatcher d;
    vector<unique_ptr<Robot>> robots;
    robots.push_back(make_unique<StandardRobot>(1, Position(0, 0)));

    // Drain battery from 100 down to 5 (Standard uses 1 per step)
    for (int i = 0; i < 95; i++)
        robots[0]->moveTo(Position(0, 0));
    check(robots[0]->getBattery() == 5, "Robot battery drained to 5");

    // Trip is 10 + 10 = 20 steps, but the robot only has 5 battery
    d.addTask(Task(1, Position(0, 10), Position(0, 20), 5));
    d.assignTasks(robots);
    check(d.findTask(1)->getStatus() == TaskStatus::PENDING, "Long task stays PENDING (not enough battery)");

    // Trip is 1 + 2 = 3 steps, which 5 battery can cover
    d.addTask(Task(2, Position(0, 1), Position(0, 3), 5));
    d.assignTasks(robots);
    check(robots[0]->getTaskId() == 2, "Robot takes the short task it can finish");
}

// HeavyRobot needs twice the battery for the same trip
void testHeavyBattery()
{
    Dispatcher d;
    vector<unique_ptr<Robot>> robots;
    robots.push_back(make_unique<HeavyRobot>(1, Position(0, 0)));

    // Drain battery from 100 down to 10 (Heavy uses 2 per step)
    for (int i = 0; i < 45; i++)
        robots[0]->moveTo(Position(0, 0));
    check(robots[0]->getBattery() == 10, "Heavy battery drained to 10");

    // 6 steps x 2 = 12 battery needed, but it only has 10
    d.addTask(Task(1, Position(0, 3), Position(0, 6), 5));
    d.assignTasks(robots);
    check(robots[0]->getTaskId() == Robot::NO_TASK, "Heavy robot skips a trip it can't afford");
}

// A robot that is out of battery never gets a task
void testOutOfBatteryRobot()
{
    Dispatcher d;
    vector<unique_ptr<Robot>> robots;
    robots.push_back(make_unique<StandardRobot>(1, Position(0, 0)));
    robots[0]->setStatus(RobotStatus::OUT_OF_BATTERY);

    d.addTask(Task(1, Position(0, 1), Position(0, 2), 5));
    d.assignTasks(robots);

    check(robots[0]->getTaskId() == Robot::NO_TASK, "OUT_OF_BATTERY robot is skipped");
}

// Tasks that are already done or failed are not given out again
void testSkipsFinishedTasks()
{
    Dispatcher d;
    vector<unique_ptr<Robot>> robots;
    robots.push_back(make_unique<StandardRobot>(1, Position(0, 0)));

    d.addTask(Task(1, Position(0, 1), Position(0, 2), 5));
    d.addTask(Task(2, Position(0, 1), Position(0, 2), 5));
    d.findTask(1)->setStatus(TaskStatus::COMPLETED);
    d.findTask(2)->setStatus(TaskStatus::FAILED);
    d.assignTasks(robots);

    check(robots[0]->getTaskId() == Robot::NO_TASK, "Completed and failed tasks are not reassigned");
}

// clear() removes everything
void testClear()
{
    Dispatcher d;
    d.addTask(Task(1, Position(0, 0), Position(0, 1), 5));
    d.clear();

    check(d.getTasks().empty(), "clear() removes all tasks");
    check(d.findTask(1) == nullptr, "Removed task can't be found");
}

int main()
{
    cout << "===== Dispatcher Tests =====" << endl;

    testAddAndFind();
    testBasicAssign();
    testHeavyTask();
    testTooHeavyForEveryone();
    testOneTaskPerRobot();
    testOrder();
    testLowBattery();
    testHeavyBattery();
    testOutOfBatteryRobot();
    testSkipsFinishedTasks();
    testClear();

    cout << endl << passed << " passed, " << failed << " failed" << endl;
    return failed == 0 ? 0 : 1;
}