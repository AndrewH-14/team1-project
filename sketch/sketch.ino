/**
 * Main file for the MakeBlock program.
 *
 * Written by: AIRE Team
*/
#include "state_line_follower.h"
#include "state_extraction.h"
#include "state_rendezvous_verify.h"
#include "led.h"

/**
 * All possible states for the MakeBlock
 */
enum MakeBlockStates {
    STATE_LINE_FOLLOWER_1,
    STATE_EXTRACTION,
    STATE_RENDEZVOUS_VERIFY,
    STATE_LINE_FOLLOWER_2,
    STATE_DONE
};

// Stores the current state of the MakeBlock
enum MakeBlockStates g_cur_state;

/**
 * Mandatory function that will be called everytime the board is powered up or
 * reset.
*/
void setup(void) {
    // Call any setup functions for peripherals
    void setup_motor();
    // Set MakeBlock to the initial state
    g_cur_state = STATE_LINE_FOLLOWER_1;
}

/**
 * Mandatory function that will loop repeatedly until the board is powered off
 * or reset. Implements a state machine, that will transition the MakeBlock
 * from one mode to the next.
 *
 * STATE_LINE_FOLLOWER_1:
 * In this state the MakeBlock will detect the line that it is placed on, and
 * follow it until it is no longer detected. The line will be a piece of blue
 * tape on a light colored floor. The MakeBlock should handle turns up to 90
 * degrees, and potentially change speed depending on the angle.
 *
 * STATE_EXTRACTION:
 * In this state the MakeBlock will enter a roomba like mode, where it will
 * traverse around an obstacle field until a hand signal is detected.
 *
 * STATE_RENDEZVOUS_VERIFY:
 * In this state the MakeBlock verify that the data is valid. This will be done
 * by detecting a correct sequence of hand signals, e.g. Left, Right, Left, Left.
 *
 * STATE_LINE_FOLLOWER_2:
 * In this state the MakeBlock will once again detect the line that it is placed
 * on, and follow it until it is no longer detected. This will follow the same
 * line conditions as mentioned above.
 *
 * STATE_DONE:
 * This state will signal the end of the mission, and should stop the robot.
 * The synthetic dataset of the hardware profile of the MakeBlock will then be
 * extracted and displayed on the GUI.
*/
void loop(void) {
    switch (g_cur_state) {
        case STATE_LINE_FOLLOWER_1:
            led_set_color(0, 0, 255);
            // Returns once the button is pressed
            if (state_line_follower_start()) {
                g_cur_state = STATE_EXTRACTION;
            }
            break;
        case STATE_EXTRACTION:
            // Returns once the hand signal is detected
            led_set_color(255, 255, 0);
            if (state_extraction_start()) {
                g_cur_state = STATE_RENDEZVOUS_VERIFY;
            }
            break;
        case STATE_RENDEZVOUS_VERIFY:
            // Returns once sensitive data has been recovered
            led_set_color(255, 0, 255);
            if (state_rendezvous_verify_start()) {
                g_cur_state = STATE_LINE_FOLLOWER_2;
            }
            break;
        case STATE_LINE_FOLLOWER_2:
            // Returns once the line is not detected
            led_set_color(0, 255, 255);
            if (state_line_follower_start()) {
                g_cur_state = STATE_DONE;
            }
            break;
        case STATE_DONE:
            // Set LED to green to signal successful mission.
            led_set_color(0, 255, 0);
            break;
        default:
            // Set LED to red to signal an error
            led_set_color(255, 0, 0);
            break;
    }
}
