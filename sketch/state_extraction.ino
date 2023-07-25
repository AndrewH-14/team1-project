/**
 * File that will implement the extraction state for the MakeBlock. The start
 * function will be called by the main loop, and should encapsulate all logic
 * pertaining to this state.
*/
#include "state_extraction.h"
#include "src/MeBarrierSensor.h"
#include <MeMegaPi.h>
#include "gesture.h"

// Obstacle state
typedef enum
{
  S3_OBJ_S2_OBJ_S1_OBJ = 0,
  S3_OBJ_S2_OBJ_S1_FREE,
  S3_OBJ_S2_FREE_S1_OBJ,
  S3_OBJ_S2_FREE_S1_FREE,
  S3_FREE_S2_OBJ_S1_OBJ,
  S3_FREE_S2_OBJ_S1_FREE,
  S3_FREE_S2_FREE_S1_OBJ,
  S3_FREE_S2_FREE_S1_FREE,
}OBSTACLE_STA_ENUM;

// Count on how many times the robot has moved forward
int count = 0;

// Value detailing how many times the robot should move this cycle
int cycle_limit;

// Value detailing if values have been initialized
bool initialized;

// Value detecting if an obstacle has been detected this exploration cycle
bool encountered_obstacle;

// Value detecting how many cycles the robot has gone through
bool cycle_count;

// Value storing gesture types
uint8_t gesture_type;

/**
 * The start point for the extraction state. This function will return once
 * the hand signal has been detected.
 *
 * The extraction state functions by travelling in a growing square that grows at
 * a slower rate if obstacles are detected in order to increase likelihood all obstacles are
 * detected. The robot will explore a growing square until the mode changes.
 *
 * @retval true  The hand signal has been detected.
 * @retval false An error occured.
*/
bool state_extraction_start(void) {
    // initialize exploration varaibles if exploration variables are uninitialized
    if (!initialized)
    {
        initialized = true;
        count = 0;
        cycle_count = 0;
        cycle_limit = 5;
        encountered_obstacle = false;
        gesture_type = 0;
    }
    avoid_object();
    // Move left if object, move forward if no object, increase count if moved forward
    if (!avoid_object()) {
        count++;
    }

    // When moved forward a # of times equal to cycle limit turn left and start new exploration cycle
    if (count == cycle_limit) {
        // This turn left needs to become 90 degrees, I'm not sure values needed
        motor_turn_left(100,500);
        // If the robot encountered an obstacle it wants to turn left at a shorter interval
        // in order to increase likelihood of recording obstacles
        if(encountered_obstacle)
        {
            if (cycle_limit > 1)
            {
                cycle_limit--;
            }
            encountered_obstacle = false;
        }
        // If the robot has turned sufficient times then the exploring region should increase
        if ((cycle_count % 4) == 3 || (cycle_count % 4) == 1);
        {
            cycle_limit += 1;
        }
        count = 0;
        cycle_count++;
    }
    gesture_type = detect_gesture();

    // Check if a hand signal has been waved and if it has stop function
    if (gesture_type != GESTURE_TYPE_RIGHT_TO_LEFT)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
* Helper method to avoid obstacles
* @retval true if no object and moved forward
* @retval false if no object and moved left
*/
bool avoid_object(void) {
    // Check if there is an object detected by a sensor
    uint8_t sta = read_obstacle_sta();
    if ((sta == S3_FREE_S2_FREE_S1_FREE))
    {
        // Move forward if no objects
        motor_move_forward(100,300);
        return false;
    }
    // If object in front move right
    else
    {
        encountered_obstacle = true;
        // Move left until no detection of object
        motor_move_backward(100, 300);
        motor_turn_right(100,500);
        motor_move_forward(100,500);
        motor_turn_left(100,500);
        return true;
    }
    return false;
}

/**
 * \par Function
 *    read_obstacle_sta
 * \par Description
 *    This function is used to get collision state.
 * \param[in]
 *    none.
 * \par Output
 *    uint8_t.
 * \return
 *    None
 * \par Others
 *    None
 */
uint8_t read_obstacle_sta(void)
{
  uint8_t sta = 0;
  sta = barrier_s1.readSensor();
  sta |= barrier_s2.readSensor() << 1 ;
  sta |= barrier_s3.readSensor() << 2 ;
  return sta;
}
