/**
 * File that will implement the rendezvous verify state for the MakeBlock.
 * The start function will be called by the main loop, and should encapsulate
 * alllogic pertaining to this state.
 */
#include "state_rendezvous_verify.h"

#include <MeMegaPi.h>

/**
 * The start point for the rendezvous verify state. This function will return
 * once the correct hand signal is detected.
 *
 * @retval true  The correct hand signal was detected.
 * @retval false An error occured.
*/
bool state_rendezvous_verify_start(void) {
    return true;
}
