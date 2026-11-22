#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include <vector>
#include "Position.h"
using namespace std;

//   Warehouse map
//   '_' = floor
//   '|' = wall
//   'P' = pickup point
//   'D' = drop off point

class Warehouse
{
private:
    vector<vector<char>> grid;

public:
    Warehouse();

    void loadDefault();

    int getRows() const;
    int getCols() const;

    bool isInBounds(const Position& p) const;
    bool isWalkable(const Position& p) const;
    char getTile(const Position& p) const;

    // For PathPlanner: 1 = wall & 0 = open
    vector<vector<int>> toPlannerGrid() const;

    // Just the map
    void display() const;

    // Prints the map with robot IDs 
    void display(const vector<Position>& robotSpots,
                 const vector<int>& robotIds) const;
};

#endif
