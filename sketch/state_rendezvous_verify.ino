/**
 * File that will implement the rendezvous verify state for the MakeBlock.
 * The start function will be called by the main loop, and should encapsulate
 * alllogic pertaining to this state.
 */
#include "state_rendezvous_verify.h"
#include <MeMegaPi.h>
#include "src/MeBarrierSensor.h"
#include "gesture.h"

/**
 * All possible states for rendezvous mode
 */
enum RendezvousStates {
    STATE_1,
    STATE_2,
    STATE_3,
    STATE_4,
    STATE_REND_DONE
};

// Stores the current state of the MakeBlock
enum RendezvousStates cur_state;

/**
 * State logic: Robot detects hand motion on team member, switches to Rendezvous Mode
 * 3 second pause
 * Flashes LED Signal to verify robot is the correct robot and not a fake
 *
 * 3 second pause
 * LED Pulse to signal Hand Signal Read mode
 *
 * Reads incorrect hand signal (e.g. Left, Left, Left, Left)
 * Flashes LED to signal unsuccessful authentication
 *
 * LED Pulse to signal Hand Signal Read mode
 * Reads expected hand signal (e.g Left, Right Left, Left)
 * Flashes LED to confirm successful authentication
 *
*/

/**
 * The start point for the rendezvous verify state. This function will return
 * once the correct hand signal is detected.
 *
 * @retval true  The correct hand signal was detected.
 * @retval false An error occured.
*/
bool state_rendezvous_verify_start(void) {

    cur_state = STATE_1;
    delay(3000); // 3 second pause
    led_set_color(0, 255, 0); // Flash Green to verify correct robot
    gesture_model();

    return true;
}

/**
 * This function utilizes the output from detect_gesture and commands what to do.
 */
void gesture_model(void)
{
  uint8_t gesture_type;

  // State Machine that checks and validates hand signals.
  while (true) {
    // TODO: LED PULSE TO SIGNAL HAND SIGNAL READ MODE
    led_set_color(255, 0, 255); // Purple LED, but needs Pulse Feature
    gesture_type = detect_gesture();
    switch (cur_state) {
        case STATE_1:
          // Left
          if (gesture_type == GESTURE_TYPE_RIGHT_TO_LEFT) {
            cur_state = STATE_2;
          }
          else if (gesture_type == GESTURE_TYPE_NONE) {
            continue;
          }
          // If incorrect, Flash LED to signal unsuccessful
          else {
            cur_state = STATE_1;
            led_set_color(255, 0, 0); // Flash red
          }
          break;
        case STATE_2:
          // Right
          if (gesture_type == GESTURE_TYPE_LEFT_TO_RIGHT) {
            cur_state = STATE_3;
          }
          else if (gesture_type == GESTURE_TYPE_NONE) {
            continue;
          }
          else {
            cur_state = STATE_1;
            led_set_color(255, 0, 0); // Flash Red
          }
          break;
        case STATE_3:
          // Left
          if (gesture_type == GESTURE_TYPE_RIGHT_TO_LEFT) {
            cur_state = STATE_4;
          }
          else if (gesture_type == GESTURE_TYPE_NONE) {
            continue;
          }
        else {
          cur_state = STATE_1;
          led_set_color(255, 0, 0); // Flash Red
        }
          break;
        case STATE_4:
          // Left
          if (gesture_type == GESTURE_TYPE_RIGHT_TO_LEFT) {
            cur_state = STATE_REND_DONE;
          }
          else if (gesture_type == GESTURE_TYPE_NONE) {
            continue;
          }
          else {
          cur_state = STATE_1;
          led_set_color(255, 0, 0); // Flash Red
          }
          break;
        case STATE_REND_DONE:
          // Flashes Green LED to confirm success
          led_set_color(0, 255, 0);
          return;
        default:
          // Set LED to red to signal an error
          led_set_color(255, 0, 0);
          break;
    }
  }
}
