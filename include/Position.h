#ifndef POSITION_H
#define POSITION_H
 
#include <iostream>
using namespace std;
 
class Position
{
private:
    int row;
    int col;
 
public:
    Position();              
    Position(int r, int c);
 
    int getRow() const;
    int getCol() const;

    bool operator==(const Position& other) const;
    bool operator!=(const Position& other) const;

    void display() const;
};
 
#endif
 