#include "Warehouse.h"
#include <iostream>
#include <string>

using namespace std;

// Builds the map as soon as a Warehouse is made
Warehouse::Warehouse()
{
    loadDefault();
}

// 20 x 20 map. Each string is one row.
void Warehouse::loadDefault()
{
   vector<string> layout = 
   {
        "P___|_______|_______",
        "____|___D___|____D__",
        "_||_|_______|_______",
        "_||_____||______||__",
        "______||____||______",
        "__||________||___|__",
        "__||__|__________|__",
        "______|_D____||_____",
        "P_____|______||___D_",
        "____________________",
        "___||______||_______",
        "___||__|___||___|___",
        "_______|________|___",
        "P______|__D_____|_D_",
        "__||________||______",
        "__||__||____||__||__",
        "______||________||__",
        "_____________D______",
        "P___||______||______",
        "____||______||______",
    };

    grid.clear();
    for (const string& line : layout)
    {
        grid.push_back(vector<char>(line.begin(), line.end()));
    }
}

int Warehouse::getRows() const
{
    return static_cast<int>(grid.size());
}

int Warehouse::getCols() const
{
    if (grid.empty())
        return 0;
    return static_cast<int>(grid[0].size());
}

// True if the spot is inside the map
bool Warehouse::isInBounds(const Position& p) const
{
    return p.getRow() >= 0 && p.getRow() < getRows()
        && p.getCol() >= 0 && p.getCol() < getCols();
}

// True if inside the map and not a wall
bool Warehouse::isWalkable(const Position& p) const
{
    if (!isInBounds(p))
        return false;
    return grid[p.getRow()][p.getCol()] != '|';
}

// What's on that square. Off the map counts as a wall.
char Warehouse::getTile(const Position& p) const
{
    if (!isInBounds(p))
        return '|';
    return grid[p.getRow()][p.getCol()];
}

// Prints the map
void Warehouse::display() const
{
    for (const auto& row : grid)
    {
        for (char c : row)
            cout << c << ' ';
        cout << '\n';
    }
}