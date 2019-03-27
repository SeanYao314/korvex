#include "main.h"
#include "okapi/api.hpp"
#include "korvexlib.h"

// motors
pros::Motor chassisLeft1(LEFT_MTR1, pros::E_MOTOR_GEARSET_18, false);
pros::Motor chassisLeft2(LEFT_MTR2, pros::E_MOTOR_GEARSET_18, false);
pros::Motor chassisRight1(RIGHT_MTR1, pros::E_MOTOR_GEARSET_18, false);
pros::Motor chassisRight2(RIGHT_MTR2, pros::E_MOTOR_GEARSET_18, false);

okapi::MotorGroup flywheelController({-FLY_MTR1, -FLY_MTR2});
pros::Motor intakeMotor(INTAKE_MTR, pros::E_MOTOR_GEARSET_18, false);
pros::Motor capflipMotor(CAPFLIP_MTR, pros::E_MOTOR_GEARSET_18, false);

// intake triggers
pros::ADIDigitalIn triggerBL(TRIGGER_BL);
pros::ADIDigitalIn triggerBR(TRIGGER_BR);
pros::ADIDigitalIn triggerTL(TRIGGER_TL);
pros::ADIDigitalIn triggerTR(TRIGGER_TR);

// other
pros::Controller controllerPros(pros::E_CONTROLLER_MASTER);

// okapi factories
using namespace okapi;

okapi::ChassisControllerPID chassis = okapi::ChassisControllerFactory::create(
    {LEFT_MTR2, LEFT_MTR1},     // Left motors (the sensor vals are read from first motor on each side)
    {-RIGHT_MTR2, -RIGHT_MTR1}, // Right motors

    IterativePosPIDController::Gains{0.003, 0.0012, 0.000012}, // distance args
    IterativePosPIDController::Gains{0.0004, 0.0005, 0.0002},  // angle args (keeps robot straight)
    IterativePosPIDController::Gains{0.003, 0.0065, 0.000045},  // turn args

    AbstractMotor::gearset::green, // normal gearset
    {4_in, 12.5_in}                // 4 inch wheels, 12.5 inch wheelbase width
);
auto myChassis = ChassisControllerFactory::create(
    {5, 2}, {3,4},
    AbstractMotor::gearset::green,
    {4_in, 12.5_in}
);

okapi::ADIGyro gyro(1, 1); // port, multiplier
