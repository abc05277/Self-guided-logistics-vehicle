#include "User_show.h"

//4*16
uint8_t Page_flag = 1;
uint8_t Row_flag = 1;
uint8_t Depth_flag = 1;
uint8_t Line_flag = 1;

uint8_t Old_Page_flag = 1;
uint8_t Old_Row_flag = 1;
uint8_t Old_Depth_flag = 1;
uint8_t Old_Line_flag = 1;

int distance;
int old_distance;

uint8_t Depth_list[5] = {0,0,0,0,0};
uint8_t Old_Depth_list[5] = {0,0,0,0,0};


bool cursor_flag = false;
bool new_flag = false;
bool cursor_show = true;


int delay_flag = 500;

SemaphoreHandle_t run_useflag;
float run[3] = {15,300,0};

double ol_temp[3];
double old_ol_temp[3];

SemaphoreHandle_t go_useflag;
float go[2];

float yaw_set;
bool door_state = false;



void Clamp_uint8_t(uint8_t *temp,uint8_t L, uint8_t H);
void Clamp_float(float *temp,float L, float H);
void Clamp_double(double *temp,double L, double H);
void new_cursor_back(void);
void run_num_new(void);
void go_num_new(void);
void IMU_num_new(void);
void UWB_num_new(void);
void Ult_num_new(void);
void Turn_num_new(void);
void set_cell_new(void);


vec3d report_temp;
vec3d old_report_temp;

vec3d Cell_Site[4] = {(vec3d){A0_X,A0_Y,A0_Z},(vec3d){A1_X,A1_Y,A1_Z},(vec3d){A2_X,A2_Y,A2_Z},(vec3d){A3_X,A3_Y,A3_Z}};
void show_init(void)
{
	run_useflag = xSemaphoreCreateMutex();
	go_useflag = xSemaphoreCreateMutex();
	OLED_Init();
	Key_init();
}

