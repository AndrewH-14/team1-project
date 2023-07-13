/**
 * File that will implement the extraction state for the MakeBlock. The start
 * function will be called by the main loop, and should encapsulate all logic
 * pertaining to this state.
*/
#include "state_extraction.h"

#include <MeMegaPi.h>

// ultrasonic sensor, not sure which port
MeUltrasonicSensor ultraSensor(PORT_7);

// Count on how many times the robot has moved forward
int count;

// Value detailing how many times the robot should move this cycle
int cycle_limit;

// Value detailing if values have been initialized
bool initialized;

// Value detecting if an obstacle has been detected this exploration cycle
bool encountered_obstacle;

// Value detecting how many cycles the robot has gone through
bool cycle_count;

// Value detecting if there is a hand signal
bool hand_signal;

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
        hand_signal = false;
    }
    // Move left if object, move forward if no object, increase count if moved forward
    if (!avoid_object) {
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
        if ((cycle_count % 4) == 3)
        {
            cycle_limit += 2;
        }
        count = 0;
        cycle_count++;
    }
    // Check if a hand signal has been waved and if it has stop function
    if (hand_signal)
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
    // Check if there is an object 5 cm away
    if (ultraSensor.distanceCm() > 5)
    {
        // Move forward if no objects
        motor_move_forward(100,500);
        return false;
    }
    // If object in front move right
    else
    {
        encountered_obstacle = true;
        // Move left until no detection of object
        motor_move_right(100,500);
        return true;
    }
}
