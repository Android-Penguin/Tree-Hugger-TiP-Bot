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
        // Initialise############################################################
        if(!driveInitialise) {
            drive.reset_drive();
            drive.set_direction(Trees);// Set starting drive direction
            driveInitialise = true;
        }
        if(!liftInitialise && lift.zeroLift()) {
            liftInitialise = true;
        }

        // Auton Routine#########################################################
        switch (autonSequenceState) {
            case 0:
                // Yup do something
                break;
        }


        if(autonSequenceState != previousState) {
            stateEntryTime = pros::millis();
            autonSequenceState = previousState;
        }
        pros::delay(20);
    }
}