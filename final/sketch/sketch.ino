/**
 * Team 1 Intern Project MakeBlock Code
 */
#include <MeMegaPi.h>

#include "src/MeNewRGBLed.h"
#include "src/MeBarrierSensor.h"
#include "src/MeCollisionSensor.h"
#include "src/MeSingleLineFollower.h"

#include "motor.h"

// LED Macros
#define RGBLED_S1_PIN         A13
#define RGBLED_S2_PIN         A14
#define RGBLED_NUM            4
// Barrier Macros
#define BARRIER_S1_PIN        A6
#define BARRIER_S2_PIN        A7
#define BARRIER_S3_PIN        A8
// Collision Macros
#define COLLISION_S1_PIN      A11
#define COLLISION_S2_PIN      A12
// Line Follower Macros
#define LINEFOLLOWER_S1_PIN   A9
#define LINEFOLLOWER_S2_PIN   A10
// Gesture Detection Macros
#define GESTURE_DIFF_TIME_MIN 20
#define GESTURE_DIFF_TIME_MAX 1000
#define GESTURE_DIFF_TIME_OUT 3000
#define PWM_MIN_OFFSET        0
#define GESTURE_MOVE_SPEED    100
// LED Sensors
MeNewRGBLed new_rgbled_s1(RGBLED_S1_PIN,RGBLED_NUM);
MeNewRGBLed new_rgbled_s2(RGBLED_S2_PIN,RGBLED_NUM);
// Barrier Sensors
MeBarrierSensor barrier_s1(BARRIER_S1_PIN);
MeBarrierSensor barrier_s2(BARRIER_S2_PIN);
MeBarrierSensor barrier_s3(BARRIER_S3_PIN);
// Collision Sensors
MeCollisionSensor collision_s1(COLLISION_S1_PIN);
MeCollisionSensor collision_s2(COLLISION_S2_PIN);
// Initialize the line follower sensors
MeSingleLineFollower line_follower_s1(LINEFOLLOWER_S1_PIN);
MeSingleLineFollower line_follower_s2(LINEFOLLOWER_S2_PIN);
// All possible line following sensor states
enum LineFollowingStates {
    SEN1_IN_SEN2_IN,
    SEN1_OUT_SEN2_IN,
    SEN1_IN_SEN2_OUT,
    SEN1_OUT_SEN2_OUT
};
// All possible gesture types
enum GestureTypes {
  GESTURE_TYPE_NONE,
  GESTURE_TYPE_LEFT_TO_RIGHT,
  GESTURE_TYPE_RIGHT_TO_LEFT,
  GESTURE_TYPE_ALL
};
// All possible obstacle states
enum ObstacleStates {
  S3_OBJ_S2_OBJ_S1_OBJ = 0,
  S3_OBJ_S2_OBJ_S1_FREE,
  S3_OBJ_S2_FREE_S1_OBJ,
  S3_OBJ_S2_FREE_S1_FREE,
  S3_FREE_S2_OBJ_S1_OBJ,
  S3_FREE_S2_OBJ_S1_FREE,
  S3_FREE_S2_FREE_S1_OBJ,
  S3_FREE_S2_FREE_S1_FREE,
};
// All possible rendezvous states
enum RendezvousStates {
    RENDEZVOUS_1,
    RENDEZVOUS_2,
    RENDEZVOUS_3,
    RENDEZVOUS_4,
    RENDEZVOUS_DONE,
    RENDEZVOUS_INCORRECT,
    RENDEZVOUS_SIGNAL_READ
};
// All possible MakeBlock states
enum MakeBlockStates {
    MB_STATE_LINE_FOLLOWER_1,
    MB_STATE_EXTRACTION,
    MB_STATE_RENDEZVOUS_VERIFY,
    MB_STATE_DATA_EXTRACTION,
    MB_STATE_LINE_FOLLOWER_2,
    MB_STATE_DONE
};
// Value detecting if an obstacle has been detected this exploration cycle
bool encountered_obstacle;

/**
 * Sets the LED color on the MakeBlock.
 *
 * @param r The amount of red to display on LED [0, 255]
 * @param g The amount of blue to display on LED [0, 255]
 * @param b The amount of green to display on LED [0, 255]
 */
void led_set_color(uint8_t r, uint8_t g, uint8_t b) {
    new_rgbled_s1.setColor(r, g, b);
    new_rgbled_s1.show();
    new_rgbled_s2.setColor(r, g, b);
    new_rgbled_s2.show();
}

