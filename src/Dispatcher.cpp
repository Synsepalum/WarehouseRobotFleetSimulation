#include "Dispatcher.h"
using namespace std;

// Removes all tasks
void Dispatcher::clear()
{
    tasks.clear();
}

// Adds a new task to the list
void Dispatcher::addTask(const Task& task)
{
    tasks.push_back(task);
}

// A robot can take a task if it is idle, has no task, has battery left,
// and can carry the task's weight
bool Dispatcher::canTake(const Robot& robot, const Task& task) const
{
    return robot.getStatus() == RobotStatus::IDLE
        && robot.getTaskId() == -1
        && robot.getBattery() > 0
        && robot.canCarry(task.getWeight());
}

// Goes through pending tasks in the order
// they were added and gives each one to
// the first robot that can take it
void Dispatcher::assignTasks(vector<unique_ptr<Robot>>& robots)
{
    for (Task& task : tasks)
    {
        if (task.getStatus() != TaskStatus::PENDING)
            continue;

        for (auto& robot : robots)
        {
            if (canTake(*robot, task))
            {
                robot->assignTask(task.getId());
                task.setStatus(TaskStatus::ASSIGNED);
                break;   // this task is taken, go to the next task
            }
        }
    }
}

// Finds a task by ID
Task* Dispatcher::findTask(int id)
{
    for (Task& task : tasks)
    {
        if (task.getId() == id)
            return &task;
    }
    return nullptr;
}

// Read-only access to all tasks
const vector<Task>& Dispatcher::getTasks() const
{
    return tasks;
}