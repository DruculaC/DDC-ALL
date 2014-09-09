/*-----------------------------------------------------
	communication.h (v1.00)
	
	header file for communication.c
-----------------------------------------------------*/

#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

//-----public constants--------------------------------
// 定义IDcard的密码
#define IDkey0 0x00
#define IDkey1 0x00
#define IDkey2 0x00
#define IDkey3 0x13
#define IDkey4 0xEC

#define CmdHead 0xc8
#define MyAddress 0xe0

#define ComMode_1 0xc1	// the verification/confirmation signal. once slave receive this signal, slave transmit a confirmation signal.
						// once host receive this signal, it knows slave is here.
#define ComMode_2 0xc2 	// battery stolen signal out
#define ComMode_3 0xc3 	// host stolen alarm signal 
#define ComMode_4 0xc4 	// raised alarm signal
#define ComMode_5 0xc5	// fell alarm signal
#define ComMode_6 0xc6	// 断线报警
#define ComMode_7 0xc7	// 发送时为开锁关锁信号，附机接收时为开锁信号
#define ComMode_8 0xe8  // 模式切换，手动模式和自动模式切换，当收到时表示自动模式
#define ComMode_9 0xc9	// 附机接收时为关锁信号
#define ComMode_10 0xc10	// 模式切换，当收到时表示手动模式

//---------public function prototype

void initsignal(void);
void Send_Data(tByte x);
void ComMode_Data(tByte ComMode, x);
void receive_byte(void);
void receive_word(void);
void send_code_to_lock(tByte x, y);	

#endif
											
/*---------------------------------------------------
	end of file
----------------------------------------------------*/