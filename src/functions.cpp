#include "main.h"
#include "global_variables.h"

void manualLCDClear() {
    pros::lcd::clear_line(0);
	pros::lcd::clear_line(1);
	pros::lcd::clear_line(2);
	pros::lcd::clear_line(3);
	pros::lcd::clear_line(4);
	pros::lcd::clear_line(5);
	pros::lcd::clear_line(6);
	pros::lcd::clear_line(7);
}

int timeInState() {
    return pros::millis()-stateEntryTime;
}
