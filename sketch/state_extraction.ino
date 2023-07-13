/**
 * File that will implement the extraction state for the MakeBlock. The start
 * function will be called by the main loop, and should encapsulate all logic
 * pertaining to this state.
*/
#include "state_extraction.h"

#include <MeMegaPi.h>

/**
 * The start point for the extraction state. This function will return once
 * the hand signal has been detected.
 *
 * @retval true  The hand signal has been detected.
 * @retval false An error occured.
*/
bool state_extraction_start(void) {
    return true;
}
