#pragma once

// Auton selection from competition initialise
extern int autonSelection;

// remembers what time a state was entered, allowing the robot to leave the state afer a set amount of time
extern int stateEntryTime;

// Lift positions
extern int folded;
extern int raised;
extern int pickup;

// Drive direction
enum Direction {
    Donuts = 1,
    Trees = -1
};