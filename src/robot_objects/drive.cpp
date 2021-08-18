#include "main.h"
#include "./../global_variables.h"
#include "pros/llemu.hpp"
#include "pros/rtos.hpp"

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
    void move_velocity(double driveSpeed, double rotateSpeed) {
        driveFrontLeft->move_velocity(driveSpeed*driveDirection + rotateSpeed);
		driveFrontRight->move_velocity(driveSpeed*driveDirection - rotateSpeed);
		driveBackLeft->move_velocity(driveSpeed*driveDirection + rotateSpeed);
		driveBackRight->move_velocity(driveSpeed*driveDirection - rotateSpeed);
    }

    // Moves the drive at set voltage
    void move_voltage(double driveSpeed, double rotateSpeed) {
        driveFrontLeft->move_voltage(driveSpeed*driveDirection + rotateSpeed);
		driveFrontRight->move_voltage(driveSpeed*driveDirection - rotateSpeed);
		driveBackLeft->move_voltage(driveSpeed*driveDirection + rotateSpeed);
		driveBackRight->move_voltage(driveSpeed*driveDirection - rotateSpeed);
    }

    // Changes direction of drive between intaking trees and intaking donuts
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

    // Returns position of selected wheel
    double wheel_position(int target_wheel) {
        switch (target_wheel) {
            case FrontLeft:
                return driveFrontLeft->get_position();
                break;
            case FrontRight:
                return driveFrontRight->get_position();
                break;
            case BackLeft:
                return driveBackLeft->get_position();
                break;
            case BackRight:
                return driveBackRight->get_position();
                break;
            default:
                return 0;
                break;
        }
    }

    // Returns the encoder value of the wheel that has travelled the least distance
    double get_position() {
        // Creates array with wheel positions
        double wheel_positions [4] = {
            static_cast<double>(std::abs(driveFrontLeft->get_position())),
            static_cast<double>(std::abs(driveFrontRight->get_position())),
            static_cast<double>(std::abs(driveBackLeft->get_position())),
            static_cast<double>(std::abs(driveBackRight->get_position()))
        };
        // Min value initial
        double min_value = wheel_positions[0];
        for(int i=0; i<4; i++) {
            if(wheel_positions[i] < min_value) {
                min_value = wheel_positions[i];
            }
        }
        return min_value;
    }

    // Moves the drive a set distance in encoder units with motion profile
    bool move_for(double targetDistance, int direction=Trees) {
        // Sequence variables
        static int moveSequenceState;
        static int stateEntryTime;
        // Ramp constants
        static const double rampUpDistance = 10000;// Unit: encoder counts
        static const double rampUpTime = 5000;// Unit: milliseconds
        static const double rampDownDistance = 10000;// Unit: encoder counts
        static const double rampDownTime = 500;// Unit: milliseconds
        // Calculated values
        static double accelerateDistance;
        static double constantVelocityDistance;
        static double decelerateDistance;
        static double speed;

        switch (moveSequenceState) {
            case 0:// Calculate distances
                reset_drive();
                set_direction(direction);
                // Distance greater than or equal to ramp times
                if(targetDistance >= (rampUpDistance+rampDownDistance)) {
                    accelerateDistance = rampUpDistance;
                    constantVelocityDistance = targetDistance-(rampUpDistance+rampDownDistance);
                    decelerateDistance = rampDownDistance;
                }
                // Distance less than ramp times
                else if(targetDistance < (rampUpDistance+rampDownDistance)) {
                    accelerateDistance = targetDistance/2.0;
                    constantVelocityDistance = 0;
                    decelerateDistance = targetDistance/2.0;
                }
                if(accelerateDistance != 0) {
                    stateEntryTime = pros::millis();
                    moveSequenceState = 1;
                }
                break;
            case 1:// Ramp up
                pros::lcd::print(3, "Time:%d", (pros::millis()-stateEntryTime));
                if(get_position() <= accelerateDistance) {
                    speed = (200.0/rampUpTime)*(pros::millis()-stateEntryTime);
                    move_velocity(speed, 0);
                } else {
                    moveSequenceState = 2;
                }
                break;
            case 2:// Constant speed
                if(get_position() <= (accelerateDistance+constantVelocityDistance)) {
                    move_velocity(speed, 0);
                } else {
                    moveSequenceState = 3;
                }
                break;
            case 3:
                break;
        }
        pros::lcd::print(2, "Move State = %d", moveSequenceState);
        pros::lcd::print(4, "Speed = %f", speed);
        pros::lcd::print(5, "Drive value = %f", get_position());
        pros::lcd::print(6, "Accel=%.0f, Const=%.0f", accelerateDistance, constantVelocityDistance);
        pros::lcd::print(7, "Decel=%.0f", decelerateDistance);
        return false;
    }

    // Resets drive encoders
    void reset_drive() {
        driveFrontLeft->set_zero_position(0);
        driveFrontRight->set_zero_position(0);
        driveBackLeft->set_zero_position(0);
        driveBackRight->set_zero_position(0);
    }

};