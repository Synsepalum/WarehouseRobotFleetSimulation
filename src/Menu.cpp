#include "Menu.h"
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

using namespace std;

// Constructor: saves a reference to the Simulation
Menu::Menu(Simulation& simulation) : sim(simulation)
{
}

// Prints the list of options
void Menu::displayMenu() const
{
    cout << "\n";
    cout << "=================================\n";
    cout << "    WAREHOUSE ROBOT SIMULATION   \n";
    cout << "=================================\n";
    cout << "1. Display Warehouse\n";
    cout << "2. Display Robots\n";
    cout << "3. Display Tasks\n";
    cout << "4. Add Task\n";
    cout << "5. Advance Simulation\n";
    cout << "6. Reset Simulation\n";
    cout << "7. Exit\n";
    cout << "=================================\n";
}

// Reads a menu choice from 1-7.
int Menu::getChoice() const
{
    string input;
    int choice;

    while (true)
    {
        cout << "Enter your choice (1-7): ";

        if (!getline(cin, input)) 
        {
            return 7;
        }

        istringstream stream(input);
        string extra;

        if ((stream >> choice) &&
            !(stream >> extra) &&
            choice >= 1 && choice <= 7)
        {
            return choice;
        }

        cout << "Invalid choice. Please enter 1-7.\n";
    }
}

// Reads one whole number with the same checks as getChoice
int Menu::readInt(const string& prompt) const
{
    string input;
    int value;

    while (true)
    {
        cout << prompt;

        if (!getline(cin, input))
        {
            return 0;
        }

        istringstream stream(input);
        string extra;

        if ((stream >> value) && !(stream >> extra))
        {
            return value;
        }

        cout << "Please enter a whole number.\n";
    }
}

// Asks for a new task and gives it to the Simulation.
// Simulation throws invalid_argument if a spot is a
// wall, off the map, can't be reached, or the weight
// is bad. We catch it here and show the message, so
// the program keeps running.
void Menu::addTask()
{
    int pickupRow  = readInt("Pickup row: ");
    int pickupCol  = readInt("Pickup column: ");
    int dropoffRow = readInt("Dropoff row: ");
    int dropoffCol = readInt("Dropoff column: ");
    int weight     = readInt("Weight: ");

    try
    {
        sim.addTask(Position(pickupRow, pickupCol),
                    Position(dropoffRow, dropoffCol),
                    weight);
        cout << "Task added.\n";
    }
    catch (const invalid_argument& e)
    {
        cout << "Error: " << e.what() << "\n";
    }
}

// Main loop: show the menu, run the choice, repeat until the user picks Exit
void Menu::run()
{
    bool running = true;

    while (running)
    {
        displayMenu();
        int choice = getChoice();

        switch (choice)
        {
            case 1:
                sim.displayWarehouse();
                break;
            case 2:
                sim.displayRobots();
                break;
            case 3:
                sim.displayTasks();
                break;
            case 4:
                addTask();
                break;
            case 5:
            {
                int steps = readInt("How many steps (1-100)? ");
                if (steps < 1 || steps > 100)
                {
                    cout << "Steps must be from 1 to 100.\n";
                    break;
                }
                for (int i = 0; i < steps; i++)
                    sim.step();
                break;
            }
            case 6:
                sim.reset();
                cout << "Simulation reset.\n";
                break;
            case 7:
                running = false;
                cout << "Goodbye!\n";
                break;
        }
    }
}