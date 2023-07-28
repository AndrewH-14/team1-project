/**
 * File that will implement all of the Makeblock's LED functionality.
*/
#include "led.h"

#include <MeMegaPi.h>
#include "src/MeNewRGBLed.h"

#define RGBLED_S1_PIN A13
#define RGBLED_S2_PIN A14
#define RGBLED_NUM    4

MeNewRGBLed new_rgbled_s1(RGBLED_S1_PIN,RGBLED_NUM);
MeNewRGBLed new_rgbled_s2(RGBLED_S2_PIN,RGBLED_NUM);

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