/**
 * \par Function
 *    detect_gesture
 * \par Description
 *    This function is used to detect gesture.
 * \param[in]
 *    none.
 * \par Output
 *    uint8_t
 * \return
 *    None
 * \par Others
 *    None
 */
uint8_t detect_gesture(void) {
    static uint32_t s_sensor1_time = 0;
    static uint32_t s_sensor2_time = 0;
    static uint32_t s_sensor3_time = 0;
    int32_t diff_time;
    int32_t diff_time_b;
    static uint32_t s_tick = 0;
    enum GestureTypes gesture_type = GESTURE_TYPE_NONE;
    // Record sensor trigger time
    if(!barrier_s1.readSensor()) {
        s_sensor1_time = millis();
    }
    if(!barrier_s2.readSensor()) {
        s_sensor2_time = millis();
    }
    if(!barrier_s3.readSensor()) {
        s_sensor3_time = millis();
    }
    // Recognize the type of gesture based on the time difference triggered by the sensor
    if((s_sensor1_time>0) && (s_sensor3_time>0)) { //&& (s_sensor2_time>0)
        diff_time = s_sensor3_time - s_sensor2_time;
        diff_time_b = s_sensor2_time - s_sensor1_time;
        s_sensor1_time = 0;
        s_sensor2_time = 0;
        s_sensor3_time = 0;
        if(!barrier_s1.readSensor() && !barrier_s3.readSensor()) { //&& !barrier_s2.readSensor()
            gesture_type = GESTURE_TYPE_ALL;
        }
        else if( ((diff_time>GESTURE_DIFF_TIME_MIN) && (diff_time<GESTURE_DIFF_TIME_MAX)) && \
                 ((diff_time_b>GESTURE_DIFF_TIME_MIN) && (diff_time_b<GESTURE_DIFF_TIME_MAX)) ) {
            gesture_type = GESTURE_TYPE_RIGHT_TO_LEFT;
        }
        else if( ((-diff_time>GESTURE_DIFF_TIME_MIN) && (-diff_time<GESTURE_DIFF_TIME_MAX)) && \
                 ((-diff_time_b>GESTURE_DIFF_TIME_MIN) && (-diff_time_b<GESTURE_DIFF_TIME_MAX))) {
            gesture_type = GESTURE_TYPE_LEFT_TO_RIGHT;
        }
    }
    else if((s_sensor1_time>0) && (s_sensor2_time>0)) {
        diff_time = s_sensor2_time - s_sensor1_time;
        if((diff_time>GESTURE_DIFF_TIME_MIN) && (diff_time<GESTURE_DIFF_TIME_MAX)) {
            s_sensor1_time = 0;
            s_sensor2_time = 0;
            gesture_type = GESTURE_TYPE_LEFT_TO_RIGHT;
        }
        else if((-diff_time>GESTURE_DIFF_TIME_MIN) && (-diff_time<GESTURE_DIFF_TIME_MAX)) {
            s_sensor1_time = 0;
            s_sensor2_time = 0;
            gesture_type = GESTURE_TYPE_RIGHT_TO_LEFT;
        }
        else {
            // Over a period of time, clear the record
            if(((millis()-s_sensor1_time)>GESTURE_DIFF_TIME_OUT) || ((millis()-s_sensor2_time)>GESTURE_DIFF_TIME_OUT)) {
                s_sensor1_time = 0;
                s_sensor2_time = 0;
            }
        }
    }
    else if((s_sensor2_time>0) && (s_sensor3_time>0)) {
        diff_time = s_sensor2_time - s_sensor3_time;
        if((diff_time>GESTURE_DIFF_TIME_MIN) && (diff_time<GESTURE_DIFF_TIME_MAX)) {
            s_sensor2_time = 0;
            s_sensor3_time = 0;
            gesture_type = GESTURE_TYPE_RIGHT_TO_LEFT;
        }
        else if((-diff_time>GESTURE_DIFF_TIME_MIN) && (-diff_time<GESTURE_DIFF_TIME_MAX)) {
            s_sensor2_time = 0;
            s_sensor3_time = 0;
            gesture_type = GESTURE_TYPE_LEFT_TO_RIGHT;
        }
        else {
            // Over a period of time, clear the record
            if(((millis()-s_sensor2_time)>GESTURE_DIFF_TIME_OUT) || ((millis()-s_sensor3_time)>GESTURE_DIFF_TIME_OUT)) {
                s_sensor2_time = 0;
                s_sensor3_time = 0;
            }
        }
    }
    return gesture_type;
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
uint8_t read_obstacle_sta(void) {
  uint8_t sta = 0;
  sta = barrier_s1.readSensor();
  sta |= barrier_s2.readSensor() << 1 ;
  sta |= barrier_s3.readSensor() << 2 ;
  return sta;
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
        motor_move_backward(100, 1000);
        motor_turn_right(100,1000);
        motor_move_forward(100,1000);
        motor_turn_left(100,1000);
        return true;
    }
    return false;
}

