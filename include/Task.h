#ifndef TASK_H
#define TASK_H

enum class TaskStatus {
      Pending,
      Assigned,
      InProgress,
      Completed
};

class Task {
private:
  int taskID;

  int pickupX;
  int pickupY;

  int deliveryX;
  int deliveryY;

  TaskStatus status;
  int assignedRobotID;
public:
  Task(int id, int px, int py, int dx, int dy);

  int getTaskID() const;
  int getPickupX() const;
  int getPickupY() const;
  int getDeliveryX() const;
  int getDeliveryY() const;
  TaskStatus getStatus() const;
  int getAssignedRobotID() const;

  bool assignToRobot(int robotID);
  void markInProgress();
  void markCompleted();
};

#endif