void show_all(void)
{
	int i;
	
	
	delay_flag--;
	
	/******************************!!!!!!!!!!!!!!!!!按键操作!!!!!!!!!!!!!!!!!!!!!!!!!!!!************************************/
	Key_NumT Key_num = Key_Read();		//按键读取
	switch(Key_num)										//按键操作
	{
		case Key_U_Num:
			if(Depth_flag < 4)
				OLED_ShowString(((Row_flag-2)%3)+2,16," ");
			Row_flag--;
			break;
		case Key_D_Num:
			if(Depth_flag < 4)
				OLED_ShowString(((Row_flag-2)%3)+2,16," ");
			Row_flag++;
			break;
		case Key_L_Num:
			Page_flag--;
			break;
		case Key_R_Num:
			Page_flag++;
			break;
		case Key_I_Num:
			if(Depth_flag < 6)
			{
				Depth_list[Depth_flag-1] = Row_flag;
				Depth_flag++;
			}
			break;
		case Key_O_Num:
			if(Depth_flag > 1)
			{
				Depth_list[Depth_flag-2] = 0;
				Depth_flag--;
			}
			break;
		default:
			break;
	}
	
	if(Depth_flag < 4	&& cursor_flag)	//光标切回
	{
		new_cursor_back();
		cursor_flag =	false;
	}
	
	

/******************************!!!!!!!!!!!!!!!!!!!数据限幅!!!!!!!!!!!!!!!!!!************************************/

	/***************************************************************************************/
	if(Depth_flag == 1)							//首页	限幅
	{
		Clamp_uint8_t(&Row_flag,3,4);
		Page_flag = 1;											//页码恒为1
	}
	/***************************************************************************************/
	else if(Depth_flag == 2 && Depth_list[0] == 3)		//小车控制页	限幅
	{
		Clamp_uint8_t(&Row_flag,2,8);
		if((Old_Row_flag == 4&&Row_flag == 5) || (Old_Row_flag == 5&&Row_flag == 4)||(Old_Row_flag == 7&&Row_flag == 8) || (Old_Row_flag == 8&&Row_flag == 7))		//翻行刷新
			new_flag = false;
		Page_flag = 1;											//页码恒为1
	}
	/***************************************************************************************/
	else if(Depth_flag == 3	&&	Depth_list[0]	== 3 && Depth_list[1] == 2)		//以特定速度角度跑距离	限幅
	{
		Clamp_uint8_t(&Row_flag,1,4);
		Page_flag = 1;	//页码恒为1
	}
	/**************************************************************************************/
	else if(Depth_flag == 3	&&	Depth_list[0]	== 3 && Depth_list[1] == 3)		//绝对角度	限幅
	{
		Clamp_uint8_t(&Row_flag,1,2);
		Page_flag = 1;	//页码恒为1
	}
	/********************************************************************************/
	else if(Depth_flag == 3	&&	Depth_list[0]	== 3 && Depth_list[1] == 4)	//相对角度	限幅
	{
		Clamp_uint8_t(&Row_flag,1,2);
		Page_flag = 1;	//页码恒为1
	}

	/********************************************************************************/
	else if(Depth_flag == 3	&&	Depth_list[0]	== 3 && Depth_list[1] == 6)	//以特定角度避障	限幅
	{
		Clamp_uint8_t(&Row_flag,1,3);
		Page_flag = 1;	//页码恒为1
	}
		/********************************************************************************/
	else if(Depth_flag == 3	&&	Depth_list[0]	== 3 && Depth_list[1] == 7)	//前往对应坐标	限幅
	{
		Clamp_uint8_t(&Row_flag,1,4);
		Page_flag = 1;	//页码恒为1
	}

	/**************************************************************************************/
	else if(Depth_flag == 3	&&	Depth_list[0]	== 3 && Depth_list[1] == 8)		//前往对应坐标 加避障	限幅
	{
		Clamp_uint8_t(&Row_flag,1,4);
		Page_flag = 1;	//页码恒为1
	}
	/************************************************************/
	else if(Depth_flag == 2 && Depth_list[0] == 4)	//小车详情页限幅
	{
		Clamp_uint8_t(&Row_flag,2,5);
		if((Old_Row_flag == 4&&Row_flag == 5) || (Old_Row_flag == 5&&Row_flag == 4))		//翻行刷新
			new_flag = false;
		Page_flag = 1;											//页码恒为1
	}
	/**************************************************************************************************/
	else if(Page_flag == 1	&&	Depth_flag >= 3	&&	Depth_list[0]	== 4 && Depth_list[1] == 2)	//显示惯导限幅及刷新
	{
		if(Depth_flag == Old_Depth_flag)
			IMU_num_new();
		Clamp_uint8_t(&Row_flag,1,1);
		Clamp_uint8_t(&Depth_flag,1,3);
		Depth_list[2] = 0;
		Page_flag = 1;	//页码恒为1
	}
	else if(Page_flag == 1	&&	Depth_flag >= 3	&&	Depth_list[0]	== 4 && Depth_list[1] == 3)	//显示惯导限幅及刷新
	{
		if(Depth_flag == Old_Depth_flag)
			UWB_num_new();
		Clamp_uint8_t(&Row_flag,1,1);
		Clamp_uint8_t(&Depth_flag,1,3);
		Depth_list[2] = 0;
		Page_flag = 1;	//页码恒为1
	}	/********************************************************************************************/
	else if(Page_flag == 1	&&	Depth_flag >= 3	&&	Depth_list[0]	== 4 && Depth_list[1] == 4)	//显示超声波限幅及刷新
	{
		if(Depth_flag == Old_Depth_flag)
			Ult_num_new();
		
		Clamp_uint8_t(&Row_flag,1,1);
		Clamp_uint8_t(&Depth_flag,1,3);
		Depth_list[2] = 0;
		Page_flag = 1;	//页码恒为1
	}
	else if(Depth_flag == 3	&&	Depth_list[0]	== 4 && Depth_list[1] == 5)	//基站坐标设置限幅
	{
		Clamp_uint8_t(&Row_flag,2,4);
		Clamp_uint8_t(&Page_flag,1,4);
	}
	/******************************!!!!!!!!!!!!!!!!!!!!!!!!!!!!数据修改!!!!!!!!!!!!!!!!************************************/
	
	
	/***************************************************************************************/
	
	if(Old_Depth_flag == 3	&& Depth_flag == 2 &&	Depth_list[0]	== 3 && Old_Depth_list[1] == 2)//跳出函数执行
	{
			sum_flag =	false;
	}
	if(Old_Depth_flag == 3	&&	Depth_flag == 4	&&	Depth_list[0]	== 3 && Depth_list[1] == 2)	//以特定速度角度跑距离修改数据初始化列标
	{
		if(Row_flag == 4) Line_flag = 7;
		else Line_flag = 8;
	}
	
	else if(Depth_flag > 3	&&	Depth_list[0]	== 3 && Depth_list[1] == 2)	//修改数据及确认
	{
		if(Old_Row_flag > 1)//修改数据类
		{
			cursor_flag = true;//切换光标
			if(Page_flag	-	Old_Page_flag == 1)
			{
				new_cursor_back();
				Line_flag++;
			}
			else if(Page_flag	-	Old_Page_flag == -1)
			{
				new_cursor_back();
				Line_flag--;
			}
			
			if(Old_Row_flag == 4)//限幅
			{
				Clamp_uint8_t(&Line_flag,7,13);
			}
			else
			{
				Clamp_uint8_t(&Line_flag,8,13);
			}

			if(Row_flag - Old_Row_flag == 1)//数据更改
			{
				xSemaphoreTake(run_useflag,portMAX_DELAY);
				Row_flag = Old_Row_flag;
				if(Line_flag == 7)	run[2] = -run[2];	
				else if(Line_flag < 11)
					run[Row_flag-2] = run[Row_flag-2] - (float)pow(10,10 - Line_flag);
				else if(Line_flag >11)
					run[Row_flag-2] = run[Row_flag-2] - (float)pow(10,11 - Line_flag);
				Clamp_float(&run[0],0,(float)999.99);
				Clamp_float(&run[1],0,(float)999.99);
				Clamp_float(&run[2],-180,180);
				run_num_new();
				xSemaphoreGive(run_useflag);
			}
			else if(Row_flag - Old_Row_flag == -1)
			{
				xSemaphoreTake(run_useflag,portMAX_DELAY);

				Row_flag = Old_Row_flag;
				if(Line_flag == 7)	run[2] = -run[2];
				else if(Line_flag < 11)
					run[Row_flag-2] = run[Row_flag-2] + (float)pow(10,10 - Line_flag);
				else if(Line_flag >11)
					run[Row_flag-2] = run[Row_flag-2] + (float)pow(10,11 - Line_flag);
				Clamp_float(&run[0],0,(float)999.99);
				Clamp_float(&run[1],0,(float)999.99);
				Clamp_float(&run[2],-180,180);

				run_num_new();
				xSemaphoreGive(run_useflag);
			}
		}
		else
		{
			xSemaphoreTake(do_flag_useflag,portMAX_DELAY);
			do_flag = car_run;
			xSemaphoreGive(do_flag_useflag);
			
			Depth_list[Depth_flag-2] = 0;
			Depth_flag--;
		}
		Clamp_uint8_t(&Depth_flag,1,4);
		Page_flag = 1;	//页码恒为1
	}
/***********************************************************************************************************/
	if(Old_Depth_flag == 3	&&	Depth_flag == 4	&&	Depth_list[0]	== 3 && Depth_list[1] == 3)//绝对角度修改数据初始化列标
		Line_flag = 6;
	else if(Depth_flag > 3	&&	Depth_list[0]	== 3 && Depth_list[1] == 3)	//修改数据及确认
	{
		if(Old_Row_flag > 1)//修改数据类
		{
			cursor_flag = true;//切换光标
			if(Page_flag	-	Old_Page_flag == 1)
			{
				new_cursor_back();
				Line_flag++;
			}
			else if(Page_flag	-	Old_Page_flag == -1)
			{
				new_cursor_back();
				Line_flag--;
			}
			
			Clamp_uint8_t(&Line_flag,6,9);

			if(Row_flag - Old_Row_flag == 1)//数据更改
			{
				Row_flag = Old_Row_flag;
				if(Line_flag > 6)
					yaw_set = yaw_set - (float)pow(10,9 - Line_flag);
				else
					yaw_set = -yaw_set;
				Clamp_float(&yaw_set,-180,180);
				Turn_num_new();
			}
			else if(Row_flag - Old_Row_flag == -1)
			{
				Row_flag = Old_Row_flag;
				if(Line_flag > 6)
					yaw_set = yaw_set + (float)pow(10,9 - Line_flag);
				else
					yaw_set = -yaw_set;
				Clamp_float(&yaw_set,-180,180);
				Turn_num_new();
			}
		}
		else
		{
			xSemaphoreTake(do_flag_useflag,portMAX_DELAY);
			do_flag = car_ATurn;
			xSemaphoreGive(do_flag_useflag);
			
			Depth_list[Depth_flag-2] = 0;
			Depth_flag--;
		}
		Clamp_uint8_t(&Depth_flag,1,4);
		Page_flag = 1;	//页码恒为1
	}
	
	/***********************************************************************************************************/
	if(Old_Depth_flag == 3	&&	Depth_flag == 4	&&	Depth_list[0]	== 3 && Depth_list[1] == 4)//相对角度修改数据初始化列标
		Line_flag = 6;
	else if(Depth_flag > 3	&&	Depth_list[0]	== 3 && Depth_list[1] == 4)	//修改数据及确认
	{
		if(Old_Row_flag > 1)//修改数据类
		{
			cursor_flag = true;//切换光标
			if(Page_flag	-	Old_Page_flag == 1)
			{
				new_cursor_back();
				Line_flag++;
			}
			else if(Page_flag	-	Old_Page_flag == -1)
			{
				new_cursor_back();
				Line_flag--;
			}
			
			Clamp_uint8_t(&Line_flag,6,9);

			if(Row_flag - Old_Row_flag == 1)//数据更改
			{
				Row_flag = Old_Row_flag;
				if(Line_flag > 6)
					yaw_set = yaw_set - (float)pow(10,9 - Line_flag);
				else
					yaw_set = -yaw_set;
				Clamp_float(&yaw_set,-180,180);
				Turn_num_new();
			}
			else if(Row_flag - Old_Row_flag == -1)
			{
				Row_flag = Old_Row_flag;
				if(Line_flag > 6)
					yaw_set = yaw_set + (float)pow(10,9 - Line_flag);
				else
					yaw_set = -yaw_set;
				Clamp_float(&yaw_set,-180,180);
				Turn_num_new();
			}
		}
		else
		{
			xSemaphoreTake(do_flag_useflag,portMAX_DELAY);
			do_flag = car_RTurn;
			xSemaphoreGive(do_flag_useflag);
			
			Depth_list[Depth_flag-2] = 0;
			Depth_flag--;
		}
		Clamp_uint8_t(&Depth_flag,1,4);
		Page_flag = 1;	//页码恒为1
	}
	/***********************************************************************************************************/
	if(Old_Depth_flag == 3	&&	Depth_flag == 4	&&	Depth_list[0]	== 3 && Depth_list[1] == 5)//车门控制	初始化列标
		Line_flag = 6;
	else if(Depth_flag >= 3	&&	Depth_list[0]	== 3 && Depth_list[1] == 5)	//修改数据及确认
	{
		if(Page_flag	-	Old_Page_flag == 1)
			{
				door_state = false;
				OLED_ShowString(2,7,"Close");
				do_flag = door_set;
				Page_flag = Old_Page_flag;

			}
			else if(Page_flag	-	Old_Page_flag == -1)
			{
				door_state = true;
				OLED_ShowString(2,7,"Open ");
				do_flag = door_set;
				Page_flag = Old_Page_flag;
			}
			if(Row_flag - Old_Row_flag == 1)//数据更改
			{
				door_state = false;
				OLED_ShowString(2,7,"Close");
				do_flag = door_set;
				Row_flag = Old_Row_flag;
			}
			else if(Row_flag - Old_Row_flag == -1)
			{
				door_state = true;
				OLED_ShowString(2,7,"Open ");
				do_flag = door_set;
				Row_flag = Old_Row_flag;
			}
	/********************************************************************************/
		Clamp_uint8_t(&Row_flag,2,2);
		Clamp_uint8_t(&Depth_flag,1,3);
		Depth_list[2] = 0;
		Page_flag = 1;	//页码恒为1
	}
	
	
	/***********************************************************************************************************/
	if(Old_Depth_flag == 3	&& Depth_flag == 2 &&	Depth_list[0]	== 3 && Old_Depth_list[1] == 6)//跳出函数执行
	{
			goto_flag =	true;
	}
	if(Old_Depth_flag == 3	&&	Depth_flag == 4	&&	Depth_list[0]	== 3 && Depth_list[1] == 6)//以特定角度避障	初始化列标
	{
			if(Row_flag < 3) Line_flag = 6;
			else Line_flag = 8;
	}
	else if(Depth_flag > 3	&&	Depth_list[0]	== 3 && Depth_list[1] == 6)	//修改数据及确认
	{
		if(Old_Row_flag > 1)//修改数据类
		{
			cursor_flag = true;//切换光标
			if(Page_flag	-	Old_Page_flag == 1)
			{
				new_cursor_back();
				Line_flag++;
			}
			else if(Page_flag	-	Old_Page_flag == -1)
			{
				new_cursor_back();
				Line_flag--;
			}
			
			if(Row_flag < 3) 	Clamp_uint8_t(&Line_flag,6,9);
			else 	Clamp_uint8_t(&Line_flag,8,13);

			if(Row_flag - Old_Row_flag == 1)//数据更改
			{
				Row_flag = Old_Row_flag;
				
				if(Row_flag < 3)
				{
					if(Line_flag > 6)
						yaw_set = yaw_set - (float)pow(10,9 - Line_flag);
					else
						yaw_set = -yaw_set;
					Clamp_float(&yaw_set,-180,180);					
				}
				else
				{
					if(Line_flag < 11)
						run[0] = run[0] -  (float)pow(10,10 - Line_flag);
					else if(Line_flag >11)
						run[0] = run[0] - (float)pow(10,11 - Line_flag);
					Clamp_float(&run[0],0,(float)999.99);
				}
				Turn_num_new();
			}
			else if(Row_flag - Old_Row_flag == -1)
			{
				Row_flag = Old_Row_flag;
				if(Row_flag < 3)
				{
					if(Line_flag > 6)
						yaw_set = yaw_set + (float)pow(10,9 - Line_flag);
					else
						yaw_set = -yaw_set;
					Clamp_float(&yaw_set,-180,180);
				}
				else
				{
					if(Line_flag < 11)
						run[0] = run[0] + (float)pow(10,10 - Line_flag);
					else if(Line_flag >11)
						run[0] = run[0] + (float)pow(10,11 - Line_flag);
					Clamp_float(&run[0],0,(float)999.99);

				}
				Turn_num_new();
			}
		}
		else
		{
			xSemaphoreTake(do_flag_useflag,portMAX_DELAY);
			do_flag = car_Coll;
			xSemaphoreGive(do_flag_useflag);
			
			Depth_list[Depth_flag-2] = 0;
			Depth_flag--;
		}
		Clamp_uint8_t(&Depth_flag,1,4);
		Page_flag = 1;	//页码恒为1
	}


/***********************************************************************************************************/
	if(Old_Depth_flag == 3	&& Depth_flag == 2 &&	Depth_list[0]	== 3 && Old_Depth_list[1] == 7)//跳出函数执行
	{
			goto_flag =	true;
	}
	if(Old_Depth_flag == 3	&&	Depth_flag == 4	&&	Depth_list[0]	== 3 && Depth_list[1] == 7)//前往对应坐标	修改数据初始化列标
	{
		if(Row_flag < 4)Line_flag = 10;
		else Line_flag = 8;
	}
	else if(Depth_flag > 3	&&	Depth_list[0]	== 3 && Depth_list[1] == 7)	//修改数据及确认
	{
		if(Old_Row_flag > 1)//修改数据类
		{
			cursor_flag = true;//切换光标
			if(Page_flag	-	Old_Page_flag == 1)
			{
				new_cursor_back();
				Line_flag++;
			}
			else if(Page_flag	-	Old_Page_flag == -1)
			{
				new_cursor_back();
				Line_flag--;
			}
			
			if(Row_flag < 3) 	Clamp_uint8_t(&Line_flag,10,13);
			else 	Clamp_uint8_t(&Line_flag,8,13);
			
			
			if(Row_flag - Old_Row_flag == 1)//数据更改
			{
				
				Row_flag = Old_Row_flag;
				if(Row_flag < 4)
				{
					xSemaphoreTake(go_useflag,portMAX_DELAY);
					if(Line_flag < 11)
						go[Row_flag-2] = go[Row_flag-2] - (float)pow(10,10 - Line_flag);
					else if(Line_flag >11)
						go[Row_flag-2] = go[Row_flag-2] - (float)pow(10,11 - Line_flag);
					Clamp_float(&go[0],0,(float)Cell_Site[3].x);
					Clamp_float(&go[1],0,(float)Cell_Site[3].y);
					
					xSemaphoreGive(go_useflag);
				}
				else
				{
					if(Line_flag < 11)
						run[0] = run[0] - (float)pow(10,10 - Line_flag);
					else if(Line_flag >11)
						run[0] = run[0] - (float)pow(10,11 - Line_flag);
					Clamp_float(&run[0],0,(float)999.99);
				}
				go_num_new();
				
			}
			else if(Row_flag - Old_Row_flag == -1)
			{
				Row_flag = Old_Row_flag;
				if(Row_flag < 4)
				{
					xSemaphoreTake(go_useflag,portMAX_DELAY);
					if(Line_flag < 11)
						go[Row_flag-2] = go[Row_flag-2] + (float)pow(10,10 - Line_flag);
					else if(Line_flag >11)
						go[Row_flag-2] = go[Row_flag-2] + (float)pow(10,11 - Line_flag);
					Clamp_float(&go[0],0,(float)Cell_Site[3].x);
					Clamp_float(&go[1],0,(float)Cell_Site[3].y);
					
					xSemaphoreGive(go_useflag);
				}
				else
				{
					if(Line_flag < 11)
						run[0] = run[0] +(float)pow(10,10 - Line_flag);
					else if(Line_flag >11)
						run[0] = run[0] + (float)pow(10,11 - Line_flag);
					Clamp_float(&run[0],0,(float)999.99);
				}
				go_num_new();

			}
		}
		else
		{
			xSemaphoreTake(do_flag_useflag,portMAX_DELAY);
			do_flag = car_goto;
			xSemaphoreGive(do_flag_useflag);
			
			Depth_list[Depth_flag-2] = 0;
			Depth_flag--;
		}
		Clamp_uint8_t(&Depth_flag,1,4);
		Page_flag = 1;	//页码恒为1
	}
/***********************************************************************************************************/
	if(Old_Depth_flag == 3	&& Depth_flag == 2 &&	Depth_list[0]	== 3 && Old_Depth_list[1] == 8)//跳出函数执行
	{
			goto_flag =	true;
	}
	if(Old_Depth_flag == 3	&&	Depth_flag == 4	&&	Depth_list[0]	== 3 && Depth_list[1] == 8)//前往对应坐标 加避障	修改数据初始化列标
	{
		if(Row_flag < 4)Line_flag = 10;
		else Line_flag = 8;
	}
	else if(Depth_flag > 3	&&	Depth_list[0]	== 3 && Depth_list[1] == 8)	//修改数据及确认
	{
		if(Old_Row_flag > 1)//修改数据类
		{
			cursor_flag = true;//切换光标
			if(Page_flag	-	Old_Page_flag == 1)
			{
				new_cursor_back();
				Line_flag++;
			}
			else if(Page_flag	-	Old_Page_flag == -1)
			{
				new_cursor_back();
				Line_flag--;
			}
			
			if(Row_flag < 3) 	Clamp_uint8_t(&Line_flag,10,13);
			else 	Clamp_uint8_t(&Line_flag,8,13);

			if(Row_flag - Old_Row_flag == 1)//数据更改
			{
				Row_flag = Old_Row_flag;
				if(Row_flag < 4)
				{
					xSemaphoreTake(go_useflag,portMAX_DELAY);
					if(Line_flag < 11)
						go[Row_flag-2] = go[Row_flag-2] - (float)pow(10,10 - Line_flag);
					else if(Line_flag >11)
						go[Row_flag-2] = go[Row_flag-2] - (float)pow(10,11 - Line_flag);
					Clamp_float(&go[0],0,(float)Cell_Site[3].x);
					Clamp_float(&go[1],0,(float)Cell_Site[3].y);
					
					xSemaphoreGive(go_useflag);
				}
				else
				{
					if(Line_flag < 11)
						run[0] = run[0] - (float)pow(10,10 - Line_flag);
					else if(Line_flag >11)
						run[0] = run[0] - (float)pow(10,11 - Line_flag);
					Clamp_float(&run[0],0,(float)999.99);
				}
				go_num_new();
				
			}
			else if(Row_flag - Old_Row_flag == -1)
			{
				Row_flag = Old_Row_flag;
				if(Row_flag < 4)
				{
					xSemaphoreTake(go_useflag,portMAX_DELAY);
					if(Line_flag < 11)
						go[Row_flag-2] = go[Row_flag-2] + (float)pow(10,10 - Line_flag);
					else if(Line_flag >11)
						go[Row_flag-2] = go[Row_flag-2] + (float)pow(10,11 - Line_flag);
					Clamp_float(&go[0],0,(float)Cell_Site[3].x);
					Clamp_float(&go[1],0,(float)Cell_Site[3].y);
					
					xSemaphoreGive(go_useflag);
				}
				else
				{
					if(Line_flag < 11)
						run[0] = run[0] + (float)pow(10,10 - Line_flag);
					else if(Line_flag >11)
						run[0] = run[0] + (float)pow(10,11 - Line_flag);
					Clamp_float(&run[0],0,(float)999.99);
				}
				go_num_new();

			}
		}
		else
		{
			xSemaphoreTake(do_flag_useflag,portMAX_DELAY);
			do_flag = car_goto_Coll;
			xSemaphoreGive(do_flag_useflag);
			
			Depth_list[Depth_flag-2] = 0;
			Depth_flag--;
		}
		Clamp_uint8_t(&Depth_flag,1,4);
		Page_flag = 1;	//页码恒为1
	}
/***********************************************************************************************************/
	if(Old_Depth_flag == 3	&&	Depth_flag == 4	&&	Depth_list[0]	== 4 && Depth_list[1] == 5)//设置基站坐标 初始化列标
		Line_flag = 6;
	else if(Depth_flag > 3	&&	Depth_list[0]	== 4 && Depth_list[1] == 5)	//修改数据及确认
	{
		if(Old_Row_flag > 1)//修改数据类
		{
			cursor_flag = true;//切换光标
			if(Page_flag	-	Old_Page_flag == 1)
			{
				Page_flag = Old_Page_flag;	//页码保持不变
				new_cursor_back();
				Line_flag++;
			}
			else if(Page_flag	-	Old_Page_flag == -1)
			{
				Page_flag = Old_Page_flag;	//页码保持不变
				new_cursor_back();
				Line_flag--;
			}
			
			Clamp_uint8_t(&Line_flag,6,10);

			if(Row_flag - Old_Row_flag == 1)//数据更改
			{
				Row_flag = Old_Row_flag;
				if(Row_flag == 2)
				{
					if(Line_flag == 6)
						Cell_Site[Page_flag - 1].x = -Cell_Site[Page_flag - 1].x;
					else if(Line_flag == 7)
						Cell_Site[Page_flag - 1].x = Cell_Site[Page_flag - 1].x - (float)pow(10,7 - Line_flag);
					else if(Line_flag >8)
						Cell_Site[Page_flag - 1].x = Cell_Site[Page_flag - 1].x - (float)pow(10,8 - Line_flag);
					Clamp_double(&Cell_Site[Page_flag - 1].x,0,(float)9.99);
				}
				if(Row_flag == 3)
				{
					if(Line_flag == 6)
						Cell_Site[Page_flag - 1].y = -Cell_Site[Page_flag - 1].y;
					else if(Line_flag == 7)
						Cell_Site[Page_flag - 1].y = Cell_Site[Page_flag - 1].y - (float)pow(10,7 - Line_flag);
					else if(Line_flag >8)
						Cell_Site[Page_flag - 1].y = Cell_Site[Page_flag - 1].y - (float)pow(10,8 - Line_flag);
					Clamp_double(&Cell_Site[Page_flag - 1].y,0,(float)9.99);
				}
				if(Row_flag == 4)
				{
					if(Line_flag == 6)
						Cell_Site[Page_flag - 1].z = -Cell_Site[Page_flag - 1].z;
					else if(Line_flag == 7)
						Cell_Site[Page_flag - 1].z = Cell_Site[Page_flag - 1].z - (float)pow(10,7 - Line_flag);
					else if(Line_flag >8)
						Cell_Site[Page_flag - 1].z = Cell_Site[Page_flag - 1].z - (float)pow(10,8 - Line_flag);
					Clamp_double(&Cell_Site[Page_flag - 1].z,0,(float)9.99);
				}
				set_cell_new();
			}
			else if(Row_flag - Old_Row_flag == -1)
			{
				Row_flag = Old_Row_flag;
				if(Row_flag == 2)
				{
					if(Line_flag == 6)
						Cell_Site[Page_flag - 1].x = -Cell_Site[Page_flag - 1].x;
					else if(Line_flag == 7)
						Cell_Site[Page_flag - 1].x = Cell_Site[Page_flag - 1].x + (float)pow(10,7 - Line_flag);
					else if(Line_flag >8)
						Cell_Site[Page_flag - 1].x = Cell_Site[Page_flag - 1].x + (float)pow(10,8 - Line_flag);
					Clamp_double(&Cell_Site[Page_flag - 1].x,0,(float)9.99);
				}
				if(Row_flag == 3)
				{
					if(Line_flag == 6)
						Cell_Site[Page_flag - 1].y = -Cell_Site[Page_flag - 1].y;
					else if(Line_flag == 7)
						Cell_Site[Page_flag - 1].y = Cell_Site[Page_flag - 1].y + (float)pow(10,7 - Line_flag);
					else if(Line_flag >8)
						Cell_Site[Page_flag - 1].y = Cell_Site[Page_flag - 1].y + (float)pow(10,8 - Line_flag);
					Clamp_double(&Cell_Site[Page_flag - 1].y,0,(float)9.99);
				}
				if(Row_flag == 4)
				{
					if(Line_flag == 6)
						Cell_Site[Page_flag - 1].z = -Cell_Site[Page_flag - 1].z;
					else if(Line_flag == 7)
						Cell_Site[Page_flag - 1].z = Cell_Site[Page_flag - 1].z + (float)pow(10,7 - Line_flag);
					else if(Line_flag >8)
						Cell_Site[Page_flag - 1].z = Cell_Site[Page_flag - 1].z + (float)pow(10,8 - Line_flag);
					Clamp_double(&Cell_Site[Page_flag - 1].z,0,(float)9.99);
				}
				set_cell_new();
			}
		}
		Clamp_uint8_t(&Depth_flag,1,4);
		Page_flag = Old_Page_flag;	//页码保持不变
	}
		if((Old_Depth_flag != Depth_flag||Old_Page_flag	!= Page_flag)&&	Old_Depth_flag + Depth_flag <	7)	//翻页,深度刷新
		new_flag = false;
	/******************************!!!!!!!!!!!!!!!!!!!!页面显示!!!!!!!!!!!!!!!!!!!!!!!************************************/

	if(!new_flag)			//刷新标志
	{
		if(Page_flag == 1	&&	Depth_flag == 1)	//用户首页	1
		{
			OLED_Clear();
			Row_flag = 3;
			OLED_ShowString(1,1,"Control Panel");
			OLED_ShowString(2,1,"Hello User");
			OLED_ShowString(3,1,"*Car Control");		//小车控制页
			OLED_ShowString(4,1,"*Detail Page");		//小车详情页
		}
		
			else if(Page_flag == 1	&&	Depth_flag == 2	&&	Depth_list[0]	== 3)	//车辆控制页
			{
				OLED_Clear();
				if(Row_flag != 5	&&	Row_flag != 4 && Row_flag != 7 && Row_flag != 8)	Row_flag = 2;
				OLED_ShowString(1,1,"Car Control");
				if(Row_flag < 5)
				{
					OLED_ShowString(2,1,"*Car Run");						//以特定速度角度跑距离
					OLED_ShowString(3,1,"*Car ATurn");					//绝对
					OLED_ShowString(4,1,"*Car RTurn");					//相对
				}
				else	if(Row_flag > 4 && Row_flag < 8	)
				{
					OLED_ShowString(2,1,"*Car Door");						//车门控制
					OLED_ShowString(3,1,"*car Coll");						//以特定角度直线避障
					OLED_ShowString(4,1,"*Car GoTo");						//前往对应坐标
				}
				else
				{
					OLED_ShowString(2,1,"*Car GoTo Coll");			//前往对应坐标 加避障
				}
			}
			
			else if(Page_flag == 1	&&	Depth_flag == 3	&&	Depth_list[0]	== 3 && Depth_list[1] == 2)	//以特定速度角度跑距离
			{
				OLED_Clear();
				Row_flag = 2;
				Line_flag = 8;
				OLED_ShowString(1,1,"Car Run   *OK");
				OLED_ShowString(2,1,"*Speed:");			//速度
				OLED_ShowString(3,1,"*Sum:");				//距离
				OLED_ShowString(4,1,"*yaw:");				//角度
				
				OLED_ShowNum(2,8,(uint32_t)fabsf(run[0])/100,1);
				OLED_ShowNum(2,9,(uint32_t)(fabsf(run[0])/10)%10,1);
				OLED_ShowNum(2,10,(uint32_t)fabsf(run[0])%10,1);
				OLED_ShowChar(2,11,'.');
				OLED_ShowNum(2,12,(uint32_t)(fabsf(run[0])*10)%10,1);
				OLED_ShowNum(2,13,(uint32_t)(fabsf(run[0])*100)%10,1);
				
				OLED_ShowNum(3,8,(uint32_t)fabsf(run[1])/100,1);
				OLED_ShowNum(3,9,(uint32_t)(fabsf(run[1])/10)%10,1);
				OLED_ShowNum(3,10,(uint32_t)fabsf(run[1])%10,1);
				OLED_ShowChar(3,11,'.');
				OLED_ShowNum(3,12,(uint32_t)(fabsf(run[1])*10)%10,1);
				OLED_ShowNum(3,13,(uint32_t)(fabsf(run[1])*100)%10,1);
			
				if(run[2] >= 0)	OLED_ShowChar(4,7,'+');
				else 					OLED_ShowChar(4,7,'-');
				
				OLED_ShowNum(4,8,(uint32_t)fabsf(run[2])/100,1);
				OLED_ShowNum(4,9,(uint32_t)(fabsf(run[2])/10)%10,1);
				OLED_ShowNum(4,10,(uint32_t)fabsf(run[2])%10,1);
				OLED_ShowChar(4,11,'.');
				OLED_ShowNum(4,12,(uint32_t)(fabsf(run[2])*10)%10,1);
				OLED_ShowNum(4,13,(uint32_t)(fabsf(run[2])*100)%10,1);
			}
			
			else if(Page_flag == 1	&&	Depth_flag == 3	&&	Depth_list[0]	== 3 && Depth_list[1] == 3)	//转绝对角度
			{
				OLED_Clear();
				Row_flag = 2;
				Line_flag = 6;
				OLED_ShowString(1,1,"Car ATurn   *OK");
				OLED_ShowString(2,1,"*yaw:");
				
				if(yaw_set >= 0)	OLED_ShowChar(2,6,'+');
				else 					OLED_ShowChar(2,6,'-');
				
				OLED_ShowNum(2,7,(uint32_t)fabsf(yaw_set/100),1);
				OLED_ShowNum(2,8,(uint32_t)(fabsf(yaw_set/10))%10,1);
				OLED_ShowNum(2,9,(uint32_t)fabsf(yaw_set)%10,1);
				
			}
			
			else if(Page_flag == 1	&&	Depth_flag == 3	&&	Depth_list[0]	== 3 && Depth_list[1] == 4)	//转相对角度
			{
				OLED_Clear();
				Row_flag = 2;
				Line_flag = 6;
				OLED_ShowString(1,1,"Car RTurn   *OK");
				
				OLED_ShowString(2,1,"*yaw:");
				if(yaw_set >= 0)	OLED_ShowChar(2,6,'+');
				else 					OLED_ShowChar(2,6,'-');
				
				OLED_ShowNum(2,7,(uint32_t)fabsf(yaw_set/100),1);
				OLED_ShowNum(2,8,(uint32_t)(fabsf(yaw_set/10))%10,1);
				OLED_ShowNum(2,9,(uint32_t)fabsf(yaw_set)%10,1);
			}
			else if(Page_flag == 1	&&	Depth_flag == 3	&&	Depth_list[0]	== 3 && Depth_list[1] == 5)	//车门
			{
				OLED_Clear();
				Row_flag = 2;
				Line_flag = 6;
				OLED_ShowString(1,1,"Car Door");
				OLED_ShowString(2,1,"state:");
				OLED_ShowString(3,1,"U/L  Open");
				OLED_ShowString(4,1,"D/R  Colse");
				
				if(door_state)
				{
					OLED_ShowString(2,7,"Open ");
				}
				else
				{
					OLED_ShowString(2,7,"Close");
				}
			}
			else if(Page_flag == 1	&&	Depth_flag == 3	&&	Depth_list[0]	== 3 && Depth_list[1] == 6)	//以特定角度避障
			{
				OLED_Clear();
				Row_flag = 2;
				Line_flag = 6;
				OLED_ShowString(1,1,"Car Coll   *OK");
				OLED_ShowString(2,1,"*yaw:");
				
				
				if(yaw_set >= 0)	OLED_ShowChar(2,6,'+');
				else 					OLED_ShowChar(2,6,'-');
				
				OLED_ShowNum(2,7,(uint32_t)fabsf(yaw_set/100),1);
				OLED_ShowNum(2,8,(uint32_t)(fabsf(yaw_set/10))%10,1);
				OLED_ShowNum(2,9,(uint32_t)fabsf(yaw_set)%10,1);
				
				OLED_ShowString(3,1,"*Speed:");			//速度
				OLED_ShowNum(3,8,(uint32_t)fabsf(run[0])/100,1);
				OLED_ShowNum(3,9,(uint32_t)(fabsf(run[0])/10)%10,1);
				OLED_ShowNum(3,10,(uint32_t)fabsf(run[0])%10,1);
				OLED_ShowChar(3,11,'.');
				OLED_ShowNum(3,12,(uint32_t)(fabsf(run[0])*10)%10,1);
				OLED_ShowNum(3,13,(uint32_t)(fabsf(run[0])*100)%10,1);

				
				
			}
			else if(Page_flag == 1	&&	Depth_flag == 3	&&	Depth_list[0]	== 3 && Depth_list[1] == 7)	//前往对应坐标
			{
				OLED_Clear();
				Row_flag = 2;
				Line_flag = 8;
				OLED_ShowString(1,1,"Car GoTo  *OK");						//前往对应坐标
				OLED_ShowString(2,1,"*X(m):");
				OLED_ShowString(3,1,"*Y(m):");
				
				OLED_ShowNum(2,10,(uint32_t)fabsf(go[0])%10,1);
				OLED_ShowChar(2,11,'.');
				OLED_ShowNum(2,12,(uint32_t)(fabsf(go[0])*10)%10,1);
				OLED_ShowNum(2,13,(uint32_t)(fabsf(go[0])*100)%10,1);
				
				OLED_ShowNum(3,10,(uint32_t)fabsf(go[1])%10,1);
				OLED_ShowChar(3,11,'.');
				OLED_ShowNum(3,12,(uint32_t)(fabsf(go[1])*10)%10,1);
				OLED_ShowNum(3,13,(uint32_t)(fabsf(go[1])*100)%10,1);
				
				OLED_ShowString(4,1,"*Speed:");			//速度
				OLED_ShowNum(4,8,(uint32_t)fabsf(run[0])/100,1);
				OLED_ShowNum(4,9,(uint32_t)(fabsf(run[0])/10)%10,1);
				OLED_ShowNum(4,10,(uint32_t)fabsf(run[0])%10,1);
				OLED_ShowChar(4,11,'.');
				OLED_ShowNum(4,12,(uint32_t)(fabsf(run[0])*10)%10,1);
				OLED_ShowNum(4,13,(uint32_t)(fabsf(run[0])*100)%10,1);
			}

			else if(Page_flag == 1	&&	Depth_flag == 3	&&	Depth_list[0]	== 3 && Depth_list[1] == 8)	//前往对应坐标 加避障
			{
				OLED_Clear();
				Row_flag = 2;
				Line_flag = 8;
				OLED_ShowString(1,1,"Car GoColl  *OK");						//前往对应坐标
				OLED_ShowString(2,1,"*X(m):");
				OLED_ShowString(3,1,"*Y(m):");
				
				OLED_ShowNum(2,10,(uint32_t)fabsf(go[0])%10,1);
				OLED_ShowChar(2,11,'.');
				OLED_ShowNum(2,12,(uint32_t)(fabsf(go[0])*10)%10,1);
				OLED_ShowNum(2,13,(uint32_t)(fabsf(go[0])*100)%10,1);
				
				OLED_ShowNum(3,10,(uint32_t)fabsf(go[1])%10,1);
				OLED_ShowChar(3,11,'.');
				OLED_ShowNum(3,12,(uint32_t)(fabsf(go[1])*10)%10,1);
				OLED_ShowNum(3,13,(uint32_t)(fabsf(go[1])*100)%10,1);
				
				OLED_ShowString(4,1,"*Speed:");			//速度
				OLED_ShowNum(4,8,(uint32_t)fabsf(run[0])/100,1);
				OLED_ShowNum(4,9,(uint32_t)(fabsf(run[0])/10)%10,1);
				OLED_ShowNum(4,10,(uint32_t)fabsf(run[0])%10,1);
				OLED_ShowChar(4,11,'.');
				OLED_ShowNum(4,12,(uint32_t)(fabsf(run[0])*10)%10,1);
				OLED_ShowNum(4,13,(uint32_t)(fabsf(run[0])*100)%10,1);

				
			}
			

			else if(Page_flag == 1	&&	Depth_flag == 2	&&	Depth_list[0]	== 4)	//小车详情页
			{
				OLED_Clear();
				if((Row_flag != 5	&&	Row_flag != 4) || (Old_Depth_flag == 1))	Row_flag = 2;
				OLED_ShowString(1,1,"Detail Page");
				if(Row_flag < 5)
				{
					OLED_ShowString(2,1,"*Show IMU");						//显示惯导数据
					OLED_ShowString(3,1,"*Show UWB");						//显示坐标位置
					OLED_ShowString(4,1,"*Show Ult");						//显示坐标位置
				}
				else
				{
					OLED_ShowString(2,1,"*Set Cell Site");						//设置基站坐标
				}
			}
			
			else if(Page_flag == 1	&&	Depth_flag == 3	&&	Depth_list[0]	== 4 && Depth_list[1] == 2)	//显示惯导
			{
				OLED_Clear();
				Row_flag = 1;
				Line_flag = 1;
				OLED_ShowString(1,1,"Show IMU");						
				OLED_ShowString(2,1,"Pitch:");
				OLED_ShowString(3,1,"Roll:");
				OLED_ShowString(4,1,"yaw:");
				
				xSemaphoreTake(ol_yaw_useflag,portMAX_DELAY);
				xSemaphoreTake(ol_first_useflag,portMAX_DELAY);
				for(i = 0;i < 3;i++)
				{
					ol_temp[i] = ol_yaw[i] - ol_first[i];
				}
				xSemaphoreGive(ol_first_useflag);
				xSemaphoreGive(ol_yaw_useflag);	
				
				if(ol_yaw[0] - ol_first[0] >= 0)	OLED_ShowChar(2,7,'+');
				else 															OLED_ShowChar(2,7,'-');

				OLED_ShowNum(2,8,(uint32_t)fabs(ol_temp[0])/100,1);
				OLED_ShowNum(2,9,(uint32_t)(fabs(ol_temp[0])/10)%10,1);
				OLED_ShowNum(2,10,(uint32_t)fabs(ol_temp[0])%10,1);
				OLED_ShowChar(2,11,'.');
				OLED_ShowNum(2,12,(uint32_t)(fabs(ol_temp[0])*10)%10,1);

				
				
				if(ol_yaw[1] - ol_first[1] >= 0)	OLED_ShowChar(3,7,'+');
				else 															OLED_ShowChar(3,7,'-');
				OLED_ShowNum(3,8,(uint32_t)fabs(ol_temp[1])/100,1);
				OLED_ShowNum(3,9,(uint32_t)(fabs(ol_temp[1])/10)%10,1);
				OLED_ShowNum(3,10,(uint32_t)fabs(ol_temp[1])%10,1);
				OLED_ShowChar(3,11,'.');
				OLED_ShowNum(3,12,(uint32_t)(fabs(ol_temp[1])*10)%10,1);

				
				if(ol_yaw[2] - ol_first[2] >= 0)	OLED_ShowChar(4,7,'+');
				else 															OLED_ShowChar(4,7,'-');

				OLED_ShowNum(4,8,(uint32_t)fabs(ol_temp[2])/100,1);
				OLED_ShowNum(4,9,(uint32_t)(fabs(ol_temp[2])/10)%10,1);
				OLED_ShowNum(4,10,(uint32_t)fabs(ol_temp[2])%10,1);
				OLED_ShowChar(4,11,'.');
				OLED_ShowNum(4,12,(uint32_t)(fabs(ol_temp[2])*10)%10,1);

				for(i = 0;i < 3;i++)
				{
					old_ol_temp[i] = ol_temp[i];
				}
			}
			
			else if(Page_flag == 1	&&	Depth_flag == 3	&&	Depth_list[0]	== 4 && Depth_list[1] == 3)	//显示坐标
			{
				OLED_Clear();
				Row_flag = 1;
				Line_flag = 1;
				OLED_ShowString(1,1,"Show IMU");						//显示坐标
				OLED_ShowString(2,1,"X(m):");
				OLED_ShowString(3,1,"Y(m):");
				if(report_temp.x < 0)
					OLED_ShowChar(2,6,'-');
				else
					OLED_ShowChar(2,6,'+');
					
				if(report_temp.y < 0)
					OLED_ShowChar(3,6,'-');
				else
					OLED_ShowChar(3,6,'+');


				OLED_ShowChar(2,8,'.');
				OLED_ShowChar(3,8,'.');
				for(i = 0; i < 3; i++)
				{	
					if(i < 2)
					{
							OLED_ShowNum(2,(uint8_t)(10 - i),(uint32_t)fabs(report_temp.x)/(uint32_t)pow(10,i)%10,1);
							OLED_ShowNum(3,(uint8_t)(10 - i),(uint32_t)fabs(report_temp.y)/(uint32_t)pow(10,i)%10,1);
					}
					else
					{
							OLED_ShowNum(2,7,(uint32_t)fabs(report_temp.x)/(uint32_t)pow(10,i)%10,1);
							OLED_ShowNum(3,7,(uint32_t)fabs(report_temp.y)/(uint32_t)pow(10,i)%10,1);
					}
				}
				
				
			}
			else if(Page_flag == 1	&&	Depth_flag == 3	&&	Depth_list[0]	== 4 && Depth_list[1] == 4)	//显示超声波
			{
				OLED_Clear();
				Row_flag = 1;
				Line_flag = 1;
				OLED_ShowString(1,1,"Show Ult");
				OLED_ShowString(2,1,"distance:");
				distance = (int)ult_ADC_Read(ult_1_ADC);
				
				OLED_ShowNum(2,10,(uint32_t)distance,3);
				OLED_ShowString(2,13,"cm.");
			}
			else if(Depth_flag == 3	&&	Depth_list[0]	== 4 && Depth_list[1] == 5)	//基站坐标设置
			{
				OLED_Clear();
				Row_flag = 2;
				Line_flag = 1;
				OLED_ShowString(1,1,"Set Cell Site A");
				OLED_ShowNum(1,16,Page_flag - 1,1);
				OLED_ShowString(2,1,"X(m):");
				OLED_ShowString(3,1,"Y(m):");
				OLED_ShowString(4,1,"Z(m):");
				
				OLED_ShowChar(2,8,'.');
				OLED_ShowChar(3,8,'.');
				OLED_ShowChar(4,8,'.');
				
				if(Cell_Site[Page_flag - 1].x < 0)
					OLED_ShowChar(2,6,'-');
				else
					OLED_ShowChar(2,6,'+');
					
				if(Cell_Site[Page_flag - 1].y < 0)
					OLED_ShowChar(3,6,'-');
				else
					OLED_ShowChar(3,6,'+');	
				
				if(Cell_Site[Page_flag - 1].z < 0)
					OLED_ShowChar(4,6,'-');
				else
					OLED_ShowChar(4,6,'+');	
				
				for(i = 0; i < 3; i++)
				{	
					if(i < 2)
					{
							OLED_ShowNum(2,(uint8_t)(10 - i),(uint32_t)fabs(Cell_Site[Page_flag - 1].x*100)/(uint32_t)pow(10,i)%10,1);
							OLED_ShowNum(3,(uint8_t)(10 - i),(uint32_t)fabs(Cell_Site[Page_flag - 1].y*100)/(uint32_t)pow(10,i)%10,1);
							OLED_ShowNum(4,(uint8_t)(10 - i),(uint32_t)fabs(Cell_Site[Page_flag - 1].z*100)/(uint32_t)pow(10,i)%10,1);
					}
					else
					{
							OLED_ShowNum(2,7,(uint32_t)fabs(Cell_Site[Page_flag - 1].x*100)/(uint32_t)pow(10,i)%10,1);
							OLED_ShowNum(3,7,(uint32_t)fabs(Cell_Site[Page_flag - 1].y*100)/(uint32_t)pow(10,i)%10,1);
							OLED_ShowNum(4,7,(uint32_t)fabs(Cell_Site[Page_flag - 1].z*100)/(uint32_t)pow(10,i)%10,1);
					}
				}
			}
			
			new_flag = true;		//增量刷新成功
	}
	if(delay_flag <= 0)		//光标闪烁
	{

		delay_flag = 500;
		if(!cursor_flag)		//行选光标
		{
			if(cursor_show == true)
			{
				OLED_ShowFT(((Row_flag-2)%3)+2,16,cursor);
				cursor_show = !cursor_show;
			}
			else
			{
				OLED_ShowString(((Row_flag-2)%3)+2,16," ");
				cursor_show = !cursor_show;
			}
		}
		else								//列选光标
		{
			OLED_ShowFT(((Row_flag-2)%3)+2,16,cursor);
			if(cursor_show == true)
			{
				OLED_ShowChar(((Row_flag-2)%3)+2,Line_flag,'_');
				cursor_show = !cursor_show;
			}
			else
			{
				new_cursor_back();
				cursor_show = !cursor_show;
			}
		}
	}
	
	Old_Depth_flag = Depth_flag;//数据刷新
	Old_Row_flag = Row_flag;
	Old_Page_flag = Page_flag;
	Old_Line_flag = Line_flag;
	for(i = 0;i < 5;i++)
	{
		Old_Depth_list[i] = Depth_list[i];
	
	}
}
	

