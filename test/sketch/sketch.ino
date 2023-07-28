#include <MeMegaPi.h>

#include "led.h"
#include "motor.h"

#include "src/MeCollisionSensor.h"
#include "src/MeSingleLineFollower.h"
#include "src/MeBarrierSensor.h"

#define COLLISION_S1_PIN  A11
#define COLLISION_S2_PIN  A12

// Barrier Macros
#define BARRIER_S1_PIN        A6
#define BARRIER_S2_PIN        A7
#define BARRIER_S3_PIN        A8

MeCollisionSensor collision_s1(COLLISION_S1_PIN);
MeCollisionSensor collision_s2(COLLISION_S2_PIN);

// Define the ports the the line follower peripherals are on
#define LINEFOLLOWER_S1_PIN  A9
#define LINEFOLLOWER_S2_PIN  A10

// Initialize the line follower sensors
MeSingleLineFollower line_follower_s1(LINEFOLLOWER_S1_PIN);
MeSingleLineFollower line_follower_s2(LINEFOLLOWER_S2_PIN);

void setup(void) {

}

void loop(void) {
    //int s1 = collision_s1.readSensor();
    //int s2 = collision_s2.readSensor();
    int s1 = line_follower_s1.readSensor();
    int s2 = line_follower_s2.readSensor();
    led_set_color(255, 255, 255);
    if (s1) {
      led_set_color(255, 0, 0);
      motor_move_forward(100, 100);
    }
    if (s2) {
      led_set_color(0, 0, 255);
    }
}
