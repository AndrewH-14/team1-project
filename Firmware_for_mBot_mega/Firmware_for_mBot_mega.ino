/*************************************************************************
* File Name          : Firmware_for_mBot_mega.ino
* Author             : huyisi
* Updated            : huyisi
* Version            : V47.01.002
* Date               : 04/29/2021
* Description        : Firmware for Makeblock Electronic modules with Scratch.  
* License            : CC-BY-SA 3.0
* Copyright (C) 2013 - 2016 Maker Works Technology Co., Ltd. All right reserved.
* http://www.makeblock.cc/
* History:
* <Author>         <Time>         <Version>        <Descr>
* huyisi         2021/03/23     47.01.001        build the new.
* zhngxi         2021/04/29     47.01.002        Fix the collision mode cannot be switched 
**************************************************************************/
#include <Arduino.h>
#include <MeMegaPi.h>
#include "MeEEPROM.h"
#include <Wire.h>
#include <SoftwareSerial.h>

#include "MeSingleLineFollower.h"
#include "MeCollisionSensor.h"
#include "MeBarrierSensor.h"
#include "MeNewRGBLed.h"

//#define DEBUG_INFO
//#define DEBUG_INFO1

//////////////////////////////////////////////////////////////////////////////////////
#define LINEFLOOWER_S1_PIN  A9
#define LINEFLOOWER_S2_PIN  A10

#define COLLISION_S1_PIN  A11
#define COLLISION_S2_PIN  A12

#define BARRIER_S1_PIN  A6
#define BARRIER_S2_PIN  A7
#define BARRIER_S3_PIN  A8

#define RGBLED_S1_PIN   A13
#define RGBLED_S2_PIN   A14

#define RGBLED_NUM      4



//Line-following speed -255`255
#define FORWARD_SPEED 80
#define TURN_SPEED 20
#define MAX_VAL  4

//Obstacle avoidance speed
#define OBSTACLE_FORWARD_SPEED 100
#define OBSTACLE_TURN_SPEED 60

#define RGB_LOW_VAL  80
#define RGB_HIGH_VAL 255


// Two sensors trigger time window，unit:ms
#define GESTURE_DIFF_TIME_MIN 20
#define GESTURE_DIFF_TIME_MAX 1000

#define GESTURE_DIFF_TIME_OUT 3000

#define PWM_MIN_OFFSET   0

#define GESTURE_MOVE_SPEED  100

//module id number
#define VERSION                0
#define RGBLED                 8
#define MOTOR                  10
#define DIGITAL                30
#define ANALOG                 31
#define PWM                    32
#define PULSEIN                37
#define TIMER                  50
#define COMMON_COMMONCMD       60
  //Secondary command
  #define SET_STARTER_MODE     0x10
  #define SET_AURIGA_MODE      0x11
  #define SET_MEGAPI_MODE      0x12
  #define GET_BATTERY_POWER    0x70
  #define GET_AURIGA_MODE      0x71
  #define GET_MEGAPI_MODE      0x72

#define SINGLE_LINE_FOLLOWER   68
#define COLLISION              69
#define OBSTACLE               70


#define CARE_CONTROL           100

#define GET 1
#define RUN 2
#define RESET 4
#define START 5

#define BLUETOOTH_MODE                       0x00
#define OBSTACLE_MODE                        0x01
#define LINE_FOLLOW_MODE                     0x02
#define MAX_MODE                             0x03

//#define POWER_PORT                           A4
//#define BUZZER_PORT                          45
//#define RGBLED_PORT                          44

#define DATA_SERIAL                            0
#define DATA_SERIAL1                           1
#define DATA_SERIAL2                           2
#define DATA_SERIAL3                           3

/////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum
{
  SS2_IN_S1_IN = 0,
  SS2_IN_S1_OUT,
  SS2_OUT_S1_IN,
  SS2_OUT_S1_OUT, 
}LINE_FOLLOWER_STA_ENUM;


// Gesture type
typedef enum
{
  GESTURE_TYPE_NONE,
  GESTURE_TYPE_LEFT_TO_RIGHT,
  GESTURE_TYPE_RIGHT_TO_LEFT,
  GESTURE_TYPE_ALL
} GESTURE_TYPE_ENUM;


// Collision state
typedef enum
{
  S2_COL_S1_COL = 0,
  S2_COL_S1_FREE,
  S2_FREE_S1_COL,
  S2_FREE_S1_RREE, 
}COLLISION_STA_ENUM;


// Obstacle state
typedef enum
{
  S3_OBJ_S2_OBJ_S1_OBJ = 0,
  S3_OBJ_S2_OBJ_S1_FREE,
  S3_OBJ_S2_FREE_S1_OBJ,
  S3_OBJ_S2_FREE_S1_FREE,
  S3_FREE_S2_OBJ_S1_OBJ,
  S3_FREE_S2_OBJ_S1_FREE,
  S3_FREE_S2_FREE_S1_OBJ,
  S3_FREE_S2_FREE_S1_FREE, 
}OBSTACLE_STA_ENUM;



typedef struct MeModule
{
  int16_t device;
  int16_t port;
  int16_t slot;
  int16_t pin;
  int16_t index;
  float values[3];
} MeModule;