void Clamp_uint8_t(uint8_t *temp,uint8_t L, uint8_t H)
{
	if(*temp < L) *temp = L;
	else if(*temp > H) *temp = H;
}

void Clamp_float(float *temp,float L, float H)
{
	if(*temp < L) *temp = L;
	else if(*temp > H) *temp = H;
}

void Clamp_double(double *temp,double L, double H)
{
	if(*temp < L) *temp = L;
	else if(*temp > H) *temp = H;
}


void new_cursor_back(void)
{
	if(Old_Depth_flag == 4	&&	Depth_list[0]	== 3 && Depth_list[1] == 2)		//以特定角度跑光标显示
	{
		if(Line_flag == 7)
		{
			if(run[2] >= 0)	OLED_ShowChar(4,7,'+');
			else 						OLED_ShowChar(4,7,'-');
		}
		else if(Line_flag < 11	&&	Line_flag >	7)
			OLED_ShowNum(((Row_flag-2)%3)+2,Line_flag,(uint32_t)(fabsf(run[Row_flag-2])*100)/(uint32_t)pow(10,12	-	Line_flag)%10,1);
		else if(Line_flag > 11)
			OLED_ShowNum(((Row_flag-2)%3)+2,Line_flag,(uint32_t)(fabsf(run[Row_flag-2])*100)/(uint32_t)pow(10,13	-	Line_flag)%10,1);
		else
			OLED_ShowChar(((Row_flag-2)%3)+2,11,'.');
	}
	else if(Old_Depth_flag == 4	&&	Depth_list[0]	== 3 && Depth_list[1] == 3)		//绝对角度
	{
		if(Line_flag == 6)
		{
			if(yaw_set >= 0)	OLED_ShowChar(2,6,'+');
			else 					OLED_ShowChar(2,6,'-');
		}
		else if(Line_flag < 11	&&	Line_flag >	6)
			OLED_ShowNum(((Row_flag-2)%3)+2,Line_flag,(uint32_t)((uint32_t)fabsf(yaw_set)/(uint32_t)pow(10,9	-	Line_flag))%10,1);
	}
	
	else if(Old_Depth_flag == 4	&&	Depth_list[0]	== 3 && Depth_list[1] == 4)//相对角度
	{
		if(Line_flag == 6)
		{
			if(yaw_set >= 0)	OLED_ShowChar(2,6,'+');
			else 					OLED_ShowChar(2,6,'-');
		}
		else if(Line_flag < 11	&&	Line_flag >	6)
			OLED_ShowNum(((Row_flag-2)%3)+2,Line_flag,(uint32_t)((uint32_t)fabsf(yaw_set)/(uint32_t)pow(10,9	-	Line_flag))%10,1);
	}
	else if(Old_Depth_flag == 4	&&	Depth_list[0]	== 3 && Depth_list[1] == 6)//Coll角度
	{
		if(Row_flag < 3)
		{
			if(Line_flag == 6)
			{
				if(yaw_set >= 0)	OLED_ShowChar(2,6,'+');
				else 					OLED_ShowChar(2,6,'-');
			}
			else if(Line_flag < 11	&&	Line_flag >	6)
				OLED_ShowNum(((Row_flag-2)%3)+2,Line_flag,(uint32_t)((uint32_t)fabsf(yaw_set)/(uint32_t)pow(10,9	-	Line_flag))%10,1);
		
		}
		else
		{
			if(Line_flag < 11	&&	Line_flag >	7)
				OLED_ShowNum(((Row_flag-2)%3)+2,Line_flag,(uint32_t)(fabsf(run[0])*100)/(uint32_t)pow(10,12	-	Line_flag)%10,1);
			else if(Line_flag > 11)
				OLED_ShowNum(((Row_flag-2)%3)+2,Line_flag,(uint32_t)(fabsf(run[0])*100)/(uint32_t)pow(10,13	-	Line_flag)%10,1);
			else
				OLED_ShowChar(((Row_flag-2)%3)+2,11,'.');
		}
		
		
	}
	else if(Old_Depth_flag == 4	&&	Depth_list[0]	== 3 && Depth_list[1] == 7)//前往对应坐标
	{
		if(Row_flag < 4)
		{
			if(Line_flag == 10)
			{
				OLED_ShowNum(((Row_flag-2)%3)+2,Line_flag,(uint32_t)((uint32_t)fabsf(go[Row_flag -2]*100)/(uint32_t)pow(10,12	-	Line_flag))%10,1);
			}
			else if(Line_flag > 11)
				OLED_ShowNum(((Row_flag-2)%3)+2,Line_flag,(uint32_t)((uint32_t)fabsf(go[Row_flag -2]*100)/(uint32_t)pow(10,13	-	Line_flag))%10,1);
			else
				OLED_ShowChar(((Row_flag-2)%3)+2,11,'.');
		}
		else
		{
			if(Line_flag < 11	&&	Line_flag >	7)
				OLED_ShowNum(((Row_flag-2)%3)+2,Line_flag,(uint32_t)(fabsf(run[0])*100)/(uint32_t)pow(10,12	-	Line_flag)%10,1);
			else if(Line_flag > 11)
				OLED_ShowNum(((Row_flag-2)%3)+2,Line_flag,(uint32_t)(fabsf(run[0])*100)/(uint32_t)pow(10,13	-	Line_flag)%10,1);
			else
				OLED_ShowChar(((Row_flag-2)%3)+2,11,'.');
		}
		
		
	}
	else if(Old_Depth_flag == 4	&&	Depth_list[0]	== 3 && Depth_list[1] == 8)//前往对应坐标
	{
		if(Row_flag < 4)
		{
			if(Line_flag == 10)
			{
				OLED_ShowNum(((Row_flag-2)%3)+2,Line_flag,(uint32_t)((uint32_t)fabsf(go[Row_flag -2]*100)/(uint32_t)pow(10,12	-	Line_flag))%10,1);
			}
			else if(Line_flag > 11)
				OLED_ShowNum(((Row_flag-2)%3)+2,Line_flag,(uint32_t)((uint32_t)fabsf(go[Row_flag -2]*100)/(uint32_t)pow(10,13	-	Line_flag))%10,1);
			else
				OLED_ShowChar(((Row_flag-2)%3)+2,11,'.');
		}
		else
		{
			if(Line_flag < 11	&&	Line_flag >	7)
				OLED_ShowNum(((Row_flag-2)%3)+2,Line_flag,(uint32_t)(fabsf(run[0])*100)/(uint32_t)pow(10,12	-	Line_flag)%10,1);
			else if(Line_flag > 11)
				OLED_ShowNum(((Row_flag-2)%3)+2,Line_flag,(uint32_t)(fabsf(run[0])*100)/(uint32_t)pow(10,13	-	Line_flag)%10,1);
			else
				OLED_ShowChar(((Row_flag-2)%3)+2,11,'.');
		}
	}
	else if(Old_Depth_flag == 4	&&	Depth_list[0]	== 4 && Depth_list[1] == 5)//修改基站坐标
	{
		if(Row_flag == 2)
		{
			if(Line_flag == 6)
			{
				if(Cell_Site[Page_flag - 1].x >= 0)	OLED_ShowChar(2,6,'+');
				else 					OLED_ShowChar(2,6,'-');
			}
			else if(Line_flag == 7)
			{
				OLED_ShowNum(((Row_flag-2)%3)+2,Line_flag,(uint32_t)((uint32_t)fabs(Cell_Site[Page_flag - 1].x*100)/(uint32_t)pow(10,9	-	Line_flag))%10,1);
			}
			else if(Line_flag > 8)
				OLED_ShowNum(((Row_flag-2)%3)+2,Line_flag,(uint32_t)((uint32_t)fabs(Cell_Site[Page_flag - 1].x*100)/(uint32_t)pow(10,10	-	Line_flag))%10,1);
			else
				OLED_ShowChar(((Row_flag-2)%3)+2,8,'.');
		}
		if(Row_flag == 3)
		{
			if(Line_flag == 6)
			{
				if(Cell_Site[Page_flag - 1].y >= 0)	OLED_ShowChar(3,6,'+');
				else 					OLED_ShowChar(3,6,'-');
			}
			else if(Line_flag == 7)
			{
				OLED_ShowNum(((Row_flag-2)%3)+2,Line_flag,(uint32_t)((uint32_t)fabs(Cell_Site[Page_flag - 1].y*100)/(uint32_t)pow(10,9	-	Line_flag))%10,1);
			}
			else if(Line_flag > 8)
				OLED_ShowNum(((Row_flag-2)%3)+2,Line_flag,(uint32_t)((uint32_t)fabs(Cell_Site[Page_flag - 1].y*100)/(uint32_t)pow(10,10	-	Line_flag))%10,1);
			else
				OLED_ShowChar(((Row_flag-2)%3)+2,8,'.');
		}		
		if(Row_flag == 4)
		{
			if(Line_flag == 6)
			{
				if(Cell_Site[Page_flag - 1].z >= 0)	OLED_ShowChar(4,6,'+');
				else 					OLED_ShowChar(4,6,'-');
			}
			else if(Line_flag == 7)
			{
				OLED_ShowNum(((Row_flag-2)%3)+2,Line_flag,(uint32_t)((uint32_t)fabs(Cell_Site[Page_flag - 1].z*100)/(uint32_t)pow(10,9	-	Line_flag))%10,1);
			}
			else if(Line_flag > 8)
				OLED_ShowNum(((Row_flag-2)%3)+2,Line_flag,(uint32_t)((uint32_t)fabs(Cell_Site[Page_flag - 1].z*100)/(uint32_t)pow(10,10	-	Line_flag))%10,1);
			else
				OLED_ShowChar(((Row_flag-2)%3)+2,8,'.');
		}
}		
}

