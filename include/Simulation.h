#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include <memory>
#include "Warehouse.h"
#include "Robot.h"
#include "Dispatcher.h"
#include "PathPlanner.h"
#include "Position.h"
using namespace std;

// Simulation runs the whole warehouse.
// It owns the map, the robots, the dispatcher, and the path planner,
// and moves everything forward one step at a time.
class Simulation
{
private:
    Warehouse warehouse;               // the map
    vector<unique_ptr<Robot>> robots;  // all the robots
    Dispatcher dispatcher;             // holds tasks and hands them out
    PathPlanner planner;               // finds routes around walls
    int stepCount;                     // how many steps have happened
    int nextTaskId;                    // ID to give the next new task

    // Moves one robot one square toward where it needs to go
    void updateRobot(Robot& robot);

public:
    // Constructor: sets up a fresh scenario
    Simulation();

    // Starts over with 3 robots and no tasks
    void reset();

    // Adds a new pickup-and-delivery job
    // Throws invalid_argument if a spot is a wall or the weight is bad
    void addTask(const Position& pickup, const Position& dropoff, int weight);

    // Moves the simulation forward one step
    void step();

    // Show what's going on
    void displayWarehouse() const;
    void displayRobots() const;
    void displayTasks() const;

    int getStepCount() const;
};

#endif