union
{
  uint8_t byteVal[4];
  float floatVal;
  long longVal;
}val;

union
{
  uint8_t byteVal[8];
  double doubleVal;
}valDouble;

union
{
  uint8_t byteVal[2];
  int16_t shortVal;
}valShort;

/////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(__AVR_ATmega32U4__) 
  int16_t analogs[12]={A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11};
#endif
#if defined(__AVR_ATmega328P__) or defined(__AVR_ATmega168__)
  int16_t analogs[8]={A0,A1,A2,A3,A4,A5,A6,A7};
#endif
#if defined(__AVR_ATmega1280__)|| defined(__AVR_ATmega2560__)
  int16_t analogs[16]={A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15};
#endif


MeMegaPiDCMotor dc_motor;


MeMegaPiDCMotor motor_foward_left(PORT2B);
MeMegaPiDCMotor motor_foward_right(PORT1A);
MeMegaPiDCMotor motor_back_left(PORT2A);
MeMegaPiDCMotor motor_back_right(PORT1B);

MeSingleLineFollower line_follower_s1(LINEFLOOWER_S1_PIN);
MeSingleLineFollower line_follower_s2(LINEFLOOWER_S2_PIN);

MeCollisionSensor collision_s1(COLLISION_S1_PIN);
MeCollisionSensor collision_s2(COLLISION_S2_PIN);

MeBarrierSensor  barrier_s1(BARRIER_S1_PIN);
MeBarrierSensor  barrier_s2(BARRIER_S2_PIN);
MeBarrierSensor  barrier_s3(BARRIER_S3_PIN);

MeNewRGBLed new_rgbled_s1(RGBLED_S1_PIN,RGBLED_NUM);
MeNewRGBLed new_rgbled_s2(RGBLED_S2_PIN,RGBLED_NUM);


uint8_t buf[64];
uint8_t bufindex;

uint8_t command_index = 0;
uint8_t megapi_mode = BLUETOOTH_MODE;
uint8_t index = 0;
uint8_t dataLen;
uint8_t prevc=0;
uint8_t BluetoothSource = DATA_SERIAL;
uint8_t serialRead;
uint8_t buffer[52];
uint8_t bufferBt1[52];
uint8_t bufferBt2[52];
double  lastTime = 0.0;
double  currentTime = 0.0;

long blink_time = 0;
boolean isStart = false;
boolean isAvailable = false;
boolean blink_flag = false;

String mVersion = "47.01.002";


/////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * \par Function
 *    motor_foward_left_run
 * \par Description
 *    This function use to control foward left wheel..
 * \param[in]
 *    speed - The speed of wheel -255~255
 * \par Output
 *    None
 * \return
 *    None
 * \par Others
 *    None
 */
void motor_foward_left_run(int16_t speed)
{
  motor_foward_left.reset(PORT2B);
  motor_foward_left.run(-speed);
}

/**
 * \par Function
 *    motor_foward_right_run
 * \par Description
 *    This function use to control foward right wheel..
 * \param[in]
 *    speed - The speed of wheel -255~255
 * \par Output
 *    None
 * \return
 *    None
 * \par Others
 *    None
 */
void motor_foward_right_run(int16_t speed)
{
  motor_foward_right.reset(PORT1A);
  motor_foward_right.run(speed);
}

/**
 * \par Function
 *    motor_back_left_run
 * \par Description
 *    This function use to control back left wheel..
 * \param[in]
 *    speed - The speed of wheel -255~255
 * \par Output
 *    None
 * \return
 *    None
 * \par Others
 *    None
 */
void motor_back_left_run(int16_t speed)
{
  motor_back_left.reset(PORT2A);
  motor_back_left.run(-speed);
}

/**
 * \par Function
 *    motor_back_right_run
 * \par Description
 *    This function use to control back right wheel..
 * \param[in]
 *    speed - The speed of wheel -255~255
 * \par Output
 *    None
 * \return
 *    None
 * \par Others
 *    None
 */
void motor_back_right_run(int16_t speed)
{
  motor_back_right.reset(PORT1B);
  motor_back_right.run(speed);
}


// vx：left or right
// vy：forward or backward
// vw：clockwise or counterclockwise
void move_control(int16_t vx, int16_t vy, int16_t vw)
{
  int16_t foward_left_speed;
  int16_t foward_right_speed;
  int16_t back_left_speed;
  int16_t back_right_speed;

  foward_left_speed = vy + vx + vw;
  foward_right_speed = vy - vx - vw;
  back_left_speed = vy - vx + vw;
  back_right_speed = vy + vx - vw;

  motor_foward_left_run(foward_left_speed);    
  motor_foward_right_run(foward_right_speed);   
  motor_back_left_run(back_left_speed);   
  motor_back_right_run(back_right_speed);   
}

//RGB_LED control
void new_rgbled_show(uint8_t pin, uint8_t index, uint8_t *rgb)
{
  if(pin == RGBLED_S1_PIN)
  {
    new_rgbled_s1.setColor(index,rgb[0],rgb[1],rgb[2]);
    new_rgbled_s1.show();
  }
  else if(pin == RGBLED_S2_PIN)
  {
    new_rgbled_s2.setColor(index,rgb[0],rgb[1],rgb[2]);
    new_rgbled_s2.show();
  }
}

