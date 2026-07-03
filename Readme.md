# 引脚定义

## 1,电机Motor

### 	A电机左,GPT4,A

​			PIN1	P203

​			PIN2	P202

​			PWM	P302

### 	B电机右,GPT4,B

​			PIN1	P204

​			PIN2	P405

​			PWM	P301

## 2,编码器Encoder

### 	A编码器,GPT0

​			A相	P512

​			B相	P414

### 	B编码器,GPT1

​			A相	P109

​			B相	P110

## 3,惯导IMU,UART9

​		RX	P601

​		TX	P602

## 4,定位标签UWB,UART5

​		RX	P502

​		TX	P501

## 5,舵机servo

### 	云台舵机上,GPT6,A

​		PWM	P702

### 	云台舵机下,GPT6,B

​		PWM	P703

### 门控舵机,GPT7,A

​		PWM	P304

## 6,导航键Key,GPIO上拉输入

​		上U	P208

​		下D	P905

​		左L	P303

​		右R	P610

​		中I	P210

## 7,显示屏OLED,I2C1

​		SCL	P205

​		SDA	P511

## 8,调试Debug,UART7

​		RX	P614

​		TX	P613

## 9,超声波测距,ultrasonic,GPT3

Trig	P005	GPIO		触发信号	

Echo	P806	ICU0,IRQ0	回应信号

## 10.蜂鸣器Buzzer,GPIO

​		I/O		P804

## 11.蓝牙模块,UART

​	RX	P607

​	TX	PA00

## 12.超声波测距,ultrasonic_ADC,ADC0

​	Out			P005

​	Ronge		P806

## 13.LED

​	STCP	P003

​	SHCP	P106

​	DS	P600











# 蓝牙指令

## 1.运行指令(直线行走)

### 	run (速度)speed (距离)sum (角度)yaw 

## 2.绝对转向指令

### 	 aturn (角度)yaw

## 3.相对转向角度

### 	rturn (角度)yaw

## 4.车门控制指令

### 	door state(参数)[open/close]

## 5.避障前进指令

### 	coll (角度)yaw

## 6.前进目标坐标(无避障)

### 	goto x y  米

## 7.前进目标坐标(有避障)

### 	gotocoll x y  米

## 8.停止指令

### 	stop





