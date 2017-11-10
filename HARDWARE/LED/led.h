#ifndef __LED_H
#define __LED_H	 
#include "include.h"								  
////////////////////////////////////////////////////////////////////////////////// 
typedef struct
{
   uint8_t BeepEn;
	 uint8_t BeepTime;
}BEEP;
extern BEEP Bee;  //������״̬��־

#define LED0    PCout(4)	//     ָʾ��

void LED_Init(void);//��ʼ��
void BeepAlarm(uint8_t Enable,uint16_t times);  //����������
void Beep_init(void);  //��������ʼ��
void LEDAlarm(uint8_t Times);
void JDQInit(void);
#endif
