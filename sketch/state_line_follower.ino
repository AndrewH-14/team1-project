/**
 * File that will implement the line follower 1 & 2 states for the MakeBlock.
 * The start function will be called by the main loop, and should encapsulate
 * alllogic pertaining to this state.
 */
#include "state_line_follower.h"

#include "motor.h"
#include <MeMegaPi.h>

// TODO: Determine coorect port to use
MeLineFollower lineFinder(PORT_3);

/**
 * The start point for the line follower 1 & 2 states. This function will return
 * once a line is no longer detected.
 *
 * @retval true  A line is no longer detected.
 * @retval false An error occured.
*/
bool state_line_follower_start(void) {
    int sensor_state = lineFinder.readSensors();
    while (sensor_state != S1_OUT_S2_OUT) {
        sensor_state = lineFinder.readSensors();
        switch (sensor_state) {
            case S1_IN_S2_IN:
                // Continue straight
                motor_move_forward(100, 100);
                break;
            case S1_IN_S2_OUT:
                // Turn left
                motor_turn_left(100, 50);
                break;
            case S1_OUT_S2_IN:
                // Turn right
                motor_turn_right(100, 50);
                break;
            case S1_OUT_S2_OUT:
            default:
                break;
        }
    }
    return true;
}
