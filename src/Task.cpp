#include "Task.h"

//===========================================
Task::Task(int id, Position pickup, Position dropoff, int weight)
    : id(id),
      pickup(pickup),
      dropoff(dropoff),
      weight(weight),
      status(TaskStatus::PENDING)
{
}
//===========================================
int Task::getId() const 
{
    return id;
}
//===========================================
Position Task::getPickup() const 
{
    return pickup;
}
//===========================================
Position Task::getDropoff() const 
{
    return dropoff;
}
//===========================================
int Task::getWeight() const 
{
    return weight;
}
//===========================================
TaskStatus Task::getStatus() const 
{
    return status;
}
//===========================================
void Task::setStatus(TaskStatus newStatus) 
{
    status = newStatus; 
}
//===========================================
