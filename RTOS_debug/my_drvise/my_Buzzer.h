#ifndef __MY_BUZZER_H__
#define __MY_BUZZER_H__

#include "Car_Thread.h"

#define HIGH_TIME		400
#define LOW_TIME		200
#define FEER_TIME		200
#define BUZZER_PIN	BSP_IO_PORT_08_PIN_04	

void Morse(uint8_t data);
extern const uint8_t Morse_data[10][5];
 void	music_one(uint8_t data);
#endif

