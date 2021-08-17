#include "main.h"
#include "global_variables.h"
#include "robot_objects.h"
#include "functions.h"
#include "pros/llemu.hpp"

void opcontrol() {
    // Initialising opcontrol
    stateEntryTime = pros::millis();
    manualLCDClear();

	// Controller
	pros::Controller master(pros::E_CONTROLLER_MASTER);

    // Drive variables
    float driveSpeed;
    float rotateSpeed;

    // Initialise variables
    bool driveInitialise = false;
    bool liftInitialise = false;// Set to false when zeroing button added

    // Cooldown variables
    bool liftRaiseCooldown = false;
    bool liftLowerCooldown = false;

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


        // LCD###################################################################
        pros::lcd::set_text(0, "---------Tree Hugger---------");
        pros::lcd::print(1, "Drive Init=%d  Lift Init=%d", driveInitialise, liftInitialise);
        pros::lcd::print(2, "Drive direction: %s", drive.get_direction());
        pros::lcd::print(3, "Lift Pos = %f", lift.get_position());
        pros::lcd::print(4, "Button State= %d", lift.get_button());


		// Drive#################################################################
        if(driveInitialise) {
            driveSpeed = master.get_analog(ANALOG_RIGHT_Y)/127.0;
            driveSpeed = driveSpeed*200;
            rotateSpeed = master.get_analog(ANALOG_RIGHT_X)/127.0;
            rotateSpeed = rotateSpeed*150;

            //uses the 2 speed values from the controller to move the robot at a proportional velocity
            drive.move_velocity(driveSpeed, rotateSpeed);

            // Intaking donuts
            if(master.get_digital(DIGITAL_A)) {
                drive.set_direction(Donuts);
            }
            // Intaking trees
            if(master.get_digital(DIGITAL_X)) {
                drive.set_direction(Trees);
            }
        }


		// Lift###################################################################
        if(liftInitialise) {
            lift.move_velocity(master.get_analog(ANALOG_LEFT_Y));

            // Raise Lift
            if(master.get_digital(DIGITAL_R1) && !liftRaiseCooldown) {
                liftRaiseCooldown = true;
                // lift.shift_position(UP);
            }
            if(!master.get_digital(DIGITAL_R1)) {
                liftRaiseCooldown = false;
            }
            // Lower lift
            if(master.get_digital(DIGITAL_R2) && !liftLowerCooldown) {
                liftLowerCooldown = true;
                // lift.shift_position(DOWN);
            }
            if(!master.get_digital(DIGITAL_R2)) {
                liftLowerCooldown = false;
            }

            lift.update();
        }

		pros::delay(20);
	}
}