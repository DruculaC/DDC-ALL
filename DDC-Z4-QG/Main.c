/*---------------------------------------------------
	main.c (v1.00)
	
	DDC-Z program, for electrocar, with ID card.
----------------------------------------------------*/

//电动车
#include "Main.h"
#include "port.h"

#include "AD.h"
#include "voice.h"
#include "pwm.h"
#include "Timer.h"
#include "Delay.h"
#include "communication.h"
#include "Battery.h"
#include "Other.h"
#include "operation.h"
#include "UART.h"
                                        
/*------- Public variable declarations --------------------------*/
extern tByte timer0_8H, timer0_8L, timer1_8H, timer1_8L;
extern bit enable_sensor_delayEN;

// ------ Private variable definitions -----------------------------
bit stolen_alarm_flag = 0;					// when host been touch 3 times, this flag 1 before alarm voice present, not to detect sensor for 1st voice alarm.
bit position_sensor_EN=0;  		//位置传感器，即倒地抬起传感器总开关，1的时候，检测这两个传感器
bit host_stolen_alarm1_EN = 0;      //判断为被盗报警后的第一段语音使能
bit host_stolen_alarm2_EN = 0;      //判断为被盗报警后的第二段语音使能
tByte host_stolen_alarm1_count = 0;		//判断为被盗报警后的第一段语音次数
tByte host_stolen_alarm2_count = 0;		//判断为被盗报警后的第二段语音次数
bit fell_flag=0;						//判断主机倒下后，置1
bit raised_flag=0;					//判断主机被抬起后，置1
tByte sensor_2ndstage_count = 0;		//传感器进入第二阶段检测时，计算低电平的时间
tWord sensor_3rdstage_time = 0;			//传感器进入第三阶段的时间，
tByte sensor_3rdstage_effcount = 0;		//传感器进入第三阶段后，有效触碰次数的计数
tByte sensor_3rdstage_count = 0;			//传感器进入第三阶段后，低电平的计数
tWord sensor_3rdstage_interval = 0;		//传感器在第三阶段中，每次有效低电平计数之间的时间间隔。在这期间的低电平不认为有效。
bit raised_fell_flag = 0;					//倒地或者抬起触发后，此标志位置1
tByte sensor_trigger_count=0;		//传感器触发计数
tWord sensor_2ndstage_time=0;		//传感器进入第二阶段后流逝时间的计数
tByte sensor_1ststage_count=0;	//传感器第一阶段判断低电平的计数
tByte nearby_away_interval = 0;		//附机离开和靠近时，语音提示和开关锁的时间间隔

tByte raised_alarm_count = 0;    //主机被抬起后，向附机发出报警信号的次数
tByte fell_alarm_count=0;        //主机倒地后，向附机发出报警信号的次数
tWord timer0_count=0;		// counter for timer0, increment every ticket 			
tByte received_data_buffer[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};		//接收数据缓存
bit receive_data_finished_flag = 0;		//接收这一串数据完成后，此标志位置1
tByte data_count = 0;				//接收数据缓存的位数，即表明第几个缓存数据
tByte one_receive_byte = 0;		//接收数据的一个字节，接收完后将其赋值给received_data_buffer相对应的字节
tByte one_receive_byte_count = 0;			//one_receive_byte有8位，此计数表明接收到第几位，每次计数到8的时候表明一个字节接收完成。
bit receive_wire_flag = 1;		//接收通信线的标志位，1表明高电平，0表明低电平，每次timer1溢出时，判断P1.1一次。以此来表明是否为一次正确的下降沿
tByte receive_HV_count = 0;		//定时器T1在没有信号到来的时候，对高电平计数，一旦超过某个值，则将one_receive_byte_count清0
tByte receive_LV_count = 0;		//每次timer1溢出时判断接收线如果为LV，则计数加1，以此来表明低电平的时间
tByte fell_wire_time=0;          //倒地检测线，检测低电平的时间
tByte raise_wire_time=0;			//抬起检测线，检测低电平的时间
tWord raised_fell_number = 0;				//倒地或者抬起出发后，计数，到达一定数值后，将其与标志位一起清零。
bit raised_fell_once_flag = 0;			//raised_fell_flag是否曾经标志过，如果标志过则置1.然后主机被恢复倒地或者恢复抬起时，此标志位复位。
tByte key_rotated_on_flag = 1;			//电动车开启标志位，1表示电动车开启了，0表示电动车关闭了
tByte key_rotated_off_flag = 0;			//电动车关闭标志位，1表示电动车开启了，0表示电动车关闭了
tWord ADC_check_result = 0;		//作为AD检测值
tByte wire_broken_count = 0;		// 作为断线后的时间检测
bit battery_stolen_EN = 0;			// 作为电池被盗的使能端
tByte battery_stolen_count = 0;	// 作为电池被盗的报警次数
bit horizontal_vibration = 0;		// 表示垂直传感器在振动，此时就算关钥匙，也不能执行关钥匙的操作�
tWord horizontal_vibration_count = 0;	//垂直传感器触发后，对时间进行计数。
bit vibration_flag = 0;
tWord vibration_count = 0;
bit wire_broken_flag = 0;			// 剪断锁线的标志位
tByte IDkey5 = IDkey0^IDkey1^IDkey2^IDkey3^IDkey4;		// 由前5位密码组成的异或
bit IDkey_flag = 0;			// 当ID卡靠近时认证通过后置1，
tByte IDkey_count = 0;		// ID卡认证通过后，计时1分钟，使钥匙能转动。
bit sensor_EN = 0;
tByte enable_sensor_delay_count = 0;		// 传感器延迟的时间
bit sensor_3rdalarm_flag = 0;
bit wheeled_flag = 0;
tWord wheeled_count = 0;

tByte key_rotate_count = 0;
bit key_rotate_flag = 0;
bit lock_rotate_on_flag = 1;
bit lock_rotate_off_flag = 0;
bit slave_away_flag = 1;
bit magnet_CW_flag = 1;
bit open_lock_EN = 0;
bit close_lock_EN = 0;
bit recovery_from_alarm = 0;

void main()
	{

/*	InitUART();
	InitT0(1);
	
	TI = 0;
	RI = 0;
	ES = 1;
	ET0 = 1;
	PS = 1;
	EA = 1;
*/
	
	InitTimer(1,100);

	sensor_EN = 0;
 	position_sensor_EN = 0;

	noVoice();

	voice_EN=0;		//将功放关闭

	raised_sensor_detect=1;
	fell_sensor_detect=1;

	P10=1;
	
    horizontal_sensor = 1;
	
	// turn off transmitter, turn on receiver
	transmiter_EN = 1;
	receiver_EN = 0;
	
	// initialize the magnet, 
	MagentControl_1 = 1;
	MagentControl_2 = 1;

	transmiter_power = 1; 
	
	wire_broken = 1;
	TR0 = 1;
	
	// lock the external motor, 防止锁还没完全打开的时候，车手加电导致轮子与锁的告诉碰撞。 
	motor_lock = 1;
	while(1)
		{        		
		// 判断传感器是否在振动
		if((sensor_detect == 0)||(horizontal_sensor == 0))
			{
			vibration_flag = 1;
			vibration_count = 0;			
			}
		}
	}

/*------------------------------------------------------------------
	timerT0()
	operation every ticket.
--------------------------------------------------------------------*/

void timer0() interrupt interrupt_timer_0_overflow
	{
	// manually reload timer0 configuration
	TH0 = timer0_8H;
	TL0 = timer0_8L;
	
	// timer0 is 1ms ticket, count the time flow of timer0, then operate every 2s.
	if(++timer0_count >= 2000)
		{
		// reset timer0 ticket counter every 2s
		timer0_count=0;

/*
		if(++key_rotate_count < 16)		
			{
			if((key_rotated_on_flag == 1)||(key_rotated_off_flag == 1))
				{
				ComMode_Data(ComMode_1, 28);				
				}
			}
		else
			key_rotate_count = 31;
			
		if(lock_rotate_off_flag == 0)
			{
			if(++IDkey_count > 4)
				{
				IDkey_count = 7;
				slave_away_flag = 1;
				}			
			}
*/
		// detect the battery voltage
		ADC_check_result = GetADCResult(6);	
		

		// if fell and raised flag is 1, send alarm signal every 2s.
		if((fell_flag==1)&&(fell_alarm_count<5))
			{
			transmiter_power = 0;
			ComMode_Data(ComMode_5, 28);
			transmiter_power = 1;
			fell_alarm_count++;
			}
		if((raised_flag==1)&&(raised_alarm_count<5))		
			{
			transmiter_power = 0;
			ComMode_Data(ComMode_4, 28);
			transmiter_power = 1;
			raised_alarm_count++;
			}
			
		if((battery_stolen_EN == 1)&&(battery_stolen_count < 4))
			{
			if(key_rotate == 0)
				{
				transmiter_power = 0;
				ComMode_Data(ComMode_2, 28);
				transmiter_power = 1;
				battery_stolen_speech();
				battery_stolen_count++;
				}
			}
/*		
		if(IDkey_flag == 1)
			{
			if(++IDkey_count >= 12)
				{
				IDkey_count = 0;
				IDkey_flag = 0;
				if(key_rotated_off_flag == 1)
					{
					enable_sensor();					
					}
				}			
			}
*/
									
		if(enable_sensor_delayEN == 1)
			{
			if(++enable_sensor_delay_count >= 5)
				{
				enable_sensor_delay_count = 0;
				enable_sensor_delayEN = 0;
				enable_sensor();
				}
			}
					
		// whether host has been touched 3 times, if yes, then alarm 2 speech alternantively.
		if((host_stolen_alarm1_EN == 1)&&(host_stolen_alarm1_count < 4))
			{
			stolen_alarm_flag = 1;
			if(key_rotate == 0)
				{
				transmiter_power = 0;
				if(wire_broken_flag == 0)
					{
					ComMode_Data(ComMode_3, 28);                                                                  
					}
				else
					{                                                                     
					ComMode_Data(ComMode_6, 28);
					}
				transmiter_power = 1;
					
				stolen_alarm_speech1();
				}
			if(++host_stolen_alarm1_count >= 4)
				{                                                                                       
				host_stolen_alarm1_count = 0;
				host_stolen_alarm1_EN = 0;
				stolen_alarm_flag = 0;
				sensor_3rdalarm_flag = 0;
				}
			}
		if((host_stolen_alarm2_EN == 1)&&(host_stolen_alarm2_count < 4))
			{
			stolen_alarm_flag = 1;
			if(key_rotate == 0)
				{
				transmiter_power = 0;
				if(wire_broken_flag == 0)
					{
					ComMode_Data(ComMode_3, 28);
					}
				else
					{
					ComMode_Data(ComMode_6, 28);
					}
				transmiter_power = 1;
				stolen_alarm_speech2();
				}
			if(++host_stolen_alarm2_count >= 4)
				{
				host_stolen_alarm2_count = 0;
				host_stolen_alarm2_EN = 0;
				stolen_alarm_flag = 0;
				sensor_3rdalarm_flag = 0;
				}
			}			
		}
	
	if((key_rotate == 1)&&(key_rotated_on_flag == 0))
		{		
		Delay(5);
		if(key_rotate == 1)
			{
			// 设置开钥匙和关钥匙的标志位
			key_rotated_on_flag = 1;
			key_rotated_off_flag = 0;
			lock_rotate_on_flag = 0;
			
			key_rotate_count = 0;			
			disable_sensor();	
			}
		}
		
	if(open_lock_EN == 1)
		{
		if((magnet_CW_flag == 0)&&(stolen_alarm_flag == 0))
			{
			magnet_CW(1000, 3200, 28);
			magnet_CW_flag = 1;			
			slave_nearby_operation();
			}
		if(stolen_alarm_flag == 1)
			{
			recovery_from_alarm = 1;
			}
		open_lock_EN = 0;
		disable_sensor();
		}
/*		
	// detect whether key is rotated on,  
	if((key_rotated_on_flag == 1)&&(IDkey_flag == 1)&&(lock_rotate_on_flag == 0))		
		{                                                                        
		magnet_CW(2000, 4000, 28);
		
		slave_nearby_operation();                     
		IDkey_count = 0;
		IDkey_flag = 0;
		lock_rotate_on_flag = 1;
		key_rotate_count = 31;
		}
*/		
	if((key_rotate == 0)&&(key_rotated_off_flag == 0)&&(vibration_flag == 0)&&(wheeled_flag == 0))
		{
		Delay(5);
		if(key_rotate == 0)
			{
			// 设置开钥匙和关钥匙的标志位
			key_rotated_on_flag = 0;
			key_rotated_off_flag = 1;
			lock_rotate_off_flag = 0;
			// enable_sensor();	
			enable_sensor_delayEN = 1;
			enable_sensor_delay_count = 0;

			key_rotate_count = 10;
			}		
		}
		
	if(close_lock_EN == 1)
		{
		if((magnet_CW_flag == 1)&&(vibration_flag == 0)&&(wheeled_flag == 0))
			{
			verifybattery(ADC_check_result);
			magnet_ACW(1000, 4000);			
			magnet_CW_flag = 0;
			slave_away_operation();
			}
		if(recovery_from_alarm == 1)
			{
			recovery_from_alarm = 0;
			// enable_sensor();	
			enable_sensor_delayEN = 1;
			enable_sensor_delay_count = 0;			
			}
		close_lock_EN = 0;
		}
/*
	if((key_rotated_off_flag == 1)&&(slave_away_flag == 1)&&(lock_rotate_off_flag == 0))
		{
		if((vibration_flag == 0)&&(wheeled_flag == 0))
			{
			// handle with battery status
			verifybattery(ADC_check_result);
			magnet_ACW(6000, 10000);			
			
			// reset key rotation flag
			slave_away_operation();
			
			key_rotate_count = 31;
			slave_away_flag = 0;
			lock_rotate_off_flag = 1;
			IDkey_flag = 0;
			}
		}
*/	
	if((sensor_detect == 0)||(horizontal_sensor == 0))
		{
		vibration_flag = 1;
		vibration_count = 0;
		}		
	if(vibration_flag == 1)
		{
		if(++vibration_count > 2000)
			{
			vibration_flag = 0;
			vibration_count = 0;
			}
		}		
	
	if(wheeled_rotate == 1)
		{
		wheeled_flag = 1;
		wheeled_count = 0;
		}
	if(wheeled_flag == 1)
		{
		if(++wheeled_count >= 2000)
			{
			wheeled_flag = 0;
			wheeled_count = 0;
            }
		}


	// judge host is fell or raised every 1ms?
	if((raised_sensor_detect == 1)&&(fell_sensor_detect == 1))
		{
		// judge vibration sensor is enable?
		if(sensor_EN == 1)	
			{			
			// sensor trigger status, 0, 1 or 2?
			switch(sensor_trigger_count)
				{
				// case 0, it means host is in waiting status. waiting for first touch.
				case 0:
					{					
					// judge host been touched and also not in vibration alarm
					if(((sensor_detect == 0)||(horizontal_sensor == 0))&&(stolen_alarm_flag == 0))		
						{
						// judge LV is more than 2ms, if yes, it means a effective touch
						if(++sensor_1ststage_count >= 2)			
							{
							sensor_1ststage_count=0;
							
							// sensor trigge status progress to case 1.
							sensor_trigger_count = 1;
							// alarm speech for first touoch
							host_touch_speech();
							}
						}
					else
						{
						// if no LV, reset the count.
						sensor_1ststage_count = 0;
						}
					}
				break;
				
				// waiting for next touch, 
				case 1:
					{
//					if((sensor_detect == 0)||(horizontal_sensor == 0))
					if((sensor_detect == 0)||(horizontal_sensor == 0))
						{
						// LV for 2s, means a effective touch             
						if(++sensor_2ndstage_count >= 2)
							{
							sensor_2ndstage_count = 0;
							sensor_trigger_count = 2;
							// alarm speech for 2nd touch
							host_2ndtouch_speech();
							}
						}
					else
						{
						sensor_2ndstage_count = 0;
						}
					
					// if there is no touch in 4s, reset sensor trigger status, etc.
					if(++sensor_2ndstage_time >= 4000)
						{
						sensor_trigger_count = 0;
						sensor_2ndstage_count = 0;
						sensor_1ststage_count = 0;
						sensor_2ndstage_time = 0;
						}
					}
				break;
				
				// waiting for 3rd touch
				case 2:
					{
					if((sensor_detect == 0)||(horizontal_sensor == 0))
						{
						// 2s LV is a effective touch
						if(++sensor_3rdstage_count >= 1)
							{
							sensor_3rdstage_count = 0;
							// stolen alarm speech enable
							host_stolen_alarm1_EN = 1;
							host_stolen_alarm2_EN = 1;						
							sensor_3rdalarm_flag = 1;							
							}
						}
					else
						{
						sensor_3rdstage_count = 0;
						}
					
					// if there is no touch in 4s, reset all.
					if(++sensor_3rdstage_time >= 4000)
						{
						sensor_trigger_count = 0;
						sensor_1ststage_count = 0;
						sensor_2ndstage_count = 0;
						sensor_2ndstage_time = 0;
						sensor_3rdstage_time = 0;
						sensor_3rdstage_interval = 800;
						sensor_3rdstage_count = 0;
						sensor_3rdstage_effcount = 0;					
						}
					}
				break;
				}
			
			// judge the wire broken, if yes, it means someone has cut the wire of magnet lock
			if((wire_broken == 0) && (wire_broken_count < 51))  
				{
				if(++wire_broken_count > 50)
					{
					host_stolen_alarm1_EN = 1;
					host_stolen_alarm2_EN = 1;	
					wire_broken_count = 51;
					wire_broken_flag = 1;
					}
				}
			else if((wire_broken == 1)&&(sensor_3rdalarm_flag == 0))
				{
				wire_broken_count = 0;
				host_stolen_alarm1_EN = 0;
				host_stolen_alarm2_EN = 0;
				}
			
			// judge whether battery has been stolen, 0.4V/4V
			if(ADC_check_result < 0x100)
				{
				battery_stolen_EN = 1;
				}
			else if(ADC_check_result > 0x100)
				{
				battery_stolen_EN = 0;
				battery_stolen_count = 0;
				}			
			}
		}
	
	// judge whether position sensor is enable
	if(position_sensor_EN==1)		
		{
		// judge whether there is a LV
		if(raised_sensor_detect == 0)	
			{
			// LV > 0.5s means a effective input
			if(++raise_wire_time >= 3)
				{
				// flag raised, and reset fell
				raised_flag=1;
				fell_flag=0;
				raise_wire_time = 11;
				// judge whether there once been a raised or fell.
				if(raised_fell_once_flag == 0)
					{
					// if no, flag raised and fell flag
					raised_fell_flag = 1;					
					}
				}		
			}
		else
			{
			raised_flag=0;
			raised_alarm_count=0;
			raise_wire_time=0;
			raised_fell_flag = 0;
			}
      // judge whether there is a LV
		if(fell_sensor_detect==0)
			{
			// LV > 0.5s means a effective input
			if(++fell_wire_time >= 3)	
				{
				// flag fell, and reset raised
				fell_flag=1;			
				raised_flag=0;
				fell_wire_time = 11;
				// judge whether there once been a raised or fell
				if(raised_fell_once_flag == 0)
					{
					raised_fell_flag = 1;					
					}
				}		
			}
		else
			{
			fell_flag=0;
			fell_alarm_count=0;
			fell_wire_time=0;
			raised_fell_flag = 0;
			}
		}
	
	// judge whether raised or fell, if yes, reset all after 10s.
	if(raised_fell_flag == 1)
		{
		if(++raised_fell_number >= 4000)
			{
			raised_fell_flag = 0;
			raised_fell_number = 0;
			raised_fell_once_flag = 1;
			
			sensor_trigger_count = 0;
			sensor_1ststage_count = 0;
			sensor_2ndstage_count = 0;
			sensor_2ndstage_time = 0;
			sensor_3rdstage_time = 0;
			sensor_3rdstage_interval = 800;
			sensor_3rdstage_count = 0;
			sensor_3rdstage_effcount = 0;					
			}
		}
		
	// detect the horizontal sensor
	if(((horizontal_sensor == 0)||(sensor_detect == 0))&&(horizontal_vibration_count > 5000))
		{
		Delay(3);
		if((horizontal_sensor == 0)||(sensor_detect == 0))
			{
			horizontal_vibration = 1;
			horizontal_vibration_count = 0;
			}
		}
	if(++horizontal_vibration_count >= 5000)
		{
		horizontal_vibration_count = 5001;
		horizontal_vibration = 0;
		}
 	}

/*-----------------------------------------------
	用UART端口接收数据，9600波特率，收到后表示
	认证已通过。
void uart_isr() interrupt 4 //中断接收数据
	{
	if(RI)
		{
		RI=0;

		// assign one byte to buffer[i] 
		received_data_buffer[data_count] = SBUF;
		
		// judge whether buffer[0] is CmdHead
		if((data_count == 0) && (received_data_buffer[0] == IDkey0))
			{
			data_count = 1;
			}
		else if((data_count == 1) && (received_data_buffer[1] == IDkey1))
			{
			data_count = 2;
			}
		else if((data_count == 2) && (received_data_buffer[2] == IDkey2))
			{
			data_count = 3;
			}
		else if((data_count == 3) && (received_data_buffer[3] == IDkey3))
			{
			data_count = 4;
			}
		else if((data_count == 4) && (received_data_buffer[4] == IDkey4))
			{
			data_count = 5;
			}
		else if((data_count == 5) && (received_data_buffer[5] == IDkey5))
			{
			data_count = 0;	
			IDkey_flag = 1;
			IDkey_count = 0;
			disable_sensor();
			}
		else 
			{
			data_count = 0;
			}
		}
	}
-----------------------------------------------*/

/*--------------------------------------------------------------------
	timerT1()
	定时器1每次溢出后执行的操作，低速率模式接收
	数据。
--------------------------------------------------------------------*/

void timerT1() interrupt interrupt_timer_1_overflow
	{
   // reload timer1
	TH1 = timer1_8H;				
	TL1 = timer1_8L;
	
	// receive a tyte
	receive_byte();
	
	// receive a word after every byte
	receive_word();
	
	// judge whether a byte has received successfully
	if(receive_data_finished_flag == 1)
		{
		receive_data_finished_flag = 0;
		// judge which mode is received.
		switch(received_data_buffer[2]) 		
			{
			case ComMode_1:  
				{
				IDkey_flag = 1;
				IDkey_count = 0;
				disable_sensor();
				}
			break;
			
			case ComMode_7:
				{
				open_lock_EN = 1;
				}
			break;

			case ComMode_8:
				{
				close_lock_EN = 1;
				}
			break;
			}
		}
	}


/*---------------------------------------------------
	end of file
----------------------------------------------------*/