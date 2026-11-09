#ifndef HEAVY_ROBOT_H
#define HEAVY_ROBOT_H

#include "Robot.h"
using namespace std;

//HeavyRobot: carries big items (up to 50)
//but uses twice the battery per step (2 instead of 1).
//It inherits from Robot directly, NOT from StandardRobot:
//the two are separate kinds of robot.
class HeavyRobot : public Robot
{
public:
    static const int MAX_LOAD = 50;   // heaviest item it can carry
    static const int STEP_COST = 2;   // battery used per square moved

    HeavyRobot(int id, const Position& start);

    bool canCarry(int weight) const override;
    int batteryCostPerStep() const override;
};

#endif