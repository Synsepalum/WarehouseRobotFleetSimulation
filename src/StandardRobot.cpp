#include "StandardRobot.h"
using namespace std;

//Constructor
//StandardRobot has no data of its own, so it just hands id and start
//to the Robot constructor, which sets battery, task, and status.
StandardRobot::StandardRobot(int id, const Position& start)
    : Robot(id, start)
{
}

// Weight must be positive AND not more than the limit.
// weight > 0 blocks bad values like 0 or -5.
bool StandardRobot::canCarry(int weight) const
{
    return weight > 0 && weight <= MAX_LOAD;
}

// Robot::moveTo() calls this to know how much battery to subtract.
int StandardRobot::batteryCostPerStep() const
{
    return STEP_COST;
}