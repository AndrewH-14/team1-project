#include "gesture.h"

#include "MeMegaPi.h"
#include "src/MeBarrierSensor.h"

#define BARRIER_S1_PIN  A6
#define BARRIER_S2_PIN  A7
#define BARRIER_S3_PIN  A8

MeBarrierSensor  barrier_s1(BARRIER_S1_PIN);
MeBarrierSensor  barrier_s2(BARRIER_S2_PIN);
MeBarrierSensor  barrier_s3(BARRIER_S3_PIN);

/**
 * Function that will determine and return the specific hand signal detected
*/
uint8_t detect_gesture(void)
{
  static uint32_t s_sensor1_time = 0;
  static uint32_t s_sensor2_time = 0;
  static uint32_t s_sensor3_time = 0;
  int32_t diff_time;
  int32_t diff_time_b;
  static uint32_t s_tick = 0;
  uint8_t gesture_type = GESTURE_TYPE_NONE;

  // Record sensor trigger time
  if(!barrier_s1.readSensor())
  {
    s_sensor1_time = millis();
  }
  if(!barrier_s2.readSensor())
  {
    s_sensor2_time = millis();
  }
  if(!barrier_s3.readSensor())
  {
    s_sensor3_time = millis();
  }

  // Recognize the type of gesture based on the time difference triggered by the sensor
  if((s_sensor1_time>0) && (s_sensor3_time>0))  //&& (s_sensor2_time>0)
  {
    diff_time = s_sensor3_time - s_sensor2_time;
    diff_time_b = s_sensor2_time - s_sensor1_time;
    s_sensor1_time = 0;
    s_sensor2_time = 0;
    s_sensor3_time = 0;
    if(!barrier_s1.readSensor() && !barrier_s3.readSensor()) //&& !barrier_s2.readSensor()
    {
      gesture_type = GESTURE_TYPE_ALL;
    }
    else if( ((diff_time>GESTURE_DIFF_TIME_MIN) && (diff_time<GESTURE_DIFF_TIME_MAX)) && \
             ((diff_time_b>GESTURE_DIFF_TIME_MIN) && (diff_time_b<GESTURE_DIFF_TIME_MAX)) )
    {
      gesture_type = GESTURE_TYPE_RIGHT_TO_LEFT;
    }
    else if( ((-diff_time>GESTURE_DIFF_TIME_MIN) && (-diff_time<GESTURE_DIFF_TIME_MAX)) && \
             ((-diff_time_b>GESTURE_DIFF_TIME_MIN) && (-diff_time_b<GESTURE_DIFF_TIME_MAX)))
    {
      gesture_type = GESTURE_TYPE_LEFT_TO_RIGHT;
    }
  }
  else if((s_sensor1_time>0) && (s_sensor2_time>0))
  {
    diff_time = s_sensor2_time - s_sensor1_time;
    if((diff_time>GESTURE_DIFF_TIME_MIN) && (diff_time<GESTURE_DIFF_TIME_MAX))
    {
      s_sensor1_time = 0;
      s_sensor2_time = 0;
      gesture_type = GESTURE_TYPE_LEFT_TO_RIGHT;
    }
    else if((-diff_time>GESTURE_DIFF_TIME_MIN) && (-diff_time<GESTURE_DIFF_TIME_MAX))
    {
      s_sensor1_time = 0;
      s_sensor2_time = 0;
      gesture_type = GESTURE_TYPE_RIGHT_TO_LEFT;
    }
    else
    {
      // Over a period of time, clear the record
      if(((millis()-s_sensor1_time)>GESTURE_DIFF_TIME_OUT) || ((millis()-s_sensor2_time)>GESTURE_DIFF_TIME_OUT))
      {
        s_sensor1_time = 0;
        s_sensor2_time = 0;
      }
    }
  }
  else if((s_sensor2_time>0) && (s_sensor3_time>0))
  {
    diff_time = s_sensor2_time - s_sensor3_time;
    if((diff_time>GESTURE_DIFF_TIME_MIN) && (diff_time<GESTURE_DIFF_TIME_MAX))
    {
      s_sensor2_time = 0;
      s_sensor3_time = 0;
      gesture_type = GESTURE_TYPE_RIGHT_TO_LEFT;
    }
    else if((-diff_time>GESTURE_DIFF_TIME_MIN) && (-diff_time<GESTURE_DIFF_TIME_MAX))
    {
      s_sensor2_time = 0;
      s_sensor3_time = 0;
      gesture_type = GESTURE_TYPE_LEFT_TO_RIGHT;
    }
    else
    {
      // Over a period of time, clear the record
      if(((millis()-s_sensor2_time)>GESTURE_DIFF_TIME_OUT) || ((millis()-s_sensor3_time)>GESTURE_DIFF_TIME_OUT))
      {
        s_sensor2_time = 0;
        s_sensor3_time = 0;
      }
    }
  }

  return gesture_type;
}