void Turn_num_new(void)
{
	switch(Row_flag)
	{
		case 2:
				if(yaw_set >= 0)	OLED_ShowChar(2,6,'+');
				else 					OLED_ShowChar(2,6,'-');
				
				OLED_ShowNum(2,7,(uint32_t)fabsf(yaw_set/100),1);
				OLED_ShowNum(2,8,(uint32_t)(fabsf(yaw_set/10))%10,1);
				OLED_ShowNum(2,9,(uint32_t)fabsf(yaw_set)%10,1);
				break;
		case 3:
				OLED_ShowNum(3,8,(uint32_t)fabsf(run[0])/100,1);
				OLED_ShowNum(3,9,(uint32_t)(fabsf(run[0])/10)%10,1);
				OLED_ShowNum(3,10,(uint32_t)fabsf(run[0])%10,1);
				OLED_ShowChar(3,11,'.');
				OLED_ShowNum(3,12,(uint32_t)(fabsf(run[0])*10)%10,1);
				OLED_ShowNum(3,13,(uint32_t)(fabsf(run[0])*100)%10,1);
		break;		
	}
}
void run_num_new(void)
{
	
	switch(Row_flag)
	{
		case 2:
				OLED_ShowNum(2,8,(uint32_t)fabsf(run[0])/100,1);
				OLED_ShowNum(2,9,(uint32_t)(fabsf(run[0])/10)%10,1);
				OLED_ShowNum(2,10,(uint32_t)fabsf(run[0])%10,1);
				OLED_ShowChar(2,11,'.');
				OLED_ShowNum(2,12,(uint32_t)(fabsf(run[0])*10)%10,1);
				OLED_ShowNum(2,13,(uint32_t)(fabsf(run[0])*100)%10,1);
				break;
		
		case 3:
				OLED_ShowNum(3,8,(uint32_t)fabsf(run[1])/100,1);
				OLED_ShowNum(3,9,(uint32_t)(fabsf(run[1])/10)%10,1);
				OLED_ShowNum(3,10,(uint32_t)fabsf(run[1])%10,1);
				OLED_ShowChar(3,11,'.');
				OLED_ShowNum(3,12,(uint32_t)(fabsf(run[1])*10)%10,1);
				OLED_ShowNum(3,13,(uint32_t)(fabsf(run[1])*100)%10,1);
				break;
		
		case 4:
				if(run[2] >= 0)	OLED_ShowChar(4,7,'+');
				else 					OLED_ShowChar(4,7,'-');
				
				OLED_ShowNum(4,8,(uint32_t)fabsf(run[2])/100,1);
				OLED_ShowNum(4,9,(uint32_t)(fabsf(run[2])/10)%10,1);
				OLED_ShowNum(4,10,(uint32_t)fabsf(run[2])%10,1);
				OLED_ShowChar(4,11,'.');
				OLED_ShowNum(4,12,(uint32_t)(fabsf(run[2])*10)%10,1);
				OLED_ShowNum(4,13,(uint32_t)(fabsf(run[2])*100)%10,1);
				break;
	}
}
				

