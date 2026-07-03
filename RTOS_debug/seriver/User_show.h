#ifndef __USER_SHOW_H__
#define __USER_SHOW_H__

#include "math.h"
#include "Show_Thread.h"
#include "my_OLED.h"
#include "my_key.h"
#include "car.h"
#include "trilateration.h"
#include "my_IMU.h"
#include "my_ultrasonic_ADC.h"

extern bool door_state;
extern float yaw_set;
extern SemaphoreHandle_t run_useflag;
extern SemaphoreHandle_t go_useflag;
extern	float run[3];
extern float go[2];

extern	uint8_t Page_flag;
extern	uint8_t Row_flag;
extern	uint8_t Depth_flag;
extern	uint8_t Line_flag;

extern	uint8_t Old_Page_flag;
extern	uint8_t Old_Row_flag;
extern	uint8_t Old_Depth_flag;
extern	uint8_t Old_Line_flag;

extern	int distance;
extern	int old_distance;

extern	uint8_t Depth_list[5];
extern	uint8_t Old_Depth_list[5];


extern	bool cursor_flag;
extern	bool new_flag;
extern	bool cursor_show;


extern	int delay_flag;

extern	SemaphoreHandle_t run_useflag;
extern	float run[3];

extern	double ol_temp[3];
extern	double old_ol_temp[3];

extern	SemaphoreHandle_t go_useflag;
extern	float go[2];

extern	float yaw_set;
extern	bool door_state;




void show_init(void);
void show_all(void);

#endif
