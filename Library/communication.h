/*-----------------------------------------------------
	communication.h (v1.00)
	
	header file for communication.c
-----------------------------------------------------*/

#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

//-----public constants--------------------------------
// ����IDcard������
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
#define ComMode_6 0xc6	// ���߱���
#define ComMode_7 0xc7	// ����ʱΪ���������źţ���������ʱΪ�����ź�
#define ComMode_8 0xe8  // ģʽ�л����ֶ�ģʽ���Զ�ģʽ�л������յ�ʱ��ʾ�Զ�ģʽ
#define ComMode_9 0xc9	// ��������ʱΪ�����ź�
#define ComMode_10 0xc10	// ģʽ�л������յ�ʱ��ʾ�ֶ�ģʽ

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