/**
 * File that will implement all of the Makeblock's LED functionality.
*/
#include "button.h"

#define BUTTON_PIN 7

/**
 * Determines if the button is being pressed.
 *
 * @retval true The button is being pressed
 * @retval false The button is not being pressed
 */
bool button_is_pressed(void) {
    // initialize the pushbutton pin as an input:
    pinMode(BUTTON_PIN, INPUT);
    // Read the state of the button
    int button_state = digitalRead(BUTTON_PIN);
    // Determine if the button is being pressed
    if (HIGH == button_state) {
        return true;
    }
    else {
        return false;
    }
}
