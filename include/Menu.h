#ifndef MENU_H
#define MENU_H

#include <string>
#include "Simulation.h"
using namespace std;

// Menu shows the options, reads what the user types,
// and calls the matching Simulation function.
// It does not store any data or apply any rules.
class Menu
{
private:
    Simulation& sim;   // the Simulation that main created (Menu doesn't own it)

    void displayMenu() const;
    int getChoice() const;

    // Reads one whole number. Keeps asking until the input is valid.
    int readInt(const string& prompt) const;

    // Asks for pickup, dropoff and weight, then adds the task
    void addTask();

public:
    // Gets the Simulation that main created
    Menu(Simulation& simulation);

    // Shows the menu over and over until the user picks Exit
    void run();
};

#endif