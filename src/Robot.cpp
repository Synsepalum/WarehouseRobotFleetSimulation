#include "Robot.h"
using namespace std;

//==============Constructor===============
//Uses an initializer list (the part after the colon)
//to set every member before the body runs.
//Every new robot starts with a full battery, no task, 
// and status IDLE.
// ===================================================
Robot::Robot(int id, const Position& start)
    : id(id), position(start), battery(MAX_BATTERY), taskId(NO_TASK), status(RobotStatus::IDLE)
{
}

//==============Destructor===============
//Robot doesn't allocate any memory with new, so there's nothing to clean up.
//It still has to exist (and be virtual in the .h) so that deleting a
//StandardRobot or HeavyRobot through a Robot pointer works correctly.
//===================================================
Robot::~Robot()
{
}

//================Getters=================
//just return the private values.
// =======================================
int Robot::getId() const
{
    return id;
}

Position Robot::getPosition() const
{
    return position;
}

int Robot::getBattery() const
{
    return battery;
}

RobotStatus Robot::getStatus() const
{
    return status;
}

int Robot::getTaskId() const
{
    return taskId;
}

//===========Changes the robot's state 
//(IDLE, MOVING, OUT_OF_BATTERY).
// ===================================================
void Robot::setStatus(RobotStatus newStatus)
{
    status = newStatus;
}


//The Dispatcher calls assignTask() when it gives this robot a job.
//The Simulation calls clearTask() when the job is delivered,
// unreachable, or abandoned because the battery ran out.
// ===================================================
void Robot::assignTask(int newTaskId)
{
    taskId = newTaskId;
}

void Robot::clearTask()
{
    taskId = NO_TASK;   // back to -1, meaning "no task"
}

//moveTo: move ONE square and use up battery.
bool Robot::moveTo(const Position& next)
{
    int cost = batteryCostPerStep();   // how much this robot type uses per step

    if (battery < cost)                // not enough battery left
    {
        return false;                  // stay put, report failure
    }

    position = next;                   // move to the new square
    battery -= cost;                   // use up battery
    return true;                       // move worked
}