#include "include.h" 	   
BEEP Bee;
//��ʼ��D2Ϊ�����.��ʹ������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;			
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PB,PD�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 //LED0-->PD2 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOD2
	GPIO_SetBits(GPIOC,GPIO_Pin_4);						 //PD2 �����
}
void LEDAlarm(uint8_t Times)
{
    uint8_t i;
    for(i=0;i<Times;i++)
    {
        LED0 = !LED0;
        delay_ms(300);
    }
}
//�������˿ڳ�ʼ��
void Beep_init(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;
	
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOA, &GPIO_InitStructure);	
   GPIO_ResetBits(GPIOA,GPIO_Pin_3);//��ʼʱ�رգ�����������
	 
   Bee.BeepEn = 0;  //ʹ��
   Bee.BeepTime = 0;  //ʱ��
}
//state:����߲���
//times:����ʱ��,ȡֵ��Χ 0---2000
void BeepAlarm(uint8_t Enable,uint16_t times)
{
	Bee.BeepTime = times;
	if(Enable==1)
	{
		 Bee.BeepEn = 1;   //ʹ�ܱ�־
	   TIM3->CR1 |= TIM_CR1_CEN;  //�򿪶�ʱ��3��Ϊ���������ж�ʱ		 
	}
	else
	{
		 Bee.BeepEn = 0;                                       //ȡ��ʹ�ܱ�־
	}
}



