#include "HeavyRobot.h"
using namespace std;

//Constructor: same idea as StandardRobot, just passes everything to Robot
HeavyRobot::HeavyRobot(int id, const Position& start)
    : Robot(id, start)
{
}

//Same rule as StandardRobot, but with a limit of 50
bool HeavyRobot::canCarry(int weight) const
{
    return weight > 0 && weight <= MAX_LOAD;
}

//Heavy robot drains 2 per step
int HeavyRobot::batteryCostPerStep() const
{
    return STEP_COST;
}