//Double RGB_LED control
void new_rgbled_show_all(uint8_t module_idx ,uint8_t r1, uint8_t g1, uint8_t b1, uint8_t r2, uint8_t g2, uint8_t b2)
{
  if(module_idx == 0)
  {
    new_rgbled_s1.setColor(0,r1,g1,b1);
    new_rgbled_s1.show();
    new_rgbled_s2.setColor(0,r2,g2,b2);
    new_rgbled_s2.show();
  }
  else if(module_idx == 1)
  {
    new_rgbled_s1.setColor(0,r1,g1,b1);
    new_rgbled_s1.show();
  }
  else if(module_idx == 2)
  {
    new_rgbled_s2.setColor(0,r2,g2,b2);
    new_rgbled_s2.show();
  }

}

/**
 * \par Function
 *    readBuffer
 * \par Description
 *    This function use to read the serial data from its buffer..
 * \param[in]
 *    index - The first address in the array
 * \par Output
 *    None
 * \return
 *    The data need to be read.
 * \par Others
 *    None
 */
uint8_t readBuffer(int16_t index)
{
  if(BluetoothSource == DATA_SERIAL)
  {
    return buffer[index];
  }
  else if(BluetoothSource == DATA_SERIAL2)
  {
    return bufferBt1[index];
  }
  else if(BluetoothSource == DATA_SERIAL3)
  {
    return bufferBt2[index];
  }
}

/**
 * \par Function
 *    writeBuffer
 * \par Description
 *    This function use to write the serial data to its buffer..
 * \param[in]
 *    index - The data's first address in the array
  * \param[in]
 *    c - The data need to be write.
 * \par Output
 *    None
 * \return
 *    None
 * \par Others
 *    None
 */
void writeBuffer(int16_t index,uint8_t c)
{
  if(BluetoothSource == DATA_SERIAL)
  {
    buffer[index]=c;
  }
  else if(BluetoothSource == DATA_SERIAL2)
  {
    bufferBt1[index]=c;
  }
  else if(BluetoothSource == DATA_SERIAL3)
  {
    bufferBt2[index]=c;
  }
}

/**
 * \par Function
 *    writeHead
 * \par Description
 *    This function use to write the head of transmission frame.
 * \param[in]
 *    None
 * \par Output
 *    None
 * \return
 *    None
 * \par Others
 *    None
 */
void writeHead(void)
{
  writeSerial(0xff);
  writeSerial(0x55);
}

/**
 * \par Function
 *    writeEnd
 * \par Description
 *    This function use to write the terminator of transmission frame.
 * \param[in]
 *    None
 * \par Output
 *    None
 * \return
 *    None
 * \par Others
 *    None
 */
void writeEnd(void)
{
  if(BluetoothSource == DATA_SERIAL)
  {
    Serial.println();
  }
  else if(BluetoothSource == DATA_SERIAL2)
  {
    Serial2.println();
  }
  else if(BluetoothSource == DATA_SERIAL3)
  {
    Serial3.println();
  }
}

/**
 * \par Function
 *    writeSerial
 * \par Description
 *    This function use to write the data to serial.
 * \param[in]
 *    c - The data need to be write.
 * \par Output
 *    None
 * \return
 *    None
 * \par Others
 *    None
 */
void writeSerial(uint8_t c)
{
  if(BluetoothSource == DATA_SERIAL)
  {
    Serial.write(c);
  }
  else if(BluetoothSource == DATA_SERIAL2)
  {
    Serial2.write(c);
  }
  else if(BluetoothSource == DATA_SERIAL3)
  {
    Serial3.write(c);
  }
}

/**
 * \par Function
 *    readSerial
 * \par Description
 *    This function use to read the data from serial, and fill the data
 *    to its buffer.
 * \param[in]
 *    None
 * \par Output
 *    None
 * \return
 *    None
 * \par Others
 *    None
 */
void readSerial(void)
{
  isAvailable = false;
  if(Serial.available() > 0)
  {
    isAvailable = true;
    BluetoothSource = DATA_SERIAL;
    serialRead = Serial.read();
  }
  else if(Serial2.available() > 0)
  {
    isAvailable = true;
    BluetoothSource = DATA_SERIAL2;
    serialRead = Serial2.read();
  }
  else if(Serial3.available() > 0)
  {
    isAvailable = true;
    BluetoothSource = DATA_SERIAL3;
    serialRead = Serial3.read();
  }
}

/**
 * \par Function
 *    parseData
 * \par Description
 *    This function use to process the data from the serial port,
 *    call the different treatment according to its action.
 *    ff 55 len idx action device port  slot  data a
 *    0  1  2   3   4      5      6     7     8
 * \param[in]
 *    None
 * \par Output
 *    None
 * \return
 *    None
 * \par Others
 *    None
 */
