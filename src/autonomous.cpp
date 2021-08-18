#include "main.h"
#include "global_variables.h"
#include "robot_objects.h"
#include "functions.h"
#include "pros/llemu.hpp"
#include "pros/rtos.hpp"

void autonomous() {
	// controls what state the robot runs, begins with 0 (Idle)
    int autonSequenceState = 0;
    // used to detect when state has changed
    int previousState;

    while (true) {
        //LCD####################################################################
        pros::lcd::set_text(0, "Tree Hugger----------------Auton");
        pros::lcd::print(1, "Auton Sequence State = %d", autonSequenceState);
        // pros::lcd::print(2, "");
        // pros::lcd::print(3, "");
        // pros::lcd::print(4, "");
    	// pros::lcd::print(5, "");
    	// pros::lcd::print(6, "");
        pros::lcd::print(7, "");

        // Auton Routine#########################################################
        switch (autonSequenceState) {
            case 0:
                autonSequenceState = 1;
                break;
            case 1:// Initialise
                if(!driveInitialise) {
                    drive.reset_drive();
                    drive.set_direction(Trees);// Set starting drive direction
                    driveInitialise = true;
                }
                if(!liftInitialise && lift.zeroLift()) {
                    liftInitialise = true;
                }
                if(liftInitialise && driveInitialise) {
                    autonSequenceState = 2;
                }
                break;
            case 2:// Drive?
                if(drive.move_for(50000, Donuts)) {
                    autonSequenceState = 3;
                }
                break;
            case 3:
                break;
        }


        if(autonSequenceState != previousState) {
            stateEntryTime = pros::millis();
            previousState = autonSequenceState;
        }
        lift.update();
        pros::delay(20);
    }
}