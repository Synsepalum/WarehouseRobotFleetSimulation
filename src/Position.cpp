#include "Position.h"
#include <iostream>

using namespace std;

// Default constructor
Position::Position() : row(0), col(0) {}

// Constructor w/ rows & columns
Position::Position(int r, int c) : row(r), col(c) {}

int Position::getRow() const
{
    return row;
}

int Position::getCol() const
{
    return col;
}

// Rows and columns are equal to each other
bool Position::operator==(const Position& other) const
{
    return row == other.row && col == other.col;
}

// Rows and columns are not equal to each other
bool Position::operator!=(const Position& other) const
{
    return !(*this == other);
}

void Position::display() const
{
    cout << "(" << row << ", " << col << ")";
}