#include "Task.h"

//===========================================
Task::Task(int id, int px, int py, int dx, int dy)
    : taskID(id),
      pickupX(px),
      pickupY(py),
      deliveryX(dx),
      deliveryY(dy),
      status(TaskStatus::Pending),
      assignedRobotID(-1)
{
}
//===========================================
int Task::getTaskID() const {
    return taskID;
}
//===========================================
int Task::getPickupX() const {
    return pickupX;
}
//===========================================
int Task::getPickupY() const {
    return pickupY;
}
//===========================================
int Task::getDeliveryX() const {
    return deliveryX;
}
//===========================================
int Task::getDeliveryY() const {
    return deliveryY;
}
//===========================================
TaskStatus Task::getStatus() const {
    return status;
}
//===========================================
int Task::getAssignedRobotID() const {
    return assignedRobotID;
}
//===========================================
bool Task::assignToRobot(int robotID) {
    if (status != TaskStatus::Pending || robotID < 0) {
        return false;
    }

    assignedRobotID = robotID;
    status = TaskStatus::Assigned;
    return true;
}
//===========================================
void Task::markInProgress() {
    if (status == TaskStatus::Assigned) {
        status = TaskStatus::InProgress;
    }
}
//===========================================
void Task::markCompleted() {
    if (status == TaskStatus::InProgress) {
        status = TaskStatus::Completed;
    }
}
//===========================================