void parseData(void)
{
  isStart = false;
  uint8_t idx = readBuffer(3);
  uint8_t action = readBuffer(4);
  uint8_t device = readBuffer(5);
  command_index = (uint8_t)idx;
  switch(action)
  {
    case GET:
      {
        readSensor(device);
        writeEnd();
      }
      break;
    case RUN:
      {
        runModule(device);
        callOK();
      }
      break;
    case RESET:
      {
        //reset
        /* reset dc motor on driver port */
        dc_motor.reset(PORT1A);
        dc_motor.run(0);
        dc_motor.reset(PORT1B);
        dc_motor.run(0);
        dc_motor.reset(PORT2A);
        dc_motor.run(0);
        dc_motor.reset(PORT2B);
        dc_motor.run(0);

        /* reset stepper motor driver */
        
        callOK();
      }
      break;
     case START:
      {
        //start
        callOK();
      }
      break;
  }
}

/**
 * \par Function
 *    callOK
 * \par Description
 *    Response for executable commands.
 * \param[in]
 *    None
 * \par Output
 *    None
 * \return
 *    None
 * \par Others
 *    None
 */
void callOK(void)
{
  writeSerial(0xff);
  writeSerial(0x55);
  writeEnd();
}

/**
 * \par Function
 *    sendByte
 * \par Description
 *    Send byte data
 * \param[in]
 *    c - the byte data need be sent.
 * \par Output
 *    None
 * \return
 *    None
 * \par Others
 *    None
 */
void sendByte(uint8_t c)
{
  writeSerial(1);
  writeSerial(c);
}

/**
 * \par Function
 *    sendString
 * \par Description
 *    Send string data
 * \param[in]
 *    s - the string data need be sent.
 * \par Output
 *    None
 * \return
 *    None
 * \par Others
 *    None
 */
void sendString(String s)
{
  int16_t l = s.length();
  writeSerial(4);
  writeSerial(l);
  for(int16_t i=0;i<l;i++)
  {
    writeSerial(s.charAt(i));
  }
}

/**
 * \par Function
 *    sendFloat
 * \par Description
 *    Sned float data
 * \param[in]
 *    value - the float data need be sent.
 * \par Output
 *    None
 * \return
 *    None
 * \par Others
 *    None
 */
void sendFloat(float value)
{ 
  writeSerial(2);
  val.floatVal = value;
  writeSerial(val.byteVal[0]);
  writeSerial(val.byteVal[1]);
  writeSerial(val.byteVal[2]);
  writeSerial(val.byteVal[3]);
}

/**
 * \par Function
 *    sendLong
 * \par Description
 *    Sned long data
 * \param[in]
 *    value - the long data need be sent.
 * \par Output
 *    None
 * \return
 *    None
 * \par Others
 *    None
 */
void sendLong(long value)
{ 
  writeSerial(6);
  val.longVal = value;
  writeSerial(val.byteVal[0]);
  writeSerial(val.byteVal[1]);
  writeSerial(val.byteVal[2]);
  writeSerial(val.byteVal[3]);
}

/**
 * \par Function
 *    sendShort
 * \par Description
 *    Sned short data
 * \param[in]
 *    value - the short data need be sent.
 * \par Output
 *    None
 * \return
 *    None
 * \par Others
 *    None
 */
void sendShort(int16_t value)
{
  writeSerial(3);
  valShort.shortVal = value;
  writeSerial(valShort.byteVal[0]);
  writeSerial(valShort.byteVal[1]);
}

/**
 * \par Function
 *    sendDouble
 * \par Description
 *    Sned double data, same as float data on arduino.
 * \param[in]
 *    value - the double data need be sent.
 * \par Output
 *    None
 * \return
 *    None
 * \par Others
 *    None
 */
void sendDouble(double value)
{
  writeSerial(5);
  valDouble.doubleVal = value;
  writeSerial(valDouble.byteVal[0]);
  writeSerial(valDouble.byteVal[1]);
  writeSerial(valDouble.byteVal[2]);
  writeSerial(valDouble.byteVal[3]);
}

/**
 * \par Function
 *    readShort
 * \par Description
 *    read the short data.
 * \param[in]
 *    idx - The data's first address in the array.
 * \par Output
 *    None
 * \return
 *    the short data.
 * \par Others
 *    None
 */
int16_t readShort(int16_t idx)
{
  valShort.byteVal[0] = readBuffer(idx);
  valShort.byteVal[1] = readBuffer(idx+1);
  return valShort.shortVal; 
}

/**
 * \par Function
 *    readFloat
 * \par Description
 *    read the float data.
 * \param[in]
 *    idx - The data's first address in the array.
 * \par Output
 *    None
 * \return
 *    the float data.
 * \par Others
 *    None
 */
float readFloat(int16_t idx)
{
  val.byteVal[0] = readBuffer(idx);
  val.byteVal[1] = readBuffer(idx+1);
  val.byteVal[2] = readBuffer(idx+2);
  val.byteVal[3] = readBuffer(idx+3);
  return val.floatVal;
}

/**
 * \par Function
 *    readLong
 * \par Description
 *    read the long data.
 * \param[in]
 *    idx - The data's first address in the array.
 * \par Output
 *    None
 * \return
 *    the long data.
 * \par Others
 *    None
 */
long readLong(int16_t idx)
{
  val.byteVal[0] = readBuffer(idx);
  val.byteVal[1] = readBuffer(idx+1);
  val.byteVal[2] = readBuffer(idx+2);
  val.byteVal[3] = readBuffer(idx+3);
  return val.longVal;
}

char _receiveStr[20] = {};
uint8_t _receiveUint8[16] = {};