/**
 * The start point for the line follower 1 & 2 states. This function will return
 * once a line is no longer detected.
 *
 * @retval true  A line is no longer detected.
 * @retval false An error occured.
*/
bool state_line_follower(void) {
    int b1 = collision_s1.readSensor();
    int b2 = collision_s2.readSensor();
    int count = 0;
    // Loop until impact trigger has been pressed three times
    while (count < 3) {
        int s1 = line_follower_s1.readSensor();
        int s2 = line_follower_s2.readSensor();
        if (s1 && s2) {
            motor_move_forward(100, 500);
        }
        else if (s1 && !s2) {
            motor_turn_right(100, 500);
        }
        else if (!s1 && s2) {
            motor_turn_left(100, 500);
        }
        // Check if the button has been pressed
        b1 = collision_s1.readSensor();
        b2 = collision_s2.readSensor();
        if (!b1 || !b2) { count++; }
    }
    return true;
}

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
bool state_extraction(void) {
    int b1 = collision_s1.readSensor();
    int b2 = collision_s2.readSensor();
    while (b1 && b2) {
        int count = 0;
        int cycle_count = 0;
        int cycle_limit = 5;
        int gesture_type = 0;
        encountered_obstacle = false;
        avoid_object();
        // Move left if object, move forward if no object, increase count if moved forward
        if (!avoid_object()) {
            count++;
        }
        // When moved forward a # of times equal to cycle limit turn left and start new exploration cycle
        if (count == cycle_limit) {
            // Turn 90 degrees to avoid obstacle
            motor_turn_left(100,1000);
            // If the robot encountered an obstacle it wants to turn left at a shorter interval
            // in order to increase likelihood of recording obstacles
            if(encountered_obstacle) {
                if (cycle_limit > 1) {
                    cycle_limit--;
                }
                encountered_obstacle = false;
            }
            // If the robot has turned sufficient times then the exploring region should increase
            if ((cycle_count % 4) == 3 || (cycle_count % 4) == 1) {
                cycle_limit += 1;
            }
            count = 0;
            cycle_count++;
        }
      b1 = collision_s1.readSensor();
      b2 = collision_s2.readSensor();
    }
    return true;
}

#define DEBUG

/**
 * Function that will implment rendezvous verify functionality
 *
 * @retval true
 * @retval false
 */
bool state_rendezvous(void) {
    enum RendezvousStates cur_state = RENDEZVOUS_SIGNAL_READ;
    // 3 second pause
    delay(3000);
    // Flash Green to verify correct robot
    led_set_color(0, 255, 0);
    delay(1000);
    led_set_color(0, 0, 0);
    // 3 second pause
    delay(3000);
    // Loop until the correct gesture sequence is detected
    while (cur_state != RENDEZVOUS_DONE) {
        // Get the current gesture type
        enum GestureTypes gesture = detect_gesture();
        switch (cur_state)
        {
            case RENDEZVOUS_SIGNAL_READ:
                // Pulse purple light to signal hand signal read mode
                for (int i = 0; i < 5; i++) {
                    delay(200);
                    led_set_color(255, 0, 255);
                    delay(200);
                    led_set_color(0, 0, 0);
                }
                cur_state = RENDEZVOUS_1;
                break;
            case RENDEZVOUS_1:
                if (gesture == GESTURE_TYPE_LEFT_TO_RIGHT) {
                  cur_state = RENDEZVOUS_2;
                  #ifdef DEBUG
                  led_set_color(0, 255, 0);
                  delay(1000);
                  led_set_color(0, 0, 0);
                  #endif

                }
                else if (gesture != GESTURE_TYPE_NONE && gesture != GESTURE_TYPE_ALL) cur_state = RENDEZVOUS_INCORRECT;
                break;
            case RENDEZVOUS_2:
                if (gesture == GESTURE_TYPE_LEFT_TO_RIGHT) {
                  cur_state = RENDEZVOUS_3;
                  #ifdef DEBUG
                  led_set_color(0, 255, 0);
                  delay(1000);
                  led_set_color(0, 0, 0);
                  #endif
                }
                else if (gesture != GESTURE_TYPE_NONE && gesture != GESTURE_TYPE_ALL) cur_state = RENDEZVOUS_INCORRECT;
                break;
            case RENDEZVOUS_3:
                if (gesture == GESTURE_TYPE_LEFT_TO_RIGHT) {
                  cur_state = RENDEZVOUS_4;
                  #ifdef DEBUG
                  led_set_color(0, 255, 0);
                  delay(1000);
                  led_set_color(0, 0, 0);
                  #endif
                }
                else if (gesture != GESTURE_TYPE_NONE && gesture != GESTURE_TYPE_ALL) cur_state = RENDEZVOUS_INCORRECT;
                break;
            case RENDEZVOUS_4:
                if (gesture == GESTURE_TYPE_LEFT_TO_RIGHT) {
                  cur_state = RENDEZVOUS_DONE;
                  #ifdef DEBUG
                  led_set_color(0, 255, 0);
                  delay(1000);
                  led_set_color(0, 0, 0);
                  #endif
                }
                else if (gesture != GESTURE_TYPE_NONE && gesture != GESTURE_TYPE_ALL) cur_state = RENDEZVOUS_INCORRECT;
                break;
            case RENDEZVOUS_INCORRECT:
                // Flash red to signal incorrect
                led_set_color(255, 0, 0);
                delay(5000);
                led_set_color(0, 0, 0);
                cur_state = RENDEZVOUS_SIGNAL_READ;
                break;
            default:
                break;
        }
    }
    // Flash green to signal success
    led_set_color(0, 255, 0);
    delay(5000);
    led_set_color(0, 0, 0);
    return true;
}

