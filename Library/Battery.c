/*---------------------------------------------------
	Battery.c (v1.00)
	
	µç³ØµçÁ¿ÅÐ¶Ï
---------------------------------------------------*/	

#include "main.h"
#include "port.h"

#include "Battery.h"
#include "voice.h"
#include "Delay.h"


/*----------------------------------------------------
	verifybattery()
	voice alarm for battery status

void verifybattery(tWord Check2)
	{	
	// battery is sufficient, battery > 50.7V, 3.72V/4.93V
	if((Check2 >= 0x304))
		{
		voice_EN = 1;
		SC_Speech(28);  	
		Delay(60);
		voice_EN = 0;
		}
   // 20km, 49.4V < battery < 50.7V, 3.66V/4.93V
	else if((Check2 < 0x302)&&(Check2 >= 0x2f8))		
		{
		voice_EN = 1;
		SC_Speech(10);  
		Delay(100);
		voice_EN = 0;
		}
	// 15km, 47.6V < battery < 49.4V, 3.58V/4.93V 
	else if((Check2 < 0x2f6)&&(Check2 >= 0x2e7))		
		{
		voice_EN = 1;
		SC_Speech(9);  	
		Delay(100);
		voice_EN = 0;
		}
	// 10km, 45.7V < battery <47.6V, 3.49V/4.93V
	else if((Check2 < 0x2e5)&&(Check2 >= 0x2d5))		
		{
		voice_EN = 1;
		SC_Speech(8);  	
		Delay(100);
		voice_EN = 0;
		}
	// 5km, 44.3V < battery < 45.7V, 3.42V/4.93V
	else if((Check2 < 0x2d3)&&(Check2 >= 0x2c6))		
		{
		voice_EN =1;
		SC_Speech(7);  	
		Delay(100);
		voice_EN = 0;
		}                     
	// battery is low, battery < 44.3V, 3.42V/4.93V
	else if(Check2 < 0x2c4)
		{
		motorBAT_low_speech();
		}
	}
-----------------------------------------------------*/

/*----------------------------------------------------
	verifybattery()
	voice alarm for battery status
-----------------------------------------------------*/

void verifybattery(tWord Check2)
	{	
	// battery is sufficient, battery > 50.7V, 3.26V/4.93V
	if((Check2 >= 0x2a4))
		{
		voice_EN = 1;
		SC_Speech(28);  	
		Delay(60);
		voice_EN = 0;
		}
   // 20km, 49.4V < battery < 50.7V, 3.17V/4.93V
	else if((Check2 < 0x2a2)&&(Check2 >= 0x291))		
		{
		voice_EN = 1;
		SC_Speech(10);  
		Delay(100);
		voice_EN = 0;
		}
	// 15km, 47.6V < battery < 49.4V, 3.07V/4.93V 
	else if((Check2 < 0x28f)&&(Check2 >= 0x27c))		
		{
		voice_EN = 1;
		SC_Speech(9);  	
		Delay(100);
		voice_EN = 0;
		}
	// 10km, 45.7V < battery <47.6V, 2.95V/4.93V
	else if((Check2 < 0x27a)&&(Check2 >= 0x263))		
		{
		voice_EN = 1;
		SC_Speech(8);  	
		Delay(100);
		voice_EN = 0;
		}
	// 5km, 44.3V < battery < 45.7V, 2.86V/4.93V
	else if((Check2 < 0x261)&&(Check2 >= 0x250))		
		{
		voice_EN =1;
		SC_Speech(7);  	
		Delay(100);
		voice_EN = 0;
		}                     
	// battery is low, battery < 44.3V, 
	else if(Check2 < 0x24e)
		{
		motorBAT_low_speech();
		}
	}

/*---------------------------------------------------
	end of file
----------------------------------------------------*/