/**
 * \par Function
 *    readString
 * \par Description
 *    read the string data.
 * \param[in]
 *    idx - The string's first address in the array.
 * \param[in]
 *    len - The length of the string data.
 * \par Output
 *    None
 * \return
 *    the address of string data.
 * \par Others
 *    None
 */
char* readString(int16_t idx,int16_t len)
{
  for(int16_t i=0;i<len;i++)
  {
    _receiveStr[i]=readBuffer(idx+i);
  }
  _receiveStr[len] = '\0';
  return _receiveStr;
}

/**
 * \par Function
 *    readUint8
 * \par Description
 *    read the uint8 data.
 * \param[in]
 *    idx - The Uint8 data's first address in the array.
 * \param[in]
 *    len - The length of the uint8 data.
 * \par Output
 *    None
 * \return
 *    the address of uint8 data.
 * \par Others
 *    None
 */
uint8_t* readUint8(int16_t idx,int16_t len)
{
  for(int16_t i=0;i<len;i++)
  {
    if(i > 15)
    {
      break;
    }
    _receiveUint8[i] = readBuffer(idx+i);
  }
  return _receiveUint8;
}


/**
 * \par Function
 *    runModule
 * \par Description
 *    Processing execute commands.
 * \param[in]
 *    device - The definition of all execute commands.
 * \par Output
 *    None
 * \return
 *    None
 * \par Others
 *    None
 */
void runModule(uint8_t device)
{
  //0xff 0x55 0x6 0x0 0x1 0xa 0x9 0x0 0x0 0xa
  uint8_t port = readBuffer(6);
  uint8_t pin = port;
  switch(device)
  {
    case MOTOR:
      {
        int16_t speed = readShort(7);
        dc_motor.reset(port);
        dc_motor.run(speed);
      }
      break;
    
    case COMMON_COMMONCMD:
      {
        uint8_t subcmd = port;
        uint8_t cmd_data = readBuffer(7);
        if(SET_MEGAPI_MODE == subcmd)
        {
          // Stop();
          move_control(0, 0, 0);
          if((cmd_data == OBSTACLE_MODE) || (cmd_data == LINE_FOLLOW_MODE)) 
          {
            megapi_mode = cmd_data;
          }
          else
          {
            megapi_mode = BLUETOOTH_MODE;
            new_rgbled_show_all(0,0,RGB_LOW_VAL,0,0,RGB_LOW_VAL,0);
          }
        }
      }
      break;
    
    case DIGITAL:
      {
        pinMode(pin,OUTPUT);
        uint8_t v = readBuffer(7);
        digitalWrite(pin,v);
     }
     break;
    case PWM:
      {
        pinMode(pin,OUTPUT);
        uint8_t v = readBuffer(7);
        analogWrite(pin,v);
      }
      break;

    case TIMER:
      {
        lastTime = millis()/1000.0; 
      }
      break;
    case CARE_CONTROL:
      {
        if(port == 0)
        {
           int16_t vx = readShort(7);
           int16_t vy = readShort(9);
           int16_t vw = readShort(11);
           move_control(vx, vy, vw);
        }
      }
      break;
    case RGBLED:
      {
        uint8_t index = readBuffer(8);
        uint8_t rgb[3] = {0};
        rgb[0] = readBuffer(9);
        rgb[1] = readBuffer(10);
        rgb[2] = readBuffer(11);
        new_rgbled_show(port,index,rgb);
      }
  }
}


/**
 * \par Function
 *    readSensor
 * \par Description
 *    This function is used to process query command.
 * \param[in]
 *    device - The definition of all query commands.
 * \par Output
 *    None
 * \return
 *    None
 * \par Others
 *    None
 */
void readSensor(uint8_t device)
{
  /**************************************************
      ff 55 len idx action device port slot data a
      0  1  2   3   4      5      6    7    8
  ***************************************************/
  float value=0.0;
  uint8_t port,slot,pin;
  port = readBuffer(6);
  pin = port;
  writeHead();
  writeSerial(command_index);
  switch(device)
  {  
    case VERSION:
      {
        sendString(mVersion);
      }
      break;
    case DIGITAL:
      {
        pinMode(pin,INPUT);
        sendFloat(digitalRead(pin));
      }
      break;
    case ANALOG:
      {
        pin = analogs[pin];
        pinMode(pin,INPUT);
        sendFloat(analogRead(pin));
      }
      break;
    case PULSEIN:
      {
        int16_t pw = readShort(7);
        pinMode(pin, INPUT);
        sendLong(pulseIn(pin,HIGH,pw));
      }
      break;
    
    case TIMER:
      {
        sendFloat((float)currentTime);
      }
      break;
  
    case COMMON_COMMONCMD:
      {
        uint8_t subcmd = port;
        if(GET_MEGAPI_MODE == subcmd)
        {
          sendByte(megapi_mode);
        }
      }
      break;
    
    case SINGLE_LINE_FOLLOWER:
      {
        pinMode(pin,INPUT);
        sendFloat(digitalRead(pin));
      }
      break;

    case COLLISION:
      {
        pinMode(pin,INPUT);
        sendFloat(digitalRead(pin));
      }
      break;

    case OBSTACLE:
      {
        pinMode(pin,INPUT);
        sendFloat(digitalRead(pin));
      }
      break;

    default:
      {
        sendFloat(0);
      }
      break;
  }//switch
}

