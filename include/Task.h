#ifndef TASK_H
#define TASK_H

#include "Position.h"

enum class TaskStatus {
      PENDING,
      ASSIGNED,
      PICKED_UP,
      COMPLETED,
      FAILED
};

class Task {
private:
      int id;
      Position pickup;
      Position dropoff;
      int weight;
      TaskStatus status;

public:
      Task(int id, Position pickup, Position dropoff, int weight);

      int getId() const;
      Position getPickup() const;
      Position getDropoff() const;
      int getWeight() const;
      TaskStatus getStatus() const;

      void setStatus(TaskStatus newStatus);
};

#endif