void go_num_new(void)
{
	
	switch(Row_flag)
	{
		case 2:
				OLED_ShowNum(2,10,(uint32_t)fabsf(go[0])%10,1);
				OLED_ShowChar(2,11,'.');
				OLED_ShowNum(2,12,(uint32_t)(fabsf(go[0])*10)%10,1);
				OLED_ShowNum(2,13,(uint32_t)(fabsf(go[0])*100)%10,1);
				break;
		
		case 3:
				OLED_ShowNum(3,10,(uint32_t)fabsf(go[1])%10,1);
				OLED_ShowChar(3,11,'.');
				OLED_ShowNum(3,12,(uint32_t)(fabsf(go[1])*10)%10,1);
				OLED_ShowNum(3,13,(uint32_t)(fabsf(go[1])*100)%10,1);
				break;
		
		case 4:
				OLED_ShowNum(4,8,(uint32_t)fabsf(run[0])/100,1);
				OLED_ShowNum(4,9,(uint32_t)(fabsf(run[0])/10)%10,1);
				OLED_ShowNum(4,10,(uint32_t)fabsf(run[0])%10,1);
				OLED_ShowChar(4,11,'.');
				OLED_ShowNum(4,12,(uint32_t)(fabsf(run[0])*10)%10,1);
				OLED_ShowNum(4,13,(uint32_t)(fabsf(run[0])*100)%10,1);
				break;
	}
}

