/**
 * File that will implement the line follower 1 & 2 states for the MakeBlock.
 * The start function will be called by the main loop, and should encapsulate
 * alllogic pertaining to this state.
 */
#include "state_line_follower.h"

#include "motor.h"
#include "button.h"
#include "led.h"

#include "src/MeSingleLineFollower.h"
#include <MeMegaPi.h>

// Define the ports the the line follower peripherals are on
#define LINEFOLLOWER_S1_PIN  A9
#define LINEFOLLOWER_S2_PIN  A10

// Initialize the line follower sensors
MeSingleLineFollower line_follower_s1(LINEFOLLOWER_S1_PIN);
MeSingleLineFollower line_follower_s2(LINEFOLLOWER_S2_PIN);

/**
 * Gets the current state of the sensors.
*/
enum LineFollowingStates get_state(void) {
    int s1 = line_follower_s1.readSensor();
    int s2 = line_follower_s2.readSensor();
    enum LineFollowingStates state = SEN1_IN_SEN2_IN;
   if (s1 == 1 && s2 == 1) {
        state = SEN1_IN_SEN2_IN;
    }
    else if (s1 == 0 && s2 == 1) {
        state = SEN1_OUT_SEN2_IN;
    }
    else if (s2 == 1 && s2 == 0) {
        state = SEN1_IN_SEN2_OUT;
    }
    else {
        state = SEN1_OUT_SEN2_OUT;
    }
    return state;
}

/**
 * The start point for the line follower 1 & 2 states. This function will return
 * once a line is no longer detected.
 *
 * @retval true  A line is no longer detected.
 * @retval false An error occured.
*/
bool state_line_follower_start(void) {
    enum LineFollowingStates sensor_state = get_state();
    while (true) {
        sensor_state = get_state();
        switch (sensor_state) {
            case SEN1_IN_SEN2_IN:
                // Continue straight
                motor_move_forward(100, 100);
                break;
            case SEN1_IN_SEN2_OUT:
                // Turn left
                motor_turn_left(100, 50);
                break;
            case SEN1_OUT_SEN2_IN:
                // Turn right
                motor_turn_right(100, 50);
                break;
            case SEN1_OUT_SEN2_OUT:
            default:
                break;
        }
    }
    return true;
}
