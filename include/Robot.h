#ifndef ROBOT_H
#define ROBOT_H

#include "Position.h"
using namespace std;


enum class RobotStatus
{
    IDLE,           //not doing anything,ready for a task
    MOVING,         //moving toward a pickup or drop-off spot
    OUT_OF_BATTERY  //battery too low to move; the robot stops (safe recovery action)
};

//Abstract base class,
//holds everything every robot has in common (id, 
//position, battery, task, status).
//The things that are DIFFERENT for each robot type are pure virtual functions (= 0),

class Robot
{
private:
    int id;               //robot's number (1, 2, 3, ...)
    Position position;    //where the robot is on the grid
    int battery;          //battery left, from 0 to MAX_BATTERY
    int taskId;           //id of the task it's working on, or NO_TASK
    RobotStatus status;   //IDLE, MOVING, or OUT_OF_BATTERY

public:
    static const int MAX_BATTERY = 100;  //every robot starts fully charged
    static const int NO_TASK = -1;       //the Dispatcher checks for -1 to mean "no task"

    //Constructor: sets the id and starting spot,
    //full battery, no task, status IDLE
    Robot(int id, const Position& start);

    //Virtual destructor is required because the Simulation stores robots as
    //unique_ptr<Robot>. When a StandardRobot is deleted through a Robot pointer,
    // "virtual" makes sure the correct destructor runs.
    virtual ~Robot();

    //Getters (const = they don't change the robot)
    int getId() const;
    Position getPosition() const;
    int getBattery() const;
    RobotStatus getStatus() const;
    int getTaskId() const;

    //Change the robot's state
    void setStatus(RobotStatus newStatus);

    //Give the robot a task, or take it away
    void assignTask(int newTaskId);
    void clearTask();

    
    bool moveTo(const Position& next);

    

    
    virtual bool canCarry(int weight) const = 0;

    // How much battery one step costs this robot type
    virtual int batteryCostPerStep() const = 0;
};

#endif