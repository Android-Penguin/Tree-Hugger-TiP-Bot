#include "main.h"
#include "./../global_variables.h"
#include "pros/adi.hpp"
#include "pros/llemu.hpp"
#include "pros/motors.h"
#include "pros/rtos.hpp"

class Lift {
    pros::Motor* liftLeft;
    pros::Motor* liftRight;
    pros::ADIButton* liftLimitButton;

public:
    // Class variables
    int liftPresetPosition = 0;

    Lift() {
        liftLeft = new pros::Motor(11, true);
        liftRight = new pros::Motor(12);
        liftLimitButton = new pros::ADIButton(8);

        liftLeft->set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    	liftRight->set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    }

    // Raises/lowers the lift to a target position based on the degrees turned on the pinion gear
    void move_to(double targetPosition, int speed) {
        liftLeft->move_absolute(targetPosition, speed);
    }

    // Raises/lowers the lift at a set speed
    void move_velocity(int speed, bool override=false) {
        liftPresetPosition = 0;//Reset preset position
        if(speed < 0 && !liftLimitButton->get_value()) {// Raising lift
            liftLeft->move_velocity(speed);
        } else if(speed > 0 && liftLeft->get_position() < pickup) {// Lowering lift
            liftLeft->move_velocity(speed);
        } else if(speed > 0 && override) {
            liftLeft->move_velocity(speed/4);
        } else {
            liftLeft->move_velocity(0);
        }
    }

    // Returns the current position of the lift in degrees turned on the pinion gear
    double get_position() {
        return liftLeft->get_position();
    }

    // Returns pressed state of the lift hard stop (1=pressed 0=not pressed)
    int get_button() {
        return liftLimitButton->get_value();
    }

    // Moves the lift between preset positions
    void shift_position(int direction) {
        if(direction == UP && liftPresetPosition < 2) {
            liftPresetPosition ++;
        } else if(direction == DOWN && liftPresetPosition > 0) {
            liftPresetPosition --;
        }

        switch (liftPresetPosition) {
            case 0://Pickup position
                liftLeft->move_absolute(pickup, 200);
                break;
            case 1://Lift raised, holding tree
                liftLeft->move_absolute(raised, 200);
                break;
            case 2://Folded into robot
                liftLeft->move_absolute(folded, 200);
                break;
        }
    }

    // Lift reset sequence; raises lift to limit switch then returns lift to pickup position
    bool zeroLift() {
        static int zeroSequenceState;
        static int functionEntryTime;

        switch (zeroSequenceState) {
            case 0:// Set function entry time
                functionEntryTime = pros::millis();
                zeroSequenceState = 1;
                break;
            case 1:// Raise lift
                liftLeft->move_velocity(-200);
                if(liftLimitButton->get_value()) {
                    liftLeft->move_velocity(0);
                    liftLeft->set_zero_position(0);
                    zeroSequenceState = 2;
                }
                break;
            case 2:// Lower lift to target posistion
                liftLeft->move_absolute(pickup, 200);
                if(liftLeft->get_position() > pickup-5) {
                    zeroSequenceState = 3;
                }
                break;
            case 3:// Return true
                pros::lcd::clear_line(7);
                return true;
                break;
        }

        pros::lcd::print(7, "Sequence State= %d", zeroSequenceState);
        liftRight->move_voltage(liftLeft->get_voltage());
        return false;
    }

    // Resets the value of the motor encoder to zero at the current position
    void reset_encoder() {
        liftLeft->set_zero_position(0);
        liftRight->set_zero_position(0);
    }

    // Called once per cycle of the main loop to update background processes
    void update() {
        /*Sets voltage of the right motor to match the voltage of the left motor
        This is done so that both motors are run at the same output level*/
        liftRight->move_voltage(liftLeft->get_voltage());
    }

};