/**
 * \par Function
 *    read_lineflower_sta
 * \par Description
 *    This function is used to read lineflower state.
 * \param[in]
 *    none.
 * \par Output
 *    uint8_t
 * \return
 *    None
 * \par Others
 *    None
 */
uint8_t read_lineflower_sta(void)
{
  uint8_t sta = 0;
  sta = line_follower_s1.readSensor();
  sta |= line_follower_s2.readSensor() << 1 ;
  
  return sta;
}

/**
 * \par Function
 *    line_model
 * \par Description
 *    The main function for Patrol Line navigation mode
 * \param[in]
 *    None
 * \par Output
 *    None
 * \return
 *    None
 * \par Others
 *    None
 */
void line_model(void)
{
  uint8_t sta = 0;
  int16_t vx = 0;
  int16_t vy = 0;
  int16_t vw = 0;
  static int8_t line_follow_sta_value = MAX_VAL;
  static int8_t diff_val = 0;

  sta = read_lineflower_sta();
  switch (sta)
  {
    case SS2_IN_S1_IN:
      new_rgbled_show_all(0,0,0,RGB_HIGH_VAL,0,0,RGB_HIGH_VAL);
      //Crossroads or T-junctions
      line_follow_sta_value = 0;
      diff_val = 0;
      break;

    case SS2_IN_S1_OUT:
      new_rgbled_show_all(0,0,0,RGB_LOW_VAL,0,0,RGB_HIGH_VAL);
      if(line_follow_sta_value < MAX_VAL) line_follow_sta_value++;
      diff_val = 0;
      break;

    case SS2_OUT_S1_IN:
      new_rgbled_show_all(0,0,0,RGB_HIGH_VAL,0,0,RGB_LOW_VAL);
      if(line_follow_sta_value > -1*MAX_VAL) line_follow_sta_value--;
      diff_val = 0;
      break;

    case SS2_OUT_S1_OUT:
      new_rgbled_show_all(0,0,0,RGB_LOW_VAL,0,0,RGB_LOW_VAL);
      if(line_follow_sta_value < 0) line_follow_sta_value = -1*MAX_VAL;
      else if(line_follow_sta_value > 0) line_follow_sta_value = MAX_VAL;
      else line_follow_sta_value = 0;
      diff_val = line_follow_sta_value;
      break;    
    
    default:
      break;
  }

  // Serial.print("Sta: ");
  // Serial.print(sta);
  // Serial.print(", ");
  // Serial.print("Val: ");
  // Serial.print(line_follow_sta_value);
  // Serial.print("diff: ");
  // Serial.println(diff_val);

  vx = 0;
  vy = FORWARD_SPEED;
  vw = diff_val * TURN_SPEED;

  move_control(vx, vy, vw);
  delay(2); 
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


/**
 * \par Function
 *    gesture_model
 * \par Description
 *    This function is used to detect gesture, run in roop function.
 * \param[in]
 *    none.
 * \par Output
 *    none.
 * \return
 *    None
 * \par Others
 *    None
 */
void gesture_model(void)
{
  uint8_t gesture_type;
  static uint8_t move_type = 0;
  static uint8_t init_led_flag = false;
  static uint32_t s_movement_start_time = 0;

  if(init_led_flag == false)
  {
    init_led_flag = true;
    new_rgbled_show_all(0,0,RGB_LOW_VAL,0,0,RGB_LOW_VAL,0);
  }
  
  gesture_type = detect_gesture();
  
  // Printout gesture type
  if(gesture_type == GESTURE_TYPE_LEFT_TO_RIGHT)
  {
    new_rgbled_show_all(0,0,RGB_HIGH_VAL,0,0,RGB_HIGH_VAL,0);
    move_type = 1;
    s_movement_start_time = millis();
    move_control(GESTURE_MOVE_SPEED, 0, 0);
    // Serial.println("gesture: left to right");
  }
  else if(gesture_type == GESTURE_TYPE_RIGHT_TO_LEFT)
  {
    new_rgbled_show_all(0,0,RGB_HIGH_VAL,0,0,RGB_HIGH_VAL,0);
    move_type = 2;
    move_control(-1 * GESTURE_MOVE_SPEED, 0, 0);
    s_movement_start_time = millis();
    // Serial.println("gesture: right to left");
  }
  else if(gesture_type == GESTURE_TYPE_ALL)
  {
    new_rgbled_show_all(0,0,RGB_HIGH_VAL,0,0,RGB_HIGH_VAL,0);
    move_type = 3;
    move_control(0, -1 * GESTURE_MOVE_SPEED, 0);
    s_movement_start_time = millis();
    // Serial.println("gesture: all");
  } 

  if((move_type==1) || (move_type==2))
  {
    if((millis() - s_movement_start_time) > 1000)
    {
      new_rgbled_show_all(0,0,RGB_LOW_VAL,0,0,RGB_LOW_VAL,0);
      // stop();
      move_control(0, 0, 0);
      move_type = 0;
    }
  }
  else if(move_type==3)
  {  
    if((millis() - s_movement_start_time) > 200)
    {
      new_rgbled_show_all(0,0,RGB_LOW_VAL,0,0,RGB_LOW_VAL,0);
      // stop();
      move_control(0, 0, 0);
      move_type = 0;
    }
  }

}


/**
 * \par Function
 *    read_collision_sta
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
uint8_t read_collision_sta(void)
{
  uint8_t sta = 0;
  sta = collision_s1.readSensor();
  sta |= collision_s2.readSensor() << 1 ;
  
  return sta;
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
uint8_t read_obstacle_sta(void)
{
  uint8_t sta = 0;
  sta = barrier_s1.readSensor();
  sta |= barrier_s2.readSensor() << 1 ;
  sta |= barrier_s3.readSensor() << 2 ;  
  return sta;
}


/**
 * \par Function
 *    gesture_model
 * \par Description
 *    This function is used to deal with collision model, run in roop function.
 * \param[in]
 *    none.
 * \par Output
 *    none.
 * \return
 *    None
 * \par Others
 *    None
 */
void obstacle_model(void)
{
  uint8_t sta = 0;
  static int16_t s_vx = 0;
  static int16_t s_vy = OBSTACLE_FORWARD_SPEED;
  static int16_t s_vw = 0;
  static uint32_t s_obstacle_start_time = 0;
  static uint8_t s_back_or_turn_flag = 0;
  sta = read_obstacle_sta();

  if(s_back_or_turn_flag == 1 && (millis() - s_obstacle_start_time > 1000))
  {
    s_obstacle_start_time = millis();
    s_back_or_turn_flag = 0;
  }
  else if(s_back_or_turn_flag == 0)
  {
    s_obstacle_start_time = millis();
  }

  switch (sta)
  {
    //back
    case S3_FREE_S2_OBJ_S1_FREE:
    case S3_OBJ_S2_FREE_S1_OBJ:
    case S3_OBJ_S2_OBJ_S1_OBJ:
      new_rgbled_show_all(0,RGB_HIGH_VAL,RGB_HIGH_VAL,0,RGB_HIGH_VAL,RGB_HIGH_VAL,0);
      if(s_back_or_turn_flag == 0)
      {
        s_vx= 0;
        s_vy = -1 * OBSTACLE_FORWARD_SPEED;
        s_vw = OBSTACLE_TURN_SPEED;
        s_back_or_turn_flag = 1;
      }
      break;
    
    //turn left
    case S3_OBJ_S2_FREE_S1_FREE:
    case S3_OBJ_S2_OBJ_S1_FREE:
      new_rgbled_show_all(0,RGB_HIGH_VAL,RGB_HIGH_VAL,0,RGB_HIGH_VAL,RGB_HIGH_VAL,0);
      if(s_back_or_turn_flag == 0)
      {
        s_vx= 0;
        s_vy = 0;
        s_vw = -1 * OBSTACLE_TURN_SPEED;
        s_back_or_turn_flag = 1;
      }
      break;

    //turn right
    case S3_FREE_S2_OBJ_S1_OBJ:
    case S3_FREE_S2_FREE_S1_OBJ:
      new_rgbled_show_all(0,RGB_HIGH_VAL,RGB_HIGH_VAL,0,RGB_HIGH_VAL,RGB_HIGH_VAL,0);
      if(s_back_or_turn_flag == 0)
      {
        s_vx= 0;
        s_vy = 0;
        s_vw = OBSTACLE_TURN_SPEED;
        s_back_or_turn_flag = 1;
      }
      break;

    //forward
    case S3_FREE_S2_FREE_S1_FREE:
      new_rgbled_show_all(0,RGB_LOW_VAL,RGB_LOW_VAL,0,RGB_LOW_VAL,RGB_LOW_VAL,0);
      if(s_back_or_turn_flag == 0)
      {
        s_vx= 0;
        s_vy = OBSTACLE_FORWARD_SPEED;
        s_vw = 0;
      }
      break;    
    
    default:
      break;
  }

  move_control(s_vx, s_vy, s_vw);
}





/**
 * \par Function
 *    switch_mode
 * \par Description
 *    This function is used to switch mode, run in roop function.
 * \param[in]
 *    none.
 * \par Output
 *    none.
 * \return
 *    None
 * \par Others
 *    None
 */
void switch_mode(void)
{
  uint8_t sta = 0;
  static uint8_t s_last_sta = 0;
  static uint8_t s_last_mode = 0;
  static uint32_t s_start_time = 0;
  static uint8_t s_setted_flag = 0;

  sta = read_collision_sta();

  if(s_last_sta != sta)
  {
    s_start_time = millis();
    s_setted_flag = 0;
  }

  switch (sta)
  {
    case S2_COL_S1_COL:
      s_start_time = millis();

      break;

    //mode++
    case S2_COL_S1_FREE:
      if(millis() - s_start_time > 1000 && s_setted_flag == 0)
      {
        if(megapi_mode < MAX_MODE)
        {
          megapi_mode++;
        }

        if(megapi_mode == MAX_MODE)
        {
          megapi_mode = BLUETOOTH_MODE;
        }
        
        if(megapi_mode == BLUETOOTH_MODE)
        {
          new_rgbled_show_all(0,0,RGB_LOW_VAL,0,0,RGB_LOW_VAL,0);
        }
        else if(megapi_mode == OBSTACLE_MODE)
        {
          new_rgbled_show_all(0,RGB_LOW_VAL,RGB_LOW_VAL,0,RGB_LOW_VAL,RGB_LOW_VAL,0);         
        }
        else if(megapi_mode == LINE_FOLLOW_MODE)
        {
          new_rgbled_show_all(0,0,0,RGB_LOW_VAL,0,0,RGB_LOW_VAL);  
        }

        s_setted_flag = 1;
      }

      break;
    
    //mode--
    case S2_FREE_S1_COL:
      if(millis() - s_start_time > 1000 && s_setted_flag == 0)
      {
        if(megapi_mode > BLUETOOTH_MODE)
        {
          megapi_mode--;
        }
        else if(megapi_mode == BLUETOOTH_MODE)
        {
          megapi_mode = LINE_FOLLOW_MODE;
        }

        if(megapi_mode == BLUETOOTH_MODE)
        {
          new_rgbled_show_all(0,0,RGB_LOW_VAL,0,0,RGB_LOW_VAL,0);
        }
        else if(megapi_mode == OBSTACLE_MODE)
        {
          new_rgbled_show_all(0,RGB_LOW_VAL,RGB_LOW_VAL,0,RGB_LOW_VAL,RGB_LOW_VAL,0);         
        }
        else if(megapi_mode == LINE_FOLLOW_MODE)
        {
          new_rgbled_show_all(0,0,0,RGB_LOW_VAL,0,0,RGB_LOW_VAL);  
        }

        s_setted_flag = 1;
      }
      break;

    case S2_FREE_S1_RREE:
      s_start_time = millis();

      break;    
    
    default:
      break;
  }

  if(s_last_mode != megapi_mode)
  {
    move_control(0, 0, 0);
  }

  s_last_sta = sta;
  s_last_mode = megapi_mode;

}


/**
 * \par Function
 *    read_serial
 * \par Description
 *    The function used to process serial data.
 * \param[in]
 *    None
 * \par Output
 *    None
 * \return
 *    Is there a valid command 
 * \par Others
 *    None
 */
boolean read_serial(void)
{
  boolean result = false;
  readSerial();
  if(isAvailable)
  {
    uint8_t c = serialRead & 0xff;
    result = true;
    if((c == 0x55) && (isStart == false))
    {
      if(prevc == 0xff)
      {
        index=1;
        isStart = true;
      }
    }
    else
    {
      prevc = c;
      if(isStart)
      {
        if(index == 2)
        {
          dataLen = c; 
        }
        else if(index > 2)
        {
          dataLen--;
        }
        writeBuffer(index,c);
      }
    }
    index++;
    if(index > 51)
    {
      index=0; 
      isStart=false;
    }
    if(isStart && (dataLen == 0) && (index > 3))
    { 
      isStart = false;
      parseData(); 
      index=0;
    }
    return result;
  }
}


void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200);
  Serial3.begin(115200);
  while(!Serial){}
  while(!Serial2){}
  while(!Serial3){}
  delay(5);

  pinMode(13,OUTPUT);

  //Set Pwm 970Hz
  TCCR1A = _BV(WGM10);
  TCCR1B = _BV(CS11) | _BV(CS10) | _BV(WGM12);
  TCCR2A = _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS22);
  TCCR3A = _BV(WGM30);
  TCCR3B = _BV(CS31) | _BV(CS30) | _BV(WGM32);
  TCCR4A = _BV(WGM40);
  TCCR4B = _BV(CS41) | _BV(CS40) | _BV(WGM42);

  //megapi_mode = BALANCED_MODE;
  Serial.print("Version: ");
  Serial.println(mVersion);
  blink_time = millis();
  BluetoothSource = DATA_SERIAL;
}

