#include "main.h"
#include "okapi/api.hpp"
#include "korvexlib.h"
using namespace okapi;

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */

void gyroTurn(int target, int timeoutVal)
{

    // tune vals
    float kP = 0.002;
    float kI = 0.005;
    float kD = 0.0004;
    int iDeadband = 400; // 0 for disable
    int tolerance = 0;

    // vars
    target = target + gyro.get(); // relative
    float p = 0;
    float i = 0;
    float d = 0;
    int gyroVal = 0;
    int error = 0;
    int lastError = 0;
    int drive = 0;
    bool timeout = false;
    bool stable = false;
    int startTime = pros::millis();

    while (!timeout && !stable)
    {

        // calculate error
        gyroVal = gyro.get();
        std::cout << "gyro " << gyro.get() << std::endl;
        error = (target - gyroVal);

        // calculate pid
        p = (error * kP);
        if (abs(error) < iDeadband || iDeadband == 0)
            i = ((i + error) * kI);
        else
            i = 0;
        d = ((error - lastError) * kD);

        // store last error
        lastError = error;

        // calculate drive
        drive = (p + i + d);

        // cap vals
        if (drive > 1)
            drive = 1;
        if (drive < -1)
            drive = -1;

        chassis.rotate(drive);

        if (startTime + timeoutVal < pros::millis()) // timeout
            timeout = true;

        if (abs(error) - tolerance <= 0 && abs(lastError) - tolerance <= 0)
            stable = true;

        pros::delay(10);
    }
    chassis.rotate(0);
}
/*void aligner()
{
    while(true){
        int screen_middle_x = 316/2; // 316/2 is mid
      if(vision_sensor.get_by_size(pros::Vision vision_sensor (11, E_VISION_ZERO_CENTER))

    }
}*/
void autonomous()
{
    int autonStart = pros::millis(); // note the start time
    chassis.resetSensors();
    int auton = 5;

    if (autonSelection != 10)
    {
        auton = autonSelection; // this is to enable auton selector
    }

    switch (auton)
    {
    case 5: // test
        chassis.setMaxVelocity(130);
        // chassis.turnAngle(-90_deg);
        chassis.turnAngle(90_deg);

        // chassis.moveDistance(72_in);
        break;
    case 0: // skills 18
        chassis.setMaxVelocity(130);

        intakeMotor.move_velocity(200);
        capflipMotor.move_absolute(-450, 200);
        chassis.moveDistanceAsync(37_in); // going to cap with ball under it

        // wait until we intake ball to bot
        timeHold = pros::millis();
        while (!(triggerBL.get_new_press() || triggerBR.get_new_press()))
        {
            pros::delay(20);
        }

        // theres a ball at the top, we want to pull it down back to the trigger
        intakeMotor.move_relative(500, 200);

        // theres a ball at the top, we want to pull it down back to the trigger
        intakeMotor.move_relative(-400, 200);
        pros::delay(200);
        // there is now a ball in both positions

        // back and turn into shooting position
        flywheelController.moveVelocity(580);
        chassis.waitUntilSettled();
        chassis.moveDistance(7_in);
        capflipMotor.move_absolute(0, 200);

        // aim for flag
        chassis.turnAngle(-270); // 90 deg counter clockwise
        capflipMotor.move_absolute(-450, 200);
        chassis.moveDistance(-5_in);

        // shoot first ball when ready
        while (!(flywheelController.getActualVelocity() > 575))
        {
            pros::delay(20);
        }
        intakeMotor.move_relative(1400, 200);

        // wait for first ball to get shot
        timeHold = pros::millis();
        while (!(flywheelController.getActualVelocity() <= 430) && (timeHold + 300 > pros::millis()))
        {
            pros::delay(20);
        }

        // quick switch to mid flag, so when flywheel power lowers cuz of stress of launch, we can use the decel to improve speed
        flywheelController.moveVelocity(430);

        // wait for spinup
        pros::delay(400);

        // shoot 2nd ball
        intakeMotor.move_relative(1200, 200);
        // wait for second ball to get shot
        pros::delay(400);
        flywheelController.moveVelocity(0);

        // flip bot flag
        chassis.turnAngle(-20); // x deg for bot flag flip
        chassis.moveDistance(48_in);

        // move back to near platform
        chassis.moveDistance(-49_in);
        capflipMotor.move_absolute(0, 200);
        chassis.moveDistance(5_in); 

        // go back to start square
        chassis.turnAngle(-270);
        chassis.moveDistance(45_in);

        // turn and move to cap
        chassis.turnAngle(400);
        chassis.moveDistance(18_in);

        // scrape
        capflipMotor.move_absolute(-530, 200);
        intakeMotor.move_velocity(200);
        pros::delay(200);
        chassis.moveDistance(-8_in);
        capflipMotor.move_absolute(0, 200);
        chassis.moveDistanceAsync(-8_in);

        // wait for first ball to get to top pos
        while (!(triggerTL.get_value() || triggerTR.get_value()))
        {
            pros::delay(20);
        }

        // wait for second ball to get to bot pos
        timeHold = pros::millis();
        while (!(triggerBL.get_value() || triggerBR.get_value()) && (timeHold + 5000 > pros::millis()))
        {
            pros::delay(20);
        }

        // we got a second ball, let it pull up a bit
        intakeMotor.move_relative(400, 200);
        pros::delay(500);
        intakeMotor.move_relative(-500, 200);

        capflipMotor.move_absolute(0, 200);
        chassis.waitUntilSettled();
        flywheelController.moveVelocity(520);
        chassis.turnAngle(-120); // aim for flags
        chassis.moveDistance(-8_in);

        // shoot closest pole to red
        pros::delay(1500);
        intakeMotor.move_relative(1500, 200);
        pros::delay(300);

        // second ball
        flywheelController.moveVelocity(550);
        pros::delay(1500);
        intakeMotor.move_relative(1000, 200);
        pros::delay(500);
        flywheelController.moveVelocity(0);

        // hit bot flag
        chassis.turnAngle(-40);
        chassis.moveDistance(50_in);

        // park
        chassis.moveDistance(-70_in);
        chassis.turnAngle(270);
        chassis.moveDistance(74_in);
        break;

    case 100: // skills old
        // setup
        chassis.setMaxVelocity(150);

        // actual auton
        intakeMotor.move_velocity(200);
        capflipMotor.move_absolute(-450, 200);
        chassis.moveDistance(37_in); // going to cap with ball under it
        capflipMotor.move_absolute(0, 200);

        // wait until we intake ball to bot
        timeHold = pros::millis();
        while (!(triggerBL.get_new_press() || triggerBR.get_new_press()) && (timeHold + 2000 > pros::millis()))
        {
            pros::delay(20);
        }

        // theres a ball at the top, we want to pull it down back to the trigger
        pros::delay(100);
        intakeMotor.move_relative(-490, 200);
        // there is now a ball in both positions

        // back and turn for descore
        chassis.moveDistance(-4_in);
        flywheelController.moveVelocity(515);
        chassis.turnAngle(-54_deg);
        pros::delay(500);
        intakeMotor.move_relative(1500, 200);
        pros::delay(300);

        // second ball
        flywheelController.moveVelocity(540);
        pros::delay(1500);
        intakeMotor.move_relative(1000, 200);
        pros::delay(500);
        flywheelController.moveVelocity(0);

        // move and turn for 1st scrape cap
        chassis.turnAngle(60_deg);
        chassis.moveDistance(-7_in);
        chassis.turnAngle(270); // things are weird so 270 = 90_deg
        chassis.moveDistance(23.5_in);
        chassis.turnAngle(-80_deg);
        chassis.moveDistance(8_in);

        // scrape
        capflipMotor.move_absolute(-525, 200);
        intakeMotor.move_velocity(200);
        pros::delay(200);
        chassis.moveDistance(-10_in);
        chassis.moveDistanceAsync(-20_in);

        // wait for first ball to get to top pos
        while (!(triggerTL.get_value() || triggerTR.get_value()))
        {
            pros::delay(20);
        }
        ballTriggerTop = true;

        // wait for second ball to get to bot pos
        timeHold = pros::millis();
        while (!(triggerBL.get_value() || triggerBR.get_value()) && (timeHold + 5000 > pros::millis()))
        {
            pros::delay(20);
        }

        if (triggerBL.get_new_press() || triggerBR.get_new_press())
        {
            ballTriggerBottom = true;
        }

        // we got a second ball, let it pull up a bit
        intakeMotor.move_relative(400, 200);
        pros::delay(500);
        intakeMotor.move_relative(-500, 200);

        capflipMotor.move_absolute(0, 200);
        chassis.waitUntilSettled();
        chassis.turnAngle(-270);

        // align with wall
        chassis.forward(-1);
        pros::delay(300);
        chassis.forward(-0.2);
        pros::delay(500);
        chassis.stop(); // let it settle
        pros::delay(500);

        // move for front flags, start with closest to red
        flywheelController.moveVelocity(430);
        chassis.moveDistance(67_in);
        chassis.turnAngleAsync(10_deg);

        // shoot closest pole to red
        pros::delay(500);
        intakeMotor.move_relative(1500, 200);
        pros::delay(300);

        // second ball
        flywheelController.moveVelocity(540);
        pros::delay(1500);
        intakeMotor.move_relative(1000, 200);
        pros::delay(500);
        flywheelController.moveVelocity(0);

        // flip bot flag
        chassis.turnAngle(-12_deg);
        chassis.moveDistance(57_in);

        // move to line up with 2nd scrape cap
        chassis.moveDistance(-43_in);
        chassis.turnAngle(40_deg);
        chassis.moveDistance(14_in);

        // scrape
        capflipMotor.move_absolute(-530, 200);
        intakeMotor.move_velocity(200);
        pros::delay(200);
        chassis.moveDistance(-20_in);
        chassis.moveDistanceAsync(6_in);

        // wait for first ball to get to top pos
        while (!(triggerTL.get_value() || triggerTR.get_value()))
        {
            pros::delay(20);
        }
        ballTriggerTop = true;

        // wait for second ball to get to bot pos
        timeHold = pros::millis();
        while (!(triggerBL.get_value() || triggerBR.get_value()) && (timeHold + 5000 > pros::millis()))
        {
            pros::delay(20);
        }

        if (triggerBL.get_new_press() || triggerBR.get_new_press())
        {
            ballTriggerBottom = true;
        }

        // we got a second ball, let it pull up a bit
        intakeMotor.move_relative(400, 200);
        pros::delay(500);
        intakeMotor.move_relative(-500, 200);

        capflipMotor.move_absolute(-450, 200);
        chassis.waitUntilSettled();

        // line up for middle pole
        chassis.turnAngle(50_deg);

        // spin up and move close to 2nd cap with ball under
        chassis.moveDistance(34_in);
        flywheelController.moveVelocity(440);

        // turn and shoot mid pole
        chassis.turnAngle(-90_deg);
        capflipMotor.move_absolute(0, 200);

        // shoot closest pole to red
        pros::delay(500);
        intakeMotor.move_relative(1500, 200);
        pros::delay(300);

        // second ball
        flywheelController.moveVelocity(550);
        pros::delay(1500);
        intakeMotor.move_relative(1000, 200);
        pros::delay(500);
        flywheelController.moveVelocity(0);

        // back and turn for park
        chassis.turnAngle(90_deg);
        chassis.moveDistance(-8_in);
        chassis.turnAngle(90_deg);

        // align with wall
        chassis.forward(1);
        pros::delay(300);
        chassis.forward(0.2);

        chassis.moveDistance(26_in);
        chassis.turnAngle(-90_deg);
        chassis.moveDistance(30_in);

        break;
    case 1: // blue close, full post, scrape and mid post
        chassis.setMaxVelocity(130);

        intakeMotor.move_velocity(200);
        chassis.moveDistance(37_in); // going to cap with ball under it
        pros::delay(200);
        chassis.moveDistanceAsync(-38_in);

        // wait until we intake ball to bot
        timeHold = pros::millis();
        while (!(triggerBL.get_new_press() || triggerBR.get_new_press()))
        {
            pros::delay(20);
        }

        // theres a ball at the top, we want to pull it down back to the trigger
        intakeMotor.move_relative(500, 200);

        // theres a ball at the top, we want to pull it down back to the trigger
        intakeMotor.move_relative(-400, 200);
        pros::delay(200);
        // there is now a ball in both positions

        // back and turn into shooting position
        flywheelController.moveVelocity(580);
        chassis.waitUntilSettled();

        // aim for flag
        chassis.turnAngle(98_deg); 
        chassis.moveDistance(-7_in);

        // shoot first ball when ready
        while (!(flywheelController.getActualVelocity() > 575))
        {
            pros::delay(20);
        }
        intakeMotor.move_relative(1400, 200);

        // wait for first ball to get shot
        timeHold = pros::millis();
        while (!(flywheelController.getActualVelocity() <= 430) && (timeHold + 300 > pros::millis()))
        {
            pros::delay(20);
        }

        // quick switch to mid flag, so when flywheel power lowers cuz of stress of launch, we can use the decel to improve speed
        flywheelController.moveVelocity(430);

        // wait for spinup
        pros::delay(400);

        // shoot 2nd ball
        intakeMotor.move_relative(1200, 200);
        // wait for second ball to get shot
        pros::delay(400);
        chassis.setMaxVelocity(150);
        flywheelController.moveVelocity(0);

        // flip bot flag
        chassis.turnAngle(14_deg);
        chassis.moveDistance(50_in);

        // move back for scrape
        chassis.setMaxVelocity(180);
        chassis.moveDistance(-35_in);

        // turn to scrape cap
        flywheelController.moveVelocity(550); // feed-through shot velocity
        chassis.turnAngle(-45_deg);

        // move to cap
        chassis.moveDistance(18_in);

        // scrape
        capflipMotor.move_absolute(-530, 200);
        intakeMotor.move_velocity(200);
        pros::delay(200);
        chassis.moveDistance(-10_in);
        chassis.turnAngle(10_deg);
        capflipMotor.move_absolute(0, 200);
        chassis.moveDistanceAsync(5_in);

        // wait for first ball to get to top pos
        while (!(triggerTL.get_value() || triggerTR.get_value()))
        {
            pros::delay(20);
        }

        // wait for second ball to get to bot pos
        timeHold = pros::millis();
        while (!(triggerBL.get_value() || triggerBR.get_value()) && (timeHold + 5000 > pros::millis()))
        {
            pros::delay(20);
        }

        // we got a second ball, let it pull up a bit
        intakeMotor.move_relative(400, 200);
        pros::delay(500);
        intakeMotor.move_relative(-500, 200);

        capflipMotor.move_absolute(0, 200);
        chassis.waitUntilSettled();
        flywheelController.moveVelocity(520);
        chassis.turnAngle(15_deg); // aim for flags

        // shoot closest pole to red
        pros::delay(500);
        intakeMotor.move_relative(1500, 200);
        pros::delay(300);

        // second ball
        flywheelController.moveVelocity(550);
        pros::delay(1500);
        intakeMotor.move_relative(1000, 200);
        pros::delay(500);
        flywheelController.moveVelocity(0);
        break;

    case 2: // blue descore (far and cap only)
        // setup
        chassis.setMaxVelocity(130);

        // actual auton
        intakeMotor.move_velocity(200);
        chassis.moveDistance(37_in); // going to cap with ball under it

        // wait until we intake ball to bot
        timeHold = pros::millis();
        while (!(triggerBL.get_new_press() || triggerBR.get_new_press()) && (timeHold + 2000 > pros::millis()))
        {
            pros::delay(20);
        }

        // theres a ball at the top, we want to pull it down back to the trigger
        pros::delay(100);
        intakeMotor.move_relative(-400, 200);

        // there is now a ball in both positions, shoot
        chassis.moveDistance(-4_in);
        flywheelController.moveVelocity(520);
        chassis.turnAngle(61_deg);
        pros::delay(2000); // wait for them to shoot
        intakeMotor.move_relative(1500, 200);
        pros::delay(300);

        // second ball
        flywheelController.moveVelocity(545);
        pros::delay(1000);
        intakeMotor.move_relative(1000, 200);
        pros::delay(500);
        flywheelController.moveVelocity(0);

        // CAPFLIP TIME! SUPRISUWU
        chassis.setMaxVelocity(180);
        chassis.turnAngle(-60_deg);
        chassis.moveDistance(7_in);
        chassis.turnAngle(-90_deg);
        capflipMotor.move_absolute(-700, 200);

        // flip and come back
        chassis.moveDistance(10_in);
        capflipMotor.move_absolute(0, 200);

        chassis.driveVector(0.5, 0.8);
        pros::delay(500);
        // chassis.turnAngle(-180_deg);
        // chassis.moveDistance(15_in);

        // // move and turn for park
        // chassis.turnAngle(-90_deg);
        // chassis.moveDistance(-10_in);
        chassis.turnAngle(270);

        // park
        chassis.moveDistance(7_in);
        chassis.moveDistance(26_in);

        break;
    case 3: // blue descore (far only) works!!!
        // setup
        chassis.setMaxVelocity(130);

        // actual auton
        intakeMotor.move_velocity(200);
        chassis.moveDistance(37_in); // going to cap with ball under it

        // wait until we intake ball to bot
        timeHold = pros::millis();
        while (!(triggerBL.get_new_press() || triggerBR.get_new_press()) && (timeHold + 2000 > pros::millis()))
        {
            pros::delay(20);
        }

        // theres a ball at the top, we want to pull it down back to the trigger
        pros::delay(100);
        intakeMotor.move_relative(-500, 200);
        // there is now a ball in both positions

        // back and turn for descore
        chassis.moveDistance(-4_in);
        flywheelController.moveVelocity(520);
        // gyroTurn(600, 1000);
        chassis.setMaxVelocity(180);
        chassis.turnAngle(59_deg);
        chassis.setMaxVelocity(130);
        pros::delay(3000); // wait for them to shoot
        if (autonPark == false) // we can afford to wait a little longer if we arent parking
            pros::delay(2000);
        intakeMotor.move_relative(1500, 200);
        pros::delay(300);

        // second ball
        flywheelController.moveVelocity(540);
        pros::delay(1500);
        intakeMotor.move_relative(1000, 200);
        pros::delay(500);
        flywheelController.moveVelocity(0);

        if (autonPark == false) 
            pros::delay(15000);

        // move and turn for park
        chassis.turnAngle(-59_deg);
        chassis.moveDistance(-5_in);
        chassis.turnAngle(270);

        // park
        chassis.moveDistance(7_in);
        chassis.moveDistance(26_in);
        break;
    case -1: // red close, mid and top flag and park
        chassis.setMaxVelocity(180);

        intakeMotor.move_velocity(200);
        chassis.moveDistance(37_in); // going to cap with ball under it
        chassis.moveDistanceAsync(-36_in);

        // wait until we intake ball to bot
        timeHold = pros::millis();
        while (!(triggerBL.get_new_press() || triggerBR.get_new_press()))
        {
            pros::delay(20);
        }

        // theres a ball at the top, we want to pull it down back to the trigger
        intakeMotor.move_relative(500, 200);

        // theres a ball at the top, we want to pull it down back to the trigger
        intakeMotor.move_relative(-400, 200);
        pros::delay(200);
        // there is now a ball in both positions

        // back and turn into shooting position
        flywheelController.moveVelocity(580);
        chassis.waitUntilSettled();

        // aim for flag
        chassis.turnAngle(-75_deg);
        chassis.moveDistance(-5_in);

        // shoot first ball when ready
        while (!(flywheelController.getActualVelocity() > 575))
        {
            pros::delay(20);
        }
        intakeMotor.move_relative(1400, 200);

        // wait for first ball to get shot
        timeHold = pros::millis();
        while (!(flywheelController.getActualVelocity() <= 430) && (timeHold + 300 > pros::millis()))
        {
            pros::delay(20);
        }

        // quick switch to mid flag, so when flywheel power lowers cuz of stress of launch, we can use the decel to improve speed
        flywheelController.moveVelocity(430);

        // wait for spinup
        pros::delay(400);

        // shoot 2nd ball
        intakeMotor.move_relative(1200, 200);
        // wait for second ball to get shot
        pros::delay(400);
        chassis.setMaxVelocity(150);
        flywheelController.moveVelocity(0);

        // flip bot flag
        chassis.turnAngle(-8_deg);
        chassis.moveDistance(50_in);

        // move back for scrape
        chassis.setMaxVelocity(180);
        chassis.moveDistance(-35_in);

        // turn to scrape cap
        flywheelController.moveVelocity(550); // feed-through shot velocity
        chassis.turnAngle(50_deg);

        // move to cap
        chassis.moveDistance(18_in);

        // scrape
        capflipMotor.move_absolute(-530, 200);
        intakeMotor.move_velocity(200);
        pros::delay(200);
        chassis.moveDistance(-10_in);
        chassis.turnAngle(10_deg);
        capflipMotor.move_absolute(0, 200);
        chassis.moveDistanceAsync(5_in);

        // wait for first ball to get to top pos
        while (!(triggerTL.get_value() || triggerTR.get_value()))
        {
            pros::delay(20);
        }

        // wait for second ball to get to bot pos
        timeHold = pros::millis();
        while (!(triggerBL.get_value() || triggerBR.get_value()) && (timeHold + 5000 > pros::millis()))
        {
            pros::delay(20);
        }

        // we got a second ball, let it pull up a bit
        intakeMotor.move_relative(400, 200);
        pros::delay(500);
        intakeMotor.move_relative(-500, 200);

        capflipMotor.move_absolute(0, 200);
        chassis.waitUntilSettled();
        flywheelController.moveVelocity(520);
        chassis.turnAngle(-15_deg); // aim for flags

        // shoot closest pole to red
        pros::delay(500);
        intakeMotor.move_relative(1500, 200);
        pros::delay(300);

        // second ball
        flywheelController.moveVelocity(550);
        pros::delay(1500);
        intakeMotor.move_relative(1000, 200);
        pros::delay(500);
        flywheelController.moveVelocity(0);
        break;

    case -2: // red full post and park only

        break;

    case -3: // red descore (far only)
        // setup
        chassis.setMaxVelocity(130);

        // actual auton
        intakeMotor.move_velocity(200);
        chassis.moveDistance(37_in); // going to cap with ball under it

        // wait until we intake ball to bot
        timeHold = pros::millis();
        while (!(triggerBL.get_new_press() || triggerBR.get_new_press()) && (timeHold + 2000 > pros::millis()))
        {
            pros::delay(20);
        }

        // theres a ball at the top, we want to pull it down back to the trigger
        pros::delay(100);
        intakeMotor.move_relative(-500, 200);
        // there is now a ball in both positions

        // back and turn for descore
        chassis.moveDistance(-4_in);
        flywheelController.moveVelocity(520);
        // gyroTurn(600, 1000);
        chassis.setMaxVelocity(180);
        chassis.turnAngle(-51_deg);
        chassis.setMaxVelocity(130);
        pros::delay(3000); // wait for them to shoot
        intakeMotor.move_relative(1500, 200);
        pros::delay(300);

        // second ball
        flywheelController.moveVelocity(555);
        pros::delay(1500);
        intakeMotor.move_relative(1000, 200);
        pros::delay(500);
        flywheelController.moveVelocity(0);

        // move and turn for park
        chassis.turnAngle(51_deg);
        chassis.moveDistance(-3_in);
        chassis.turnAngle(-89_deg);

        // park
        chassis.moveDistance(10_in);
        chassis.moveDistance(24_in);
        break;
    default:
        break;
    }
}