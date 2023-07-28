/**
 * Header for all line follower state.
 */
#ifndef GESTURE_H
#define GESTURE_H

#include "MeMegaPi.h"

// Two sensors trigger time window，unit:ms
#define GESTURE_DIFF_TIME_MIN 20
#define GESTURE_DIFF_TIME_MAX 1000

#define GESTURE_DIFF_TIME_OUT 3000

#define PWM_MIN_OFFSET   0

#define GESTURE_MOVE_SPEED  100

// Gesture type
typedef enum
{
  GESTURE_TYPE_NONE,
  GESTURE_TYPE_LEFT_TO_RIGHT,
  GESTURE_TYPE_RIGHT_TO_LEFT,
  GESTURE_TYPE_ALL
} GESTURE_TYPE_ENUM;

uint8_t detect_gesture(void);

#endif // GESTURE_H
