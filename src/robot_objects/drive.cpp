#include "main.h"
#include "./../global_variables.h"

class Drive {
    pros::Motor* driveFrontLeft;
	pros::Motor* driveFrontRight;
	pros::Motor* driveBackLeft;
	pros::Motor* driveBackRight;

public:
    // Class variables
    int driveDirection;
    
    Drive() {
        driveFrontLeft = new pros::Motor(1);
    	driveFrontRight = new pros::Motor(2, true);
    	driveBackLeft = new pros::Motor(3);
    	driveBackRight = new pros::Motor(4, true);
    }

    // Moves the drive at a set velocity
    void move_velocity(float driveSpeed, float rotateSpeed) {
        driveFrontLeft->move_velocity(driveSpeed*driveDirection + rotateSpeed);
		driveFrontRight->move_velocity(driveSpeed*driveDirection - rotateSpeed);
		driveBackLeft->move_velocity(driveSpeed*driveDirection + rotateSpeed);
		driveBackRight->move_velocity(driveSpeed*driveDirection - rotateSpeed);
    }

    // Moves the drive at set voltage
    void move_voltage(float driveSpeed, float rotateSpeed) {
        driveFrontLeft->move_voltage(driveSpeed*driveDirection + rotateSpeed);
		driveFrontRight->move_voltage(driveSpeed*driveDirection - rotateSpeed);
		driveBackLeft->move_voltage(driveSpeed*driveDirection + rotateSpeed);
		driveBackRight->move_voltage(driveSpeed*driveDirection - rotateSpeed);
    }

    // Changes direction of drive
    void set_direction(int direction) {
        driveDirection = direction;
    }

    // Returns direction of drive
    std::string get_direction() {
        if(driveDirection == Donuts) {
            return "Donuts";
        } else if(driveDirection == Trees) {
            return "Trees";
        } else {
            return "Direction Error";
        }
    }

    // Reset drive
    void reset_drive() {
        driveFrontLeft->set_zero_position(0);
        driveFrontRight->set_zero_position(0);
        driveBackLeft->set_zero_position(0);
        driveBackRight->set_zero_position(0);
    }

};