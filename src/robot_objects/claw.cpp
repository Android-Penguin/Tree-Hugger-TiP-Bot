#include "main.h"
#include "./../global_variables.h"
#include "pros/adi.hpp"
#include "pros/llemu.hpp"
#include "pros/motors.h"
#include "pros/rtos.hpp"

class Claw {
    pros::Motor* clawGrabber;
    pros::Motor* clawLift;
    pros::ADIButton* clawLimitButton;

public:
    // Class variables
    int liftPresetPosition = 0;

    Claw() {
        clawGrabber = new pros::Motor(13);
        clawLift = new pros::Motor(14);
        clawLimitButton = new pros::ADIButton(1);

        clawLift->set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    }

    // ###########
    // # Grabber #
    // ###########

    // Closes claw at a voltage
    void close_voltage(int voltage) {
        clawGrabber->move_voltage(abs(voltage));
    }

    //Open claw
    void open_claw() {
        clawGrabber->move_absolute(0, 100);
    }

    // Return torque
    double get_torque() {
        return clawGrabber->get_torque();
    }

    // Return voltage
    double get_voltage() {
        return clawGrabber->get_voltage();
    }

    // Return claw pos
    double get_grabber_pos() {
        return clawGrabber->get_position();
    }

    // ########
    // # Lift #
    // ########

    // Move lift at a speed (-200 to 200)
    void move_lift(int speed) {
        if(speed < 0) {
            if(!clawLimitButton->get_value()) {
                clawLift->move_velocity(speed);
            }
        } else {
            clawLift->move_velocity(speed);
        }
    }

    void move_lift_to(double position, int speed) {
        clawLift->move_absolute(position, speed);
    }

    // Lift reset sequence; raises lift to limit switch then returns lift to pickup position
    bool zeroLift(int speed) {
        static int zeroSequenceState;
        static int functionEntryTime;

        switch (zeroSequenceState) {
            case 0:// Set function entry time
                functionEntryTime = pros::millis();
                zeroSequenceState = 1;
                break;
            case 1:// Lower lift
                clawLift->move_velocity(-speed);
                if(clawLimitButton->get_value()) {
                    clawLift->move_velocity(0);
                    clawLift->set_zero_position(0);
                    zeroSequenceState = 2;
                }
                break;
            case 2:// Return true
                return true;
                break;
        }

        return false;
    }

    // Retrns position of claw lift 
    double get_lift_position() {
        return clawLift->get_position();
    }

    // Returns pressed state of the claw lift hard stop (1=pressed 0=not pressed)
    int get_button() {
        return clawLimitButton->get_value();
    }

};