/**
 * \par Function
 *    loop
 * \par Description
 *    main function for arduino
 * \param[in]
 *    None
 * \par Output
 *    None
 * \return
 *    None
 * \par Others
 *    None
 */
void loop()
{
  currentTime = millis()/1000.0-lastTime;

  if(millis() - blink_time > 1000)
  {
    blink_time = millis();
    blink_flag = !blink_flag;
    digitalWrite(13,blink_flag);
  }

  readSerial();
  while(isAvailable)
  {
    unsigned char c = serialRead & 0xff;
    if((c == 0x55) && (isStart == false))
    {
      if(prevc == 0xff)
      {
        index=1;
        isStart = true;
      }
    }
    else
    {
      prevc = c;
      if(isStart)
      {
        if(index == 2)
        {
          dataLen = c; 
        }
        else if(index > 2)
        {
          dataLen--;
        }
        writeBuffer(index,c);
      }
    }
    index++;
    if(index > 51)
    {
      index=0; 
      isStart=false;
    }
    if(isStart && (dataLen == 0) && (index > 3))
    { 
      isStart = false;
      parseData(); 
      index=0;
    }
    readSerial();
  }

  switch_mode();

  if(megapi_mode == BLUETOOTH_MODE)
  {
    gesture_model();
  }
  else if(megapi_mode == OBSTACLE_MODE)
  {
    obstacle_model();
  }
  else if(megapi_mode == LINE_FOLLOW_MODE)
  {
    line_model();
  }
}