void IMU_num_new(void)
{
	uint8_t i,j;
	IMU_Read();
	xSemaphoreTake(ol_yaw_useflag,portMAX_DELAY);
	xSemaphoreTake(ol_first_useflag,portMAX_DELAY);

	for(i = 0;i < 3;i++)
	{
		ol_temp[i] = ol_yaw[i] - ol_first[i];
		if(ol_temp[i] > 180)
		{
			ol_temp[i] = ol_temp[i] -360;
		}
		else if(ol_temp[i] < -180)
		{
			ol_temp[i] = ol_temp[i] +360;
		}
	}
	xSemaphoreGive(ol_yaw_useflag);
	xSemaphoreGive(ol_first_useflag);
	
	for(j = 0;j < 3;j++)
	{
		for(i = 0;i < 5;i++)
		{
			if(i == 0	&&	old_ol_temp[j]	*	ol_temp[j] < 0)
			{
				if(ol_temp[j] >= 0)	OLED_ShowChar(2 + j,7+i,'+');
				else 								OLED_ShowChar(2 + j,7+i,'-');
			}
			else if(i < 4	&&	(uint32_t)(fabs(old_ol_temp[j])*100)/(uint32_t)pow(10,5 - i)%10 != (uint32_t)(fabs(ol_temp[j])*100)/(uint32_t)pow(10,5 - i)%10 )
			{
				OLED_ShowNum(2+j,7+i,(uint32_t)(fabs(ol_temp[j])*100)/(uint32_t)pow(10,5 - i)%10,1);
			}
			else if(i > 3	&&	((uint32_t)(fabs(old_ol_temp[j])*100)/(uint32_t)pow(10,5 - i)%10 != (uint32_t)(fabs(ol_temp[j])*100)/(uint32_t)pow(10,5 - i)%10 ))
			{
				OLED_ShowNum(2+j,8+i,(uint32_t)(fabs(ol_temp[j])*100)/(uint32_t)pow(10,5 - i)%10,1);
			}
		}
		old_ol_temp[j] = ol_temp[j];
	}
}


