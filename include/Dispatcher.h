#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <vector>
#include <memory>
#include "Task.h"
#include "Robot.h"
using namespace std;

// Dispatcher owns every task and decides which robot gets which task.
// It does not move robots or print anything.
class Dispatcher
{
private:
    vector<Task> tasks;   // every task: pending, in progress, and done

    // Returns true if this robot is free and able to take this task
    bool canTake(const Robot& robot, const Task& task) const;

public:
    // Removes all tasks 
    void clear();

    // Adds a new task. It starts as pending.
    void addTask(const Task& task);

    // Gives each pending task to the first free robot that can carry it
    // and stays pending if none can take it
    void assignTasks(vector<unique_ptr<Robot>>& robots);

    // Returns a pointer to the task with this ID, or nullptr if not found
    Task* findTask(int id);

    // list of every task
    const vector<Task>& getTasks() const;
};

#endif