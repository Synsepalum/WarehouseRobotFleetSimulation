# Warehouse Robot Fleet Simulation

A console-based C++ program that simulates a small automated warehouse. Robots move around a grid, pick up items at pickup points and deliver them to drop-off points. The user adds delivery tasks and moves the simulation forward step by step while watching the robots move, use battery and finish their jobs.

## Project Overview

The warehouse is a 20 x 20 grid with walls, 4 pickup points and 8 drop-off points. Three robots start in the top-left corner:

| Robot | Type | Heaviest item | Battery used per step |
|---|---|---|---|
| 1 | Standard | 10 | 1 |
| 2 | Standard | 10 | 1 |
| 3 | Heavy | 50 | 2 |

Every robot starts with 100 battery. Each task has a pickup spot, a drop-off spot and a weight. The program decides which robot gets each task, finds a route around the walls and moves the robots one square per step.

## Team Members

| Name | GitHub | Primary Responsibilities |
|---|---|---|
| Jeremy Huang | Synsepalum | Repository setup, `Dispatcher` (task assignment and battery check), Dispatcher tests, connecting `Menu` to `Simulation`, `main.cpp`, README and design document |
| Vikunwat Lav | viav0 | `Task` class, `Menu` class and input checking |
| Devon Nghiem | DevonNghiem | `Position` class, `Warehouse` class and map, UML class diagram |
| Mahir Altuntas | mahiraltuntas | `Simulation` class (runs each step, pickups, deliveries and battery recovery) |
| Mohammad Bahramand | MohammadBahramand | `Robot` base class, `StandardRobot`, `HeavyRobot`, robot tests |
| Abdullah Shafi | abdullah-shafi2007 | `PathPlanner` (BFS pathfinding) and pathfinding tests |

## Features

- Map display with each robot shown by its ID number
- Three robots of two types, each with a position, battery level and status
- Add pickup-and-delivery tasks with a weight
- Tasks go from PENDING to ASSIGNED to PICKED_UP to COMPLETED (or FAILED)
- Automatic task assignment: each task goes to a free robot that can carry it and has enough battery for the trip
- Shortest-route pathfinding around walls (BFS)
- Battery used on every move, with heavy robots using twice as much
- Safe stop when a robot runs out of battery: its task goes back to pending or is marked failed
- Advance the simulation 1 to 100 steps at a time
- Reset the scenario at any time
- Input checking at every prompt, so letters, extra words or bad numbers never crash the program
- Rejects tasks on walls, off the map, with a bad weight, or with a drop-off that can't be reached

## Class Design

| Class | Responsibility |
|---|---|
| `Position` | One spot on the grid (row and column). Overloads `==` and `!=`. |
| `Warehouse` | The 20 x 20 map. Checks if a spot is open and prints the map. |
| `PathPlanner` | Finds the shortest route between two spots using BFS |
| `Robot` | Abstract base class for all robots: ID, position, battery, status and current task |
| `StandardRobot` | Carries up to 10, uses 1 battery per step |
| `HeavyRobot` | Carries up to 50, uses 2 battery per step |
| `Task` | One pickup-and-delivery job with a weight and a status |
| `Dispatcher` | Owns all tasks and decides which robot gets each one |
| `Simulation` | Owns the warehouse, robots, dispatcher and planner. Runs each step. |
| `Menu` | Shows the options, reads and checks input, and calls `Simulation` |

`main.cpp` only creates the `Simulation` and runs the `Menu`.

See [docs/DESIGN.md](docs/DESIGN.md) for the full design explanation and [docs/UML.pdf](docs/UML.pdf) for the UML class diagram.

## Building

The project needs a C++17 compiler.

**g++ or clang++ (Linux, macOS, or Windows with MinGW):**

From the top folder of the repository, run:

```
g++ -std=c++17 -Iinclude src/*.cpp -o WarehouseSim
```

On a Mac you can use `clang++` instead of `g++` with the same flags.