bool UWB_num_flag = false;

void UWB_num_new(void)
{
	int i,temp;
	temp = tril_read();
	xSemaphoreTake(report_useflag,portMAX_DELAY);
	report_temp = report;
	xSemaphoreGive(report_useflag);
	report_temp.x = report_temp.x*100;
	report_temp.y = report_temp.y*100;
	report_temp.z = report_temp.z*100;
	
	if(temp == -1 ||((int)report_temp.x == 0 && (int)report_temp.y == 0))
	{
		OLED_ShowString(2,6,"ERROR");
		OLED_ShowString(3,6,"ERROR");
		old_report_temp.x = 0;
		old_report_temp.y = 0;
		old_report_temp.z = 0;
		
		report_temp.x = 0;
		report_temp.y = 0;
		report_temp.z = 0;
		
		UWB_num_flag = false;
	}
	else
	{
		if(UWB_num_flag == false)
		{
			UWB_num_flag = true;
			if(report_temp.x < 0)
				OLED_ShowChar(2,6,'-');
			else
				OLED_ShowChar(2,6,'+');
				
			if(report_temp.y < 0)
				OLED_ShowChar(3,6,'-');
			else
				OLED_ShowChar(3,6,'+');


			OLED_ShowChar(2,8,'.');
			OLED_ShowChar(3,8,'.');
			for(i = 0; i < 3; i++)
			{	
				if(i < 2)
				{
						OLED_ShowNum(2,(uint8_t)(10 - i),(uint32_t)fabs(report_temp.x)/(uint32_t)pow(10,i)%10,1);
						OLED_ShowNum(3,(uint8_t)(10 - i),(uint32_t)fabs(report_temp.y)/(uint32_t)pow(10,i)%10,1);
				}
				else
				{
						OLED_ShowNum(2,7,(uint32_t)fabs(report_temp.x)/(uint32_t)pow(10,i)%10,1);
						OLED_ShowNum(3,7,(uint32_t)fabs(report_temp.y)/(uint32_t)pow(10,i)%10,1);
				}
			}
		}
		
		if(report_temp.x*old_report_temp.x < 0)
		{
			if(report_temp.x < 0)
				OLED_ShowChar(2,6,'-');
			else
				OLED_ShowChar(2,6,'+');
		}
		if(report_temp.y*old_report_temp.y < 0)
		{
			if(report_temp.y < 0)
				OLED_ShowChar(3,6,'-');
			else
				OLED_ShowChar(3,6,'+');
		}
		for(i = 0;i < 3;i++)
		{
			if(i < 2)
			{
				if((int)fabs(report_temp.x)/(int)pow(10,i)%10 != (int)fabs(old_report_temp.x)/(int)pow(10,i)%10)
					OLED_ShowNum(2,(uint8_t)(10 - i),(uint32_t)fabs(report_temp.x)/(uint32_t)pow(10,i)%10,1);
				
				if((int)fabs(report_temp.y)/(int)pow(10,i)%10 != (int)fabs(old_report_temp.y)/(int)pow(10,i)%10)
					OLED_ShowNum(3,(uint8_t)(10 - i),(uint32_t)fabs(report_temp.y)/(uint32_t)pow(10,i)%10,1);
			}
			else
			{
				if((int)fabs(report_temp.x)/(int)pow(10,i)%10 != (int)fabs(old_report_temp.x)/(int)pow(10,i)%10)
					OLED_ShowNum(2,7,(uint32_t)fabs(report_temp.x)/(uint32_t)pow(10,i)%10,1);
				
				if((int)fabs(report_temp.y)/(int)pow(10,i)%10 != (int)fabs(old_report_temp.y)/(int)pow(10,i)%10)
					OLED_ShowNum(3,7,(uint32_t)fabs(report_temp.y)/(uint32_t)pow(10,i)%10,1);
			}
		}
		old_report_temp = report_temp;
	}
	vTaskDelay(100);
}


