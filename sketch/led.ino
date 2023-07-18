/**
 * File that will implement all of the Makeblock's LED functionality.
*/
#include "led.h"

#include <MeMegaPi.h>

// TODO: Determine the correct LED
MeRGBLed led1(PORT_13);
MeRGBLed led2(PORT_14);

/**
 * Sets the LED color on the MakeBlock.
 * 
 * @param r The amount of red to display on LED [0, 255]
 * @param g The amount of blue to display on LED [0, 255]
 * @param b The amount of green to display on LED [0, 255]
 */
void led_set_color(uint8_t r, uint8_t g, uint8_t b) {
      led1.setColor(r, g, b);
      led1.show();
      led2.setColor(r, g, b);
      led2.show();
}