**Visual Studio:**

1. Create an empty C++ console project.
2. Add every file in `src/` as a source file and every file in `include/` as a header file. Do not add the files in `tests/`, since each one has its own `main()`.
3. In Project Properties, add the `include` folder under C/C++ → General → Additional Include Directories.
4. Set the C++ Language Standard to C++17.

## Running

```
./WarehouseSim        (Linux / macOS)
WarehouseSim.exe      (Windows)
```

A menu appears:

```
1. Display Warehouse
2. Display Robots
3. Display Tasks
4. Add Task
5. Advance Simulation
6. Reset Simulation
7. Exit
```

Type a number and press Enter.

**Reading the map:**

| Symbol | Meaning |
|---|---|
| `_` | Open floor |
| `\|` | Wall |
| `P` | Pickup point |
| `D` | Drop-off point |
| `1`, `2`, `3` | A robot, shown by its ID |

Rows and columns are numbered 0 to 19, with (0, 0) in the top-left corner. When adding a task, enter the row first and then the column.

## Example Run

This run shows a full delivery by each robot type, battery use and several edge cases.

1. **Light task:** choose 4 and enter pickup `0 0`, drop-off `1 8`, weight `5`. Robot 1 gets it.
2. **Heavy task:** choose 4 and enter pickup `0 3`, drop-off `7 8`, weight `30`. The standard robots can't carry 30, so Robot 3 (Heavy) gets it.
3. **Too heavy for every robot:** choose 4 and enter pickup `8 0`, drop-off `13 10`, weight `80`. It is added but stays PENDING because no robot can carry it.
4. **Wall spot:** choose 4 and enter pickup `0 4`, drop-off `1 8`, weight `5`. The pickup is a wall, so the task is rejected with a message.
5. **Unreachable drop-off:** choose 4 and enter pickup `13 0`, drop-off `19 19`, weight `5`. That drop-off is walled in, so the task is rejected with a message.
6. **Bad input:** type `abc` or `9` at the menu. The program shows an error and asks again.
7. Choose 5 and advance `15` steps. Both robots pick up their items and deliver them by step 15.
8. Choose 1 to see the map, 2 to see the robots and 3 to see the tasks. Robot 1's battery drops by 1 per move and Robot 3's drops by 2 per move. Tasks 1 and 2 are COMPLETED and task 3 is still PENDING.

## Running the Tests

Each test file has its own `main()`, so each one is built separately. From the top folder:

**Robot tests:**
```
g++ -std=c++17 -Iinclude tests/RobotTests.cpp src/Robot.cpp src/StandardRobot.cpp src/HeavyRobot.cpp src/Position.cpp -o robot_tests
./robot_tests
```

**PathPlanner tests:**
```
g++ -std=c++17 -Iinclude tests/PathPlannerTests.cpp src/PathPlanner.cpp src/Position.cpp -o planner_tests
./planner_tests
```

**Dispatcher tests:**
```
g++ -std=c++17 -Iinclude tests/DispatcherTests.cpp src/Dispatcher.cpp src/Task.cpp src/Robot.cpp src/StandardRobot.cpp src/HeavyRobot.cpp src/Position.cpp -o dispatcher_tests
./dispatcher_tests
```

The Robot and Dispatcher tests print `[PASS]` or `[FAIL]` for each check and a total at the end. The PathPlanner tests stop with an error if a check fails, and print the number of passing test cases if they all pass.

## Known Limitations

- Robots are allowed to share a square. The assignment does not require traffic control between robots.
- The battery check before assigning a task does not count walls, so a robot can still run out of battery on a long trip. When that happens it stops safely, and its task goes back to pending or is marked failed.
- A robot that runs out of battery stays that way until the simulation is reset. There is no charging.
- The map is fixed. To change it, edit the layout in `Warehouse::loadDefault()`.
- A task heavier than 50 stays pending forever because no robot can carry it.