/**
 * Function that will implement data extraction functionality.
 *
 * @retval true
 * @retval false
 */
bool state_data_extraction(void) {
    // Pause operations
    int b1 = collision_s1.readSensor();
    int b2 = collision_s2.readSensor();
    while (b1 && b2) {
        delay(100);
        b1 = collision_s1.readSensor();
        b2 = collision_s2.readSensor();
    }
    // Delay until impact switch is released
    while (!b1 || !b2) {
        delay(100);
        b1 = collision_s1.readSensor();
        b2 = collision_s2.readSensor();
    }
    // Wait until operations are resumed
    while (b1 && b2) {
        delay(100);
        b1 = collision_s1.readSensor();
        b2 = collision_s2.readSensor();
    }
    // Delay until impact switch is released
    while (!b1 || !b2) {
        delay(100);
        b1 = collision_s1.readSensor();
        b2 = collision_s2.readSensor();
    }
    return true;
}

// Init state
enum MakeBlockStates g_cur_state =  MB_STATE_DATA_EXTRACTION;

// Setup function called upon initialization
void setup(void) {}

// Main loop function
void loop(void) {
    switch (g_cur_state) {
        case MB_STATE_LINE_FOLLOWER_1:
            // LEDs will be blue during this state
            led_set_color(0, 0, 255);
            delay(1000);
            led_set_color(0, 0, 0);
            state_line_follower();
            g_cur_state = MB_STATE_EXTRACTION;
            break;
        case MB_STATE_EXTRACTION:
            // LEDs will be yellow during this state
            led_set_color(255, 255, 0);
            delay(1000);
            led_set_color(0, 0, 0);
            state_extraction();
            g_cur_state = MB_STATE_RENDEZVOUS_VERIFY;
            break;
        case MB_STATE_RENDEZVOUS_VERIFY:
            // LEDS will be purple during this state
            led_set_color(255, 0, 255);
            delay(1000);
            led_set_color(0, 0, 0);
            state_rendezvous();
            g_cur_state = MB_STATE_DATA_EXTRACTION;
            break;
        case MB_STATE_DATA_EXTRACTION:
            // LEDS will be white during this state
            led_set_color(255, 255, 255);
            delay(1000);
            led_set_color(0, 0, 0);
            state_data_extraction();
            g_cur_state = MB_STATE_LINE_FOLLOWER_2;
            break;
        case MB_STATE_LINE_FOLLOWER_2:
            // LEDS will be cyan during this state
            led_set_color(0, 255, 255);
            delay(1000);
            led_set_color(0, 0, 0);
            state_line_follower();
            g_cur_state = MB_STATE_DONE;
            break;
        case MB_STATE_DONE:
            // Set LEDs to green to signal successful mission.
            led_set_color(0, 255, 0);
            break;
        default:
            break;
    }
}
