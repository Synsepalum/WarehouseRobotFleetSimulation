#ifndef STANDARD_ROBOT_H
#define STANDARD_ROBOT_H

#include "Robot.h"
using namespace std;

//StandardRobot: a normal warehouse robot.
//Carries light items (up to 10) and uses little battery (1 per step).
//"public Robot" means it inherits everything Robot has.
class StandardRobot : public Robot
{
public:
    static const int MAX_LOAD = 10;   // heaviest item it can carry
    static const int STEP_COST = 1;   // battery used per square moved

    // Constructor: passes id and start up to Robot's constructor
    StandardRobot(int id, const Position& start);

    // "override" tells the compiler: this replaces Robot's pure virtual version.
    // If the function name or parameters didn't match Robot's, the compiler would give an error.
    bool canCarry(int weight) const override;
    int batteryCostPerStep() const override;
};

#endif