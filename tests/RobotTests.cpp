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

// A brand-new robot should have the starting values from Robot's constructor
void testNewRobot()
{
    StandardRobot r(1, Position(2, 3));
    check(r.getId() == 1, "New robot has the right id");
    check(r.getPosition() == Position(2, 3), "New robot starts at the right spot");  // uses Position's operator==
    check(r.getBattery() == Robot::MAX_BATTERY, "New robot starts with full battery");
    check(r.getStatus() == RobotStatus::IDLE, "New robot starts IDLE");
    check(r.getTaskId() == Robot::NO_TASK, "New robot has no task (-1)");
}

// Edge cases: exactly at the limit, one over the limit, zero, negative
void testCanCarry()
{
    StandardRobot s(1, Position(0, 0));
    HeavyRobot h(2, Position(0, 1));
    check(s.canCarry(10), "Standard can carry exactly 10");
    check(!s.canCarry(11), "Standard can't carry 11");
    check(h.canCarry(11), "Heavy can carry 11");
    check(h.canCarry(50), "Heavy can carry exactly 50");
    check(!h.canCarry(51), "Heavy can't carry 51");
    check(!s.canCarry(0) && !h.canCarry(0), "Nobody accepts weight 0");
    check(!s.canCarry(-5) && !h.canCarry(-5), "Nobody accepts negative weight");
}

// One step: Standard loses 1, Heavy loses 2
void testBatteryDrain()
{
    StandardRobot s(1, Position(0, 0));
    HeavyRobot h(2, Position(1, 0));
    check(s.moveTo(Position(0, 1)), "Standard move succeeds");
    check(s.getPosition() == Position(0, 1), "Standard is at the new spot");
    check(s.getBattery() == 99, "Standard battery 100 -> 99 after one step");
    check(h.moveTo(Position(1, 1)), "Heavy move succeeds");
    check(h.getBattery() == 98, "Heavy battery 100 -> 98 after one step");
}

// Battery edge case: robot stops safely when the battery runs out
void testOutOfBattery()
{
    // Heavy: 100 battery / 2 per step = exactly 50 moves, then it must refuse
    HeavyRobot h(1, Position(0, 0));
    int moves = 0;
    while (h.moveTo(Position(0, 0)))
    {
        moves++;
    }
    check(moves == 50, "Heavy makes exactly 50 moves on a full battery");
    check(h.getBattery() == 0, "Heavy battery ends at 0, never negative");

    // Standard: use up all 100, then the next move should fail and not move it
    StandardRobot s(2, Position(5, 5));
    for (int i = 0; i < 100; i++)
    {
        s.moveTo(Position(5, 5));
    }
    check(!s.moveTo(Position(5, 6)), "Move fails when battery is 0");
    check(s.getPosition() == Position(5, 5), "Robot doesn't move when the move fails");
}

// Task and status functions that the Dispatcher and Simulation use
void testTasksAndStatus()
{
    StandardRobot r(1, Position(0, 0));
    r.assignTask(7);
    check(r.getTaskId() == 7, "assignTask sets the task id");
    r.clearTask();
    check(r.getTaskId() == Robot::NO_TASK, "clearTask sets it back to -1");
    r.setStatus(RobotStatus::MOVING);
    check(r.getStatus() == RobotStatus::MOVING, "setStatus changes the status");
}

// Polymorphism: store robots exactly like the Simulation does (vector<unique_ptr<Robot>>)
// and check that each one still uses its OWN canCarry and batteryCostPerStep
void testPolymorphism()
{
    vector<unique_ptr<Robot>> robots;
    robots.push_back(make_unique<StandardRobot>(1, Position(0, 0)));
    robots.push_back(make_unique<StandardRobot>(2, Position(0, 1)));
    robots.push_back(make_unique<HeavyRobot>(3, Position(0, 2)));

    int canCarry30 = 0;
    for (const auto& robot : robots)
    {
        if (robot->canCarry(30))   // virtual call: picks Standard's or Heavy's version
        {
            canCarry30++;
        }
    }
    check(canCarry30 == 1, "Only the HeavyRobot can carry 30 (through Robot pointers)");
    check(robots[0]->batteryCostPerStep() == 1, "Robot pointer calls Standard's cost (1)");
    check(robots[2]->batteryCostPerStep() == 2, "Robot pointer calls Heavy's cost (2)");
}

int main()
{
    cout << "===== Robot Tests =====" << endl;
    testNewRobot();
    testCanCarry();
    testBatteryDrain();
    testOutOfBattery();
    testTasksAndStatus();
    testPolymorphism();
    cout << "=======================" << endl;
    cout << passed << " passed, " << failed << " failed" << endl;
    return failed == 0 ? 0 : 1;
}