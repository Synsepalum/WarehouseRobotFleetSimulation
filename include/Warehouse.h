#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include <vector>
#include "Position.h"
using namespace std;

//   Warehouse map: 2D grid of characters
//   '_' = floor
//   '|' = wall
//   'P' = pickup point
//   'D' = drop but that off point

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

    void display() const;
};

#endif