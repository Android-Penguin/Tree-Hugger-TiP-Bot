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
        clawLimitButton = new pros::ADIButton(8);

        clawLift->set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    }


    // Closes claw at a voltage
    void close_voltage(int voltage) {
        clawGrabber->move_voltage(voltage);
    }

    //Open claw
    void open_claw() {
        clawGrabber->move_absolute(0, 50);
    }

    // Move lift
    void move_lift(int speed) {
        if(speed < 0) {
            if(!clawLimitButton->get_value()) {
                clawLift->move_velocity(speed);
            }
        } else {
            clawLift->move_velocity(speed);
        }
    }

};