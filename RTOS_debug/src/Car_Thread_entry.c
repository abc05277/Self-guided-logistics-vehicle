#include "Car_Thread.h"
#include "car.h"

                /* Car_Thread entry function */
                /* pvParameters contains TaskHandle_t */
                void Car_Thread_entry(void * pvParameters)
                {
                    FSP_PARAMETER_NOT_USED(pvParameters);
										debug_init();

										car_init(CarT1);
									/* TODO: add your own code here */
                    while(1)
                    {
											vTaskDelay(1);
											
											switch(do_flag)
											{
												case car_free:
													vTaskDelay(1);
													break;
												case car_run:
													{
														xSemaphoreTake(run_useflag,portMAX_DELAY);
														float speed_temp,yaw_temp,sum_temp;
														speed_temp = run[0];
														sum_temp = run[1];
														yaw_temp = run[2];
														
														
														xSemaphoreGive(run_useflag);
													
														car_sum(speed_temp,sum_temp,yaw_temp);
													}
													xSemaphoreTake(do_flag_useflag,portMAX_DELAY);
													do_flag = car_free;
													xSemaphoreGive(do_flag_useflag);
													Morse(0);
													break;
												case car_ATurn:
													car_ATurning(yaw_set);
													xSemaphoreTake(do_flag_useflag,portMAX_DELAY);
													do_flag = car_free;
													xSemaphoreGive(do_flag_useflag);
													Morse(0);
												break;
												case car_RTurn:
													car_RTurning(yaw_set);
													xSemaphoreTake(do_flag_useflag,portMAX_DELAY);
													do_flag = car_free;
													xSemaphoreGive(do_flag_useflag);
													Morse(0);
												break;
												case door_set:
													car_door_set(door_state);
													xSemaphoreTake(do_flag_useflag,portMAX_DELAY);
													do_flag = car_free;
													xSemaphoreGive(do_flag_useflag);
													Morse(0);
													break;
												case car_Coll:
													{
														xSemaphoreTake(run_useflag,portMAX_DELAY);
														float speed_temp,yaw_temp,sum_temp;
														speed_temp = run[0];
														sum_temp = run[1];
														yaw_temp = run[2];
														xSemaphoreGive(run_useflag);
														car_Collision(yaw_set,speed_temp);
													}
													
													xSemaphoreTake(do_flag_useflag,portMAX_DELAY);
													do_flag = car_free;
													xSemaphoreGive(do_flag_useflag);
													Morse(0);
													break;
												case car_goto:
													
												
													{
														xSemaphoreTake(run_useflag,portMAX_DELAY);
														float speed_temp,yaw_temp,sum_temp;
														speed_temp = run[0];
														sum_temp = run[1];
														yaw_temp = run[2];
														xSemaphoreGive(run_useflag);
														
														car_goto_do(go[0],go[1],speed_temp);
													}
													xSemaphoreTake(do_flag_useflag,portMAX_DELAY);
													do_flag = car_free;
													xSemaphoreGive(do_flag_useflag);
													Morse(0);
													break;
												case car_goto_Coll:
													{
														xSemaphoreTake(run_useflag,portMAX_DELAY);
														float speed_temp,yaw_temp,sum_temp;
														speed_temp = run[0];
														sum_temp = run[1];
														yaw_temp = run[2];
														xSemaphoreGive(run_useflag);
														car_goto_coll_do(go[0],go[1],speed_temp);
													}
													xSemaphoreTake(do_flag_useflag,portMAX_DELAY);
													do_flag = car_free;
													xSemaphoreGive(do_flag_useflag);
													Morse(0);
													break;
												case car_always:
													goto_flag = false;
													car_ATurning(0);
													while(!goto_flag)
													{
														xSemaphoreTake(run_useflag,portMAX_DELAY);
														float speed_temp,yaw_temp;
														speed_temp = run[0];
														yaw_temp = run[2];
														xSemaphoreGive(run_useflag);

														car_A_always_run(speed_temp, yaw_temp);   // ËÙ¶È¡¢½Ç¶È
													}
													xSemaphoreTake(do_flag_useflag, portMAX_DELAY);
													PID_Rest();
													Motor_Set(M_L,0);
													Motor_Set(M_R,0);
													do_flag = car_free;
													xSemaphoreGive(do_flag_useflag);
													Morse(0);
												break;
														
												default:
													xSemaphoreTake(do_flag_useflag,portMAX_DELAY);
													do_flag = car_free;
													xSemaphoreGive(do_flag_useflag);
												break;
											}
                    }
                }
