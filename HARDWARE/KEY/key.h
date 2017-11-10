#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	
//����PC3�˿ڵ�״̬
#define  SET_IN   {GPIOC->CRH&=0XFFFFFF0F;GPIOC->CRH|=8<<4;}   //PC9�˿�����Ϊ��������ģʽ
#define  SET_OUT  {GPIOC->CRH&=0XFFFFFF0F;GPIOC->CRH|=3<<4;}  //PC9�Ķ˿�����Ϊ�������ģʽ

#define KEY0  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_3)//��ȡ����0
#define POWER_DETECC GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)
#define POWER_CTRL    PCout(6)
extern volatile uint8_t Encoder_TYPE;   //��������ֵ

//����ֵ
typedef enum
{
	Encoder_Void = 0,
	Encoder_Forword =1,
	Encoder_Backword = 2,
	Encoder_Pressed = 3
}EncoderVal;
void EncoderInit(void);		//��������ʼ��
uint8_t KeyScan(void);
void StartMachineInit(void);

	
	
	
	
#endif




