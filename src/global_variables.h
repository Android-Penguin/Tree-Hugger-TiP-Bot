#pragma once

// Auton selection from competition initialise
extern int autonSelection;

// remembers what time a state was entered, allowing the robot to leave the state afer a set amount of time
extern int stateEntryTime;

// Lift positions
extern int folded;
extern int raised;
extern int pickup;

// Lift shift direction
enum shiftDirection {
    UP = -1,
    DOWN = 1
};

// Drive direction
enum Direction {
    Donuts = 1,
    Trees = -1
};