#include "main.h"
#include "global_variables.h"
#include "robot_objects.h"
#include "functions.h"
#include "pros/llemu.hpp"

using std::string;

int displayPage = 1;
void left_button() {if(displayPage>1){displayPage--;}else{displayPage=4;}}
void center_button() {displayPage=1;}
void right_button() {if(displayPage<4){displayPage++;}else {displayPage=1;}}

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

    // Claw variables
    bool clawClosed = false;
    bool clawJoystick = true;//Flag to enable movement from joystick
    int grabState = 0;
    int clawResetState = 0;
    string grabAction = "releasing";

    // Cooldown variables
    bool liftRaiseCooldown = false;
    bool liftLowerCooldown = false;
    bool joystickControl = false;
    bool clawCooldown = false;

	while (true) {
        //! Initialise############################################################
        if(!driveInitialise) {
            drive.reset_drive();
            drive.set_direction(Trees);// Set starting drive direction
            driveInitialise = true;
        }
        if(!liftInitialise && lift.zeroLift()) {
            liftInitialise = true;
        }
        if(!clawInitialise && claw.zeroLift(100)) {
            clawInitialise = true;
        }


        //! LCD###################################################################
        switch (displayPage) {
            case 1://Default Page
                pros::lcd::set_text(0, "Tree Hugger----Home-------Page 1");
                pros::lcd::print(1, "Drive Init=%d  Lift Init=%d", driveInitialise, liftInitialise);
                pros::lcd::print(2, "Drive direction: %s", drive.get_direction());
                pros::lcd::print(3, "Drive Pos=%06.0f", drive.get_position());
                pros::lcd::print(4, "Lift Pos = %04.0f", lift.get_position());
                pros::lcd::print(5, "Grab Action = %s", grabAction);
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
            case 4://Claw debug page
                pros::lcd::set_text(0, "Tree Hugger----Claw---------Page 4");
                pros::lcd::print(1, "Lift position = %04.0f", claw.get_lift_position());
              	pros::lcd::print(2, "Button = %d", claw.get_button());
                pros::lcd::print(3, "Claw voltage=%05.0f", claw.get_voltage());
              	pros::lcd::print(4, "Claw closed = %d", clawClosed);
    	        pros::lcd::print(5, "Claw torque = %.04f", claw.get_torque());
    	        pros::lcd::print(6, "Claw pos=%04.0f", claw.get_grabber_pos());
    	        pros::lcd::print(7, "Previous      Home          Next");
                break;
            case 5://template page
                pros::lcd::set_text(0, "Tree Hugger---------------Page 5");
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


		//! Drive#################################################################
        if(driveInitialise) {
            driveSpeed = master.get_analog(ANALOG_RIGHT_Y)/127.0;
            driveSpeed = driveSpeed*200;
            rotateSpeed = master.get_analog(ANALOG_RIGHT_X)/127.0;
            rotateSpeed = rotateSpeed*120;

            //uses the 2 speed values from the controller to move the robot at a proportional velocity
            drive.move_velocity(driveSpeed, rotateSpeed);

            // Intaking donuts
            if(master.get_digital(DIGITAL_X)) {
                drive.set_direction(Donuts);
                clawJoystick = false;
                claw.move_lift_to(0, 150);
            }
            // Intaking trees
            if(master.get_digital(DIGITAL_A)) {
                drive.set_direction(Trees);
                clawJoystick = false;
                claw.move_lift_to(840, 200);
            }
        }


		//! Lift###################################################################
        if(liftInitialise) {
            if(master.get_digital(DIGITAL_L2)) {
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


        if(clawInitialise) {
            //* Claw Grabber######################################################
            if(master.get_digital(DIGITAL_DOWN) && !clawCooldown) {
                clawClosed = !clawClosed;
                clawCooldown = true;
            }
            if(!master.get_digital(DIGITAL_DOWN)) {
                clawCooldown = false;
            }

            if(clawClosed) {
                if(claw.get_grabber_pos() < 180) {
                    claw.close_voltage(9000);
                } else {
                    claw.close_voltage(2000);
                }
            } else {
                claw.open_claw();
            }

            //* Claw Lift#########################################################
            if(abs(master.get_analog(ANALOG_LEFT_Y)) > 0) {
                grabState = 0;
                clawResetState = 3;
                clawJoystick = true;
            }
            if(clawJoystick) {
                if(!master.get_digital(DIGITAL_L2)) {
                    claw.move_lift(master.get_analog(ANALOG_LEFT_Y)*0.6);
                } else {
                    claw.move_lift(0);
                }
            }

            //* Grab Sequence#####################################################
            if(master.get_digital(DIGITAL_L1)) {
                clawJoystick = false;
                clawResetState = 0;
                switch (grabState) {
                    case 0:// Idle
                        grabAction = "grabbing";
                        grabState = 1;
                        break;
                    case 1:// Close claw
                        clawClosed = true;
                        if(claw.get_grabber_pos() > 180) {
                            grabState = 2;
                        }
                        break;
                    case 2:// Move lift
                        claw.move_lift_to(1660, 200);
                        if(claw.get_lift_position() > 1655) {
                            stateEntryTime = pros::millis();
                            grabState = 3;
                        }
                        break;
                    case 3://Wait for bounce to not be bounce
                        if(pros::millis() - stateEntryTime > 210) {
                            stateEntryTime = pros::millis();
                            grabState = 4;
                        }
                        break;
                    case 4:// Release donut
                        clawClosed = false;
                        if(pros::millis()-stateEntryTime > 250) {
                            grabState = 5;
                        }
                        break;
                    case 5:// Lower lift
                        claw.move_lift(-150);
                        if(claw.get_button()) {
                            claw.move_lift(0);
                            grabState = 6;
                        }
                        break;
                    case 6:// Null state
                        break;
                }
            } else {
                grabState = 0;
                switch (clawResetState) {
                    case 0:// Idle
                        grabAction = "releasing";
                        clawResetState = 1;
                        break;
                    case 1:// Open claw and lower lift
                        clawClosed = false;
                        claw.move_lift(-150);
                        if(claw.get_button()) {
                            claw.move_lift(0);
                            clawResetState = 3;
                        }
                        break;
                    case 3:// Null state
                        break;
                }
            }

        }

		pros::delay(20);
	}
}