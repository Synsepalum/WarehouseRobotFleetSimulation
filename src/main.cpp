#include "Simulation.h"
#include "Menu.h"

// Menu reads the user's commands and calls Simulation.
int main()
{
    Simulation sim;
    Menu menu(sim);
    menu.run();
    return 0;
}