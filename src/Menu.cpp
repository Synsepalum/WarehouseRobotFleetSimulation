#include "Menu.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace std;
//===============================================
Menu::Menu()
{
}
//===============================================
void Menu::displayMenu() const {
    cout << "\n";
    cout << "=================================\n";
    cout << "    WAREHOUSE ROBOT SIMULATION   \n";
    cout << "=================================\n";
    cout << "1. Display Warehouse\n";
    cout << "2. Display Robots\n";
    cout << "3. Add Task\n";
    cout << "4. Display Tasks\n";
    cout << "5. Assign Tasks\n";
    cout << "6. Advance Simulation\n";
    cout << "7. Reset Simulation\n";
    cout << "8. Exit\n";
    cout << "=================================\n";
}
//===============================================
int Menu::getChoice() const 
{
    string input;
    int choice;

    while (true) 
      {
        cout << "Enter your choice (1-8): ";

        if (!getline(cin, input)) 
        {
            return 8;
        }

        istringstream stream(input);
        string extra;

        if ((stream >> choice) &&
            !(stream >> extra) &&
            choice >= 1 && choice <= 8) 
        {
            return choice;
        }

        cout << "Invalid choice. Please enter 1-8.\n";
    }
}
//===============================================