void Ult_num_new(void)
{
	int i;
	distance = (int)ult_ADC_Read(ult_1_ADC);
	for(i = 0;i < 3;i++)
	{
		if(old_distance/(int)pow(10,i)%10 != distance/(int)pow(10,i)%10 )
			OLED_ShowNum(2,(uint8_t)(12-i),(uint32_t)(distance/(int)pow(10,i)%10),1);
	}
	old_distance = distance;
	vTaskDelay(100);
}

void set_cell_new(void)
{
	int i;
	switch(Row_flag)
	{
		case 2:
		OLED_ShowChar(2,8,'.');
		if(Cell_Site[Page_flag - 1].x < 0)
			OLED_ShowChar(2,6,'-');
		else
			OLED_ShowChar(2,6,'+');
		for(i = 0; i < 3; i++)
		{	
			if(i < 2)
			{
					OLED_ShowNum(2,(uint8_t)(10 - i),(uint32_t)fabs(Cell_Site[Page_flag - 1].x*100)/(uint32_t)pow(10,i)%10,1);
			}
			else
			{
					OLED_ShowNum(2,7,(uint32_t)fabs(Cell_Site[Page_flag - 1].x*100)/(uint32_t)pow(10,i)%10,1);
			}
		}
		break;
		
		case 3:
			OLED_ShowChar(3,8,'.');
			if(Cell_Site[Page_flag - 1].y < 0)
				OLED_ShowChar(3,6,'-');
			else
				OLED_ShowChar(3,6,'+');	
			for(i = 0; i < 3; i++)
			{	
				if(i < 2)
				{
						OLED_ShowNum(3,(uint8_t)(10 - i),(uint32_t)fabs(Cell_Site[Page_flag - 1].y*100)/(uint32_t)pow(10,i)%10,1);
				}
				else
				{
						OLED_ShowNum(3,7,(uint32_t)fabs(Cell_Site[Page_flag - 1].y*100)/(uint32_t)pow(10,i)%10,1);
				}
			}
		break;
				
		case 4:
			OLED_ShowChar(4,8,'.');
			if(Cell_Site[Page_flag - 1].z < 0)
				OLED_ShowChar(4,6,'-');
			else
				OLED_ShowChar(4,6,'+');
			for(i = 0; i < 3; i++)
			{	
				if(i < 2)
				{
						OLED_ShowNum(4,(uint8_t)(10 - i),(uint32_t)fabs(Cell_Site[Page_flag - 1].z*100)/(uint32_t)pow(10,i)%10,1);
				}
				else
				{
						OLED_ShowNum(4,7,(uint32_t)fabs(Cell_Site[Page_flag - 1].z*100)/(uint32_t)pow(10,i)%10,1);
				}
			}
		break;

	}
}



