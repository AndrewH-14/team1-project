/**
 * File that will implement all of the Makeblock's LED functionality.
*/
#include "button.h"

#include "src/MeCollisionSensor.h"

#define COLLISION_S1_PIN  A11
#define COLLISION_S2_PIN  A12

MeCollisionSensor collision_s1(COLLISION_S1_PIN);
MeCollisionSensor collision_s2(COLLISION_S2_PIN);

/**
 * Function that determines whether the impact switches are being pressed.
 *
 * @retval true  The impact switch is being pressed.
 * @retval false The impact switch is not being pressed.
*/
bool button_is_pressed(void) {
    int s1 = collision_s1.readSensor();
    int s2 = collision_s2.readSensor();
    if (s1 || s2) {
        return true;
    }
    else {
        return false;
    }
}
