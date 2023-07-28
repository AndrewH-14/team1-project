/**
 * Header for all MakeBlock motor functionality.
 */
#include <Arduino.h>

#ifndef MOTOR_H
#define MOTOR_H

void motor_move_forward(uint8_t speed, uint16_t time_ms);
void motor_move_backward(uint8_t speed, uint16_t time_ms);
void motor_move_left(uint8_t speed, uint16_t time_ms);
void motor_move_right(uint8_t speed, uint16_t time_ms);

void motor_turn_left(uint8_t speed, uint16_t time_ms);
void motor_turn_right(uint8_t speed, uint16_t time_ms);

#endif // MOTOR_H
