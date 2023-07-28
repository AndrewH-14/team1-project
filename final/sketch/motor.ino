#include "motor.h"

#include <MeMegaPi.h>

// Motor Macros
#define MOTOR_FRONT_RIGHT PORT1A
#define MOTOR_BACK_RIGHT  PORT1B
#define MOTOR_FRONT_LEFT  PORT1A
#define MOTOR_BACK_LEFT   PORT1A
MeMegaPiDCMotor motor1(PORT1A);
MeMegaPiDCMotor motor2(PORT1B);
MeMegaPiDCMotor motor3(PORT2A);
MeMegaPiDCMotor motor4(PORT2B);

/**
 * Function that will stop all motor activity.
 */
static void motor_stop(void) {
    motor1.stop();
    motor2.stop();
    motor3.stop();
    motor4.stop();
}

/**
 * Function to move the MakeBlock forwards.
 *
 * @param speed   The speed at which the makeblock should move forwards. This
 *                value should be [0, 255].
 * @param time_ms The amount of time in milliseconds that the makeblock will
 *                move forwards.
 */
void motor_move_forward(uint8_t speed, uint16_t time_ms)
{
  motor_stop();
  motor1.run(speed);
  motor2.run(speed);
  motor3.run(-speed);
  motor4.run(-speed);
  delay(time_ms);
  motor_stop();
}

/**
 * Function to move the MakeBlock backwards.
 *
 * @param speed   The speed at which the makeblock should move backwards. This
 *                value should be [0, 255].
 * @param time_ms The amount of time in milliseconds that the makeblock will
 *                move backwards.
 */
void motor_move_backward(uint8_t speed, uint16_t time_ms)
{
  motor_stop();
  motor1.run(-speed);
  motor2.run(-speed);
  motor3.run(speed);
  motor4.run(speed);
  delay(time_ms);
  motor_stop();
}

/**
 * Function to turn the MakeBlock to the right.
 *
 * @param speed   The speed at which the makeblock should turn left. This
 *                value should be [0, 255].
 * @param time_ms The amount of time in milliseconds that the makeblock will
 *                turn left.
 */
void motor_turn_left(uint8_t speed, uint16_t time_ms)
{
  motor_stop();
  motor1.run(-speed);
  motor2.run(-speed);
  motor3.run(-speed);
  motor4.run(-speed);
  delay(time_ms);
  motor_stop();
}

/**
 * Function to turn the MakeBlock to the right.
 *
 * @param speed   The speed at which the makeblock should turn right. This
 *                value should be [0, 255].
 * @param time_ms The amount of time in milliseconds that the makeblock will
 *                turn right.
 */
void motor_turn_right(uint8_t speed, uint16_t time_ms)
{
  motor_stop();
  motor1.run(-speed);
  motor2.run(-speed);
  motor3.run(-speed);
  motor4.run(-speed);
  delay(time_ms);
  motor_stop();
}

/**
 * Function to move the MakeBlock to the move directly to the left
 * without turning.
 *
 * @param speed   The speed at which the makeblock should move left. This
 *                value should be [0, 255].
 * @param time_ms The amount of time in milliseconds that the makeblock will
 *                move to the left.
 */
void motor_move_left(uint8_t speed, uint16_t time_ms) {
    motor_stop();
    motor1.run(-(speed / 2));
    motor2.run(speed / 2);
    motor3.run(speed / 2);
    motor4.run(-(speed / 2));
    delay(time_ms);
    motor_stop();
}

/**
 * Function to move the MakeBlock to the move directly to the right
 * without turning.
 *
 * @param speed   The speed at which the makeblock should move right. This
 *                value should be [0, 255].
 * @param time_ms The amount of time in milliseconds that the makeblock will
 *                move to the right.
 */
void motor_move_right(uint8_t speed, uint16_t time_ms) {
    motor_stop();
    motor1.run(speed / 2);
    motor2.run(-(speed / 2));
    motor3.run(-(speed / 2));
    motor4.run(speed / 2);
    delay(time_ms);
    motor_stop();
}
