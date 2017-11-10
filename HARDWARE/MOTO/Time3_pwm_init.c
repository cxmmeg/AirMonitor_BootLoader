#ifndef __Time3_pwm_init
#define __Time3_pwm_init
#include <stm32f10x.h>
#include "include.h"
uint16_t Timecnt2=0;
extern uint8_t CntFlag;
void Timer2_Init(uint32_t period)
{
    TIM_TimeBaseInitTypeDef	TIM_TimeBaseStructure;     //��ʱʱ��
    NVIC_InitTypeDef NVIC_InitStructure;                 //�ж�����
	/* Compute the prescaler value */
	/* Time base configuration */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);   //ʱ��ʹ��
	TIM_TimeBaseStructure.TIM_Period = period;//1ms����һ���ж�		       //��������	500,50ms����һ���ж�
	TIM_TimeBaseStructure.TIM_Prescaler = 72000-1;	           //����1KHz��Ƶ��
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//���ϼ���
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);        //��ʼ���ṹ��		
	
	TIM_ARRPreloadConfig(TIM2, ENABLE);   //ʹ��Ԥװ�ؼĴ���
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); 						//TIM2_DIER[2]=1  ��������ж�
	TIM_Cmd(TIM2, DISABLE);                      //ʹ�ܶ�ʱ��3��ʱ
	NVIC_InitStructure.NVIC_IRQChannel = 	TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���  
}
//��ʱ��3�����ж�����˿ڵ�״̬�Լ�Ϊ�������ṩ��ʱʱ��
void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)     //ȷ���Ƕ�ʱ��2�����ж�
    {
        TIM2->SR = (uint16_t)~TIM_IT_Update;		       //����жϱ�־λ
		CntFlag = 0;										//�����־
	}
}
//��ʱ��PWM��ʼ��
void Tim3_init(void)
{
    TIM_OCInitTypeDef TIM_OCInitStructure;       //����Ƚ�
    TIM_TimeBaseInitTypeDef	TIM_TimeBaseStructure;     //��ʱʱ��
    GPIO_InitTypeDef GPIO_InitStructure;
  
	uint16_t PrescalerValue = 0;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3,ENABLE);
	/* Compute the prescaler value */
	PrescalerValue = (uint16_t) (SystemCoreClock / 24000000) - 1;  //3-1=2;
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 999;		//��ʱ��Ԥװ��ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;	//pwmʱ�ӷ�Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//���ϼ���
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	/* PWM1 Mode configuration: Channel */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;//��ʼռ�ձ�Ϊ0
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
//	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
//	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
//	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
//	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	//��ʼ��PWM�����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7; //PBO PB1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	//TIM3->CCR4=250;
}
//����PWM��ֵ�����ڵƹ������
void SetDriverPWM(uint16_t Drive1,uint16_t Drive2)
{	
  if(Drive1>1000)
	  Drive1 = 1000;
	else if(Drive1<=0)
		Drive1 = 0;
	if(Drive2>1000)
	  Drive2 = 1000;
	else if(Drive2 <= 0)
		Drive2 = 0;
	TIM3->CCR1 = Drive1;
	TIM3->CCR2 = Drive2;
}




#endif


