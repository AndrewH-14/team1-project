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
#define LINEFOLLOWER_FORWARD_SPEED 75
#define LINEFOLLOWER_FORWARD_DELAY 1
#define LINEFOLLOWER_TURN_SPEED    100
#define LINEFOLLOWER_TURN_DELAY    75
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
    RENDEZVOUS_CHECK,
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
    // Check if we are on a line
    int s1 = line_follower_s1.readSensor();
    int s2 = line_follower_s2.readSensor();
    // If we are on a line, we want to turn 180 degrees backwards
    if (!s1 || !s2) {
        motor_turn_right(100, 500);
        return false;
    }
    else if ((sta == S3_FREE_S2_FREE_S1_FREE)) {
        // Move forward if no objects
        motor_move_forward(50,200);
        return false;
    }
    // If object in front move right
    else {
        // Move right until no detection of object
        motor_move_backward(100, 200);
        motor_turn_right(100,300);
        return true;
    }
    return false;
}

/**
 * The start point for the line follower 1 & 2 states. This function will return
 * once the impact trigger has been pressed three times.
 *
 * @retval true  Line dection state over.
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
        if (!s1 && !s2) {
            motor_move_forward(LINEFOLLOWER_FORWARD_SPEED, LINEFOLLOWER_FORWARD_DELAY);
        }
        else if (!s1 && s2) {
            motor_turn_right(LINEFOLLOWER_TURN_SPEED, LINEFOLLOWER_TURN_DELAY);
        }
        else if (s1 && !s2) {
            motor_turn_left(LINEFOLLOWER_TURN_SPEED, LINEFOLLOWER_TURN_DELAY);
        }
        // Check if the button has been pressed
        b1 = collision_s1.readSensor();
        b2 = collision_s2.readSensor();
        if (!b1 || !b2) {
            count++;
            // Delay until button is released
            while (!b1 || !b2) {
                // Debouncing
                delay(100);
                b1 = collision_s1.readSensor();
                b2 = collision_s2.readSensor();
            }
        }
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
        avoid_object();
        b1 = collision_s1.readSensor();
        b2 = collision_s2.readSensor();
    }
    // Delay until button is released
    while (!b1 || !b2) {
        // Debouncing
        delay(100);
        b1 = collision_s1.readSensor();
        b2 = collision_s2.readSensor();
    }
    return true;
}

/**
 * Function that will implment rendezvous verify functionality
 *
 * @retval true
 * @retval false
 */
bool state_rendezvous(void) {
    enum RendezvousStates cur_state = RENDEZVOUS_SIGNAL_READ;
    bool correct_sequence = true;
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
                // Will be set to false if wroing hand gesture detected
                correct_sequence = true;
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
                else if (gesture != GESTURE_TYPE_NONE && gesture != GESTURE_TYPE_ALL) {
                    cur_state = RENDEZVOUS_2;
                    correct_sequence = false;
                    #ifdef DEBUG
                    led_set_color(255, 0, 0);
                    delay(1000);
                    led_set_color(0, 0, 0);
                    #endif
                }
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
                else if (gesture != GESTURE_TYPE_NONE && gesture != GESTURE_TYPE_ALL) {
                    cur_state = RENDEZVOUS_3;
                    correct_sequence = false;
                    #ifdef DEBUG
                    led_set_color(255, 0, 0);
                    delay(1000);
                    led_set_color(0, 0, 0);
                    #endif
                }
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
                else if (gesture != GESTURE_TYPE_NONE && gesture != GESTURE_TYPE_ALL) {
                    cur_state = RENDEZVOUS_4;
                    correct_sequence = false;
                    #ifdef DEBUG
                    led_set_color(255, 0, 0);
                    delay(1000);
                    led_set_color(0, 0, 0);
                    #endif
                }
                break;
            case RENDEZVOUS_4:
                if (gesture == GESTURE_TYPE_LEFT_TO_RIGHT) {
                    cur_state = RENDEZVOUS_CHECK;
                    #ifdef DEBUG
                    led_set_color(0, 255, 0);
                    delay(1000);
                    led_set_color(0, 0, 0);
                    #endif
                }
                else if (gesture != GESTURE_TYPE_NONE && gesture != GESTURE_TYPE_ALL) {
                    cur_state = RENDEZVOUS_CHECK;
                    correct_sequence = false;
                    #ifdef DEBUG
                    led_set_color(255, 0, 0);
                    delay(1000);
                    led_set_color(0, 0, 0);
                    #endif
                }
                break;
            case RENDEZVOUS_CHECK:
                if (!correct_sequence) {
                    // Flash red to signal incorrect
                    led_set_color(255, 0, 0);
                    delay(5000);
                    led_set_color(0, 0, 0);
                    cur_state = RENDEZVOUS_SIGNAL_READ;
                }
                else {
                    // Flash green to signal success
                    led_set_color(0, 255, 0);
                    delay(5000);
                    led_set_color(0, 0, 0);
                    cur_state = RENDEZVOUS_DONE;
                }
                break;
            default:
                break;
        }
    }
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
        // Debouncing
        delay(100);
        b1 = collision_s1.readSensor();
        b2 = collision_s2.readSensor();
    }
    // Delay until impact switch is released
    while (!b1 || !b2) {
        // Debouncing
        delay(100);
        b1 = collision_s1.readSensor();
        b2 = collision_s2.readSensor();
    }
    // Wait until operations are resumed
    while (b1 && b2) {
        // Debouncing
        delay(100);
        b1 = collision_s1.readSensor();
        b2 = collision_s2.readSensor();
    }
    // Delay until impact switch is released
    while (!b1 || !b2) {
        // Debouncing
        delay(100);
        b1 = collision_s1.readSensor();
        b2 = collision_s2.readSensor();
    }
    return true;
}

// Init state
enum MakeBlockStates g_cur_state =  MB_STATE_LINE_FOLLOWER_1;

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
