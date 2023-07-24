/**
 * File that will implement the rendezvous verify state for the MakeBlock.
 * The start function will be called by the main loop, and should encapsulate
 * alllogic pertaining to this state.
 */
#include "state_rendezvous_verify.h"
#include <MeMegaPi.h>
#include "src/MeBarrierSensor.h"


// Two sensors trigger time window，unit:ms
#define GESTURE_DIFF_TIME_MIN 20
#define GESTURE_DIFF_TIME_MAX 1000

#define GESTURE_DIFF_TIME_OUT 3000

#define PWM_MIN_OFFSET   0

#define GESTURE_MOVE_SPEED  100

// Gesture type
typedef enum
{
  GESTURE_TYPE_NONE,
  GESTURE_TYPE_LEFT_TO_RIGHT,
  GESTURE_TYPE_RIGHT_TO_LEFT,
  GESTURE_TYPE_ALL
} GESTURE_TYPE_ENUM;

/**
 * All possible states for rendezvous mode
 */
enum RendezvousStates {
    STATE_1,
    STATE_2,
    STATE_3,
    STATE_4,
    STATE_DONE
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
    static uint8_t init_led_flag = false;

    if(init_led_flag == false) {
    init_led_flag = true;
    new_rgbled_show_all(0,0,RGB_LOW_VAL,0,0,RGB_LOW_VAL,0);
    }

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
            cur_state = STATE_DONE;
          }
          else if (gesture_type == GESTURE_TYPE_NONE) {
            continue;
          }
          else {
          cur_state = STATE_1;
          led_set_color(255, 0, 0); // Flash Red
          }
          break;
        case STATE_DONE:
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

/**
 * Function that will determine and return the specific hand signal detected
*/
uint8_t detect_gesture(void)
{
  static uint32_t s_sensor1_time = 0;
  static uint32_t s_sensor2_time = 0;
  static uint32_t s_sensor3_time = 0;
  int32_t diff_time;
  int32_t diff_time_b;
  static uint32_t s_tick = 0;
  uint8_t gesture_type = GESTURE_TYPE_NONE;

  // Record sensor trigger time
  if(!
  .readSensor())
  {
    s_sensor1_time = millis();
  }
  if(!barrier_s2.readSensor())
  {
    s_sensor2_time = millis();
  }
  if(!barrier_s3.readSensor())
  {
    s_sensor3_time = millis();
  }

  // Recognize the type of gesture based on the time difference triggered by the sensor
  if((s_sensor1_time>0) && (s_sensor3_time>0))  //&& (s_sensor2_time>0)
  {
    diff_time = s_sensor3_time - s_sensor2_time;
    diff_time_b = s_sensor2_time - s_sensor1_time;
    s_sensor1_time = 0;
    s_sensor2_time = 0;
    s_sensor3_time = 0;
    if(!barrier_s1.readSensor() && !barrier_s3.readSensor()) //&& !barrier_s2.readSensor()
    {
      gesture_type = GESTURE_TYPE_ALL;
    }
    else if( ((diff_time>GESTURE_DIFF_TIME_MIN) && (diff_time<GESTURE_DIFF_TIME_MAX)) && \
             ((diff_time_b>GESTURE_DIFF_TIME_MIN) && (diff_time_b<GESTURE_DIFF_TIME_MAX)) )
    {
      gesture_type = GESTURE_TYPE_RIGHT_TO_LEFT;
    }
    else if( ((-diff_time>GESTURE_DIFF_TIME_MIN) && (-diff_time<GESTURE_DIFF_TIME_MAX)) && \
             ((-diff_time_b>GESTURE_DIFF_TIME_MIN) && (-diff_time_b<GESTURE_DIFF_TIME_MAX)))
    {
      gesture_type = GESTURE_TYPE_LEFT_TO_RIGHT;
    }
  }
  else if((s_sensor1_time>0) && (s_sensor2_time>0))
  {
    diff_time = s_sensor2_time - s_sensor1_time;
    if((diff_time>GESTURE_DIFF_TIME_MIN) && (diff_time<GESTURE_DIFF_TIME_MAX))
    {
      s_sensor1_time = 0;
      s_sensor2_time = 0;
      gesture_type = GESTURE_TYPE_LEFT_TO_RIGHT;
    }
    else if((-diff_time>GESTURE_DIFF_TIME_MIN) && (-diff_time<GESTURE_DIFF_TIME_MAX))
    {
      s_sensor1_time = 0;
      s_sensor2_time = 0;
      gesture_type = GESTURE_TYPE_RIGHT_TO_LEFT;
    }
    else
    {
      // Over a period of time, clear the record
      if(((millis()-s_sensor1_time)>GESTURE_DIFF_TIME_OUT) || ((millis()-s_sensor2_time)>GESTURE_DIFF_TIME_OUT))
      {
        s_sensor1_time = 0;
        s_sensor2_time = 0;
      }
    }
  }
  else if((s_sensor2_time>0) && (s_sensor3_time>0))
  {
    diff_time = s_sensor2_time - s_sensor3_time;
    if((diff_time>GESTURE_DIFF_TIME_MIN) && (diff_time<GESTURE_DIFF_TIME_MAX))
    {
      s_sensor2_time = 0;
      s_sensor3_time = 0;
      gesture_type = GESTURE_TYPE_RIGHT_TO_LEFT;
    }
    else if((-diff_time>GESTURE_DIFF_TIME_MIN) && (-diff_time<GESTURE_DIFF_TIME_MAX))
    {
      s_sensor2_time = 0;
      s_sensor3_time = 0;
      gesture_type = GESTURE_TYPE_LEFT_TO_RIGHT;
    }
    else
    {
      // Over a period of time, clear the record
      if(((millis()-s_sensor2_time)>GESTURE_DIFF_TIME_OUT) || ((millis()-s_sensor3_time)>GESTURE_DIFF_TIME_OUT))
      {
        s_sensor2_time = 0;
        s_sensor3_time = 0;
      }
    }
  }

  return gesture_type;
}
