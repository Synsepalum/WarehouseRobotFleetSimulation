#include "Warehouse.h"
#include <iostream>
#include <string>

using namespace std;

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
        "P___||______||_____|",
        "____||______||____|D",   // (19,19) D is walled in on purpose: unreachable test
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

// What's on that square. Off map = wall
char Warehouse::getTile(const Position& p) const
{
    if (!isInBounds(p))
        return '|';
    return grid[p.getRow()][p.getCol()];
}

// Map as numbers for PathPlanner.
// Wall '|' = 1. Everything else = 0.
vector<vector<int>> Warehouse::toPlannerGrid() const
{
    vector<vector<int>> numbers;

    for (const auto& row : grid)
    {
        vector<int> numberRow;
        for (char c : row)
        {
            if (c == '|')
                numberRow.push_back(1);
            else
                numberRow.push_back(0);
        }
        numbers.push_back(numberRow);
    }

    return numbers;
}

// Prints just the map
void Warehouse::display() const
{
    for (const auto& row : grid)
    {
        for (char c : row)
            cout << c << ' ';
        cout << '\n';
    }
}

// Prints the map but if a robot is on a square, prints its ID instead
void Warehouse::display(    const vector<Position>& robotSpots,
                            const vector<int>& robotIds        ) const
{
    for (int r = 0; r < getRows(); r++)
    {
        for (int c = 0; c < getCols(); c++)
        {
            char symbol = grid[r][c];

            // Check every robot. Stop at the shorter list so we never
            // read past the end if the two lists are different sizes.
            for (size_t i = 0; i < robotSpots.size() && i < robotIds.size(); i++)
            {
                if (robotSpots[i] == Position(r, c))
                {
                    int id = robotIds[i];
                    if (id >= 0 && id <= 9)
                        symbol = '0' + id;   
                    else
                        symbol = 'R';        
                }
            }

            cout << symbol << ' ';
        }
        cout << '\n';
    }
}