#include "main.h"
#include "global_variables.h"
#include "robot_objects.h"
#include "functions.h"
#include "pros/llemu.hpp"

int displayPage = 1;
void left_button() {if(displayPage>1){displayPage--;}else{displayPage=3;}}
void center_button() {displayPage=1;}
void right_button() {if(displayPage<3){displayPage++;}else {displayPage=1;}}

void opcontrol() {
    // Initialising opcontrol
    stateEntryTime = pros::millis();

    // LCD setup
    manualLCDClear();
    pros::lcd::register_btn0_cb(left_button);
	pros::lcd::register_btn1_cb(center_button);
	pros::lcd::register_btn2_cb(right_button);

	// Controller
	pros::Controller master(pros::E_CONTROLLER_MASTER);

    // Drive variables
    float driveSpeed;
    float rotateSpeed;

    // Cooldown variables
    bool liftRaiseCooldown = false;
    bool liftLowerCooldown = false;
    bool joystickControl = false;

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
        switch (displayPage) {
            case 1://Default Page
                pros::lcd::set_text(0, "Tree Hugger----Home-------Page 1");
                pros::lcd::print(1, "Drive Init=%d  Lift Init=%d", driveInitialise, liftInitialise);
                pros::lcd::print(2, "Drive direction: %s", drive.get_direction());
                pros::lcd::print(3, "Drive Pos=%06.0f", drive.get_position());
                pros::lcd::print(4, "Lift Pos = %04.0f", lift.get_position());
                pros::lcd::print(5, "");
    	        pros::lcd::print(6, "");
                pros::lcd::print(7, "Previous      Home          Next");
                break;
            case 2://Drive debug page
                pros::lcd::set_text(0, "Tree Hugger----Drive------Page 2");
                pros::lcd::print(1, "Drive direction: %s", drive.get_direction());
              	pros::lcd::print(2, "FL=%06.0f, FR=%06.0f", drive.wheel_position(FrontLeft), drive.wheel_position(FrontRight));
                pros::lcd::print(3, "BL=%06.0f, BR=%06.0f", drive.wheel_position(BackLeft), drive.wheel_position(BackRight));
              	pros::lcd::print(4, "Drive Pos=%06.0f", drive.get_position());
    	        pros::lcd::print(5, "");
    	        pros::lcd::print(6, "");
    	        pros::lcd::print(7, "Previous      Home          Next");
                break;
            case 3://Lift debug page
                pros::lcd::set_text(0, "Tree Hugger----Lift-------Page 3");
                pros::lcd::print(1, "Lift Pos = %04.0f", lift.get_position());
              	pros::lcd::print(2, "Lift Torque = %.04f", lift.get_torque());
                pros::lcd::print(3, "");
              	pros::lcd::print(4, "");
    	        pros::lcd::print(5, "");
    	        pros::lcd::print(6, "");
    	        pros::lcd::print(7, "Previous      Home          Next");
                break;
            case 4://template page
                pros::lcd::set_text(0, "Tree Hugger---------------Page 4");
                pros::lcd::print(1, "");
              	pros::lcd::print(2, "");
                pros::lcd::print(3, "");
              	pros::lcd::print(4, "");
    	        pros::lcd::print(5, "");
    	        pros::lcd::print(6, "");
    	        pros::lcd::print(7, "Previous      Home          Next");
                break;
            default:
                displayPage = 1;
                break;
        }


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
            if(master.get_digital(DIGITAL_L1)) {
                lift.move_velocity(master.get_analog(ANALOG_LEFT_Y));
                joystickControl = true;
            } else if(master.get_digital(DIGITAL_L2)) {
                lift.move_velocity(master.get_analog(ANALOG_LEFT_Y), true);
                joystickControl = true;
            } else if(joystickControl) {
                lift.move_velocity(0);
                joystickControl = false;
            }


            // Raise Lift
            if(master.get_digital(DIGITAL_R1) && !liftRaiseCooldown) {
                liftRaiseCooldown = true;
                lift.shift_position(UP);
            }
            if(!master.get_digital(DIGITAL_R1)) {
                liftRaiseCooldown = false;
            }
            // Lower lift
            if(master.get_digital(DIGITAL_R2) && !liftLowerCooldown) {
                liftLowerCooldown = true;
                lift.shift_position(DOWN);
            }
            if(!master.get_digital(DIGITAL_R2)) {
                liftLowerCooldown = false;
            }

            lift.update();
        } else if(lift.zeroLift()) {
            liftInitialise = true;
        }

		pros::delay(20);
	}
}