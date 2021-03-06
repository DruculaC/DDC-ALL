/*---------------------------------------------------
	Battery.c (v1.00)
	
	��ص����ж�
---------------------------------------------------*/	

#include "main.h"
#include "port.h"

#include "Battery.h"
#include "voice.h"
#include "Delay.h"

                                                         
/*----------------------------------------------------
	verifybattery() - Z2
	voice alarm for battery status without power module
-----------------------------------------------------*/
void verifybattery(tWord Check2)
	{	
	// battery is sufficient, battery > 50.4V, 3.68V/4.9V
	if((Check2 >= 0x301))
		{
		voice_EN = 1;
		SC_Speech(28);  	
		Delay(60);
		voice_EN = 0;
		}
   // 20km, 49V < battery < 50.4V, 3.62V/4.9V
	else if((Check2 < 0x301)&&(Check2 >= 0x2f4))		
		{
		voice_EN = 1;
		SC_Speech(10);  
		Delay(100);
		voice_EN = 0;
		}
	// 15km, 47.4V < battery < 49V, 3.55V/4.9V 
	else if((Check2 < 0x2f4)&&(Check2 >= 0x2e5))		
		{
		voice_EN = 1;
		SC_Speech(9);  	
		Delay(100);
		voice_EN = 0;
		}
	// 10km, 45.7V < battery <47.4V, 3.47V/4.9V
	else if((Check2 < 0x2e5)&&(Check2 >= 0x2d5))		
		{
		voice_EN = 1;
		SC_Speech(8);  	
		Delay(100);
		voice_EN = 0;
		}
	// 5km, 44.4V < battery < 45.7V, 3.41V/4.9V
	else if((Check2 < 0x2d5)&&(Check2 >= 0x2c8))		
		{
		voice_EN =1;
		SC_Speech(7);  	
		Delay(100);
		voice_EN = 0;
		}                     
	// battery is low, battery < 44.4V, 
	else if(Check2 < 0x2c8)
		{
		motorBAT_low_speech();
		}
	}

/*----------------------------------------------------
	verifybattery()  - Z4
	voice alarm for battery status with power module

void verifybattery(tWord Check2)
	{	
	// battery is sufficient, battery > 50.4V, 2.48V/5V
	if((Check2 >= 0x1fb))
		{
		voice_EN = 1;
		SC_Speech(28);  	
		Delay(60);
		voice_EN = 0;
		}
   // 20km, 49V < battery < 50.4V, 2.42V/5V
	else if((Check2 < 0x1fb)&&(Check2 >= 0x1ef))		
		{
		voice_EN = 1;
		SC_Speech(10);  
		Delay(100);
		voice_EN = 0;
		}
	// 15km, 47.8V < battery < 49V, 2.36V/5V 
	else if((Check2 < 0x1ef)&&(Check2 >= 0x1e3))		
		{
		voice_EN = 1;
		SC_Speech(9);  	
		Delay(100);
		voice_EN = 0;
		}
	// 10km, 46.5V < battery <47.8V, 2.3V/5V
	else if((Check2 < 0x1e3)&&(Check2 >= 0x1d7))		
		{
		voice_EN = 1;
		SC_Speech(8);  	
		Delay(100);
		voice_EN = 0;
		}
	// 5km, 45.1V < battery < 46.5V, 2.86V/4.93V
	else if((Check2 < 0x1d7)&&(Check2 >= 0x1c9))		
		{
		voice_EN =1;
		SC_Speech(7);  	
		Delay(100);
		voice_EN = 0;
		}                     
	// battery is low, battery < 45.1V, 
	else if(Check2 < 0x1c9)
		{
		motorBAT_low_speech();
		}
	}
-----------------------------------------------------*/
/*---------------------------------------------------
	end of file
----------------------------------------------------*/