#include "key.h"
#include "delay.h"	 
//������ʼ������ 
volatile uint8_t Encoder_TYPE;   //��������ֵ
void EncoderInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//�ⲿ�жϣ���Ҫʹ��AFIOʱ��		
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//ʹ��PORTA,PORTCʱ��
	RCC_LSEConfig(RCC_LSE_OFF);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//�ر�jtag��ʹ��SWD��������SWDģʽ����
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3|GPIO_Pin_14|GPIO_Pin_15;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
 	GPIO_Init(GPIOC, &GPIO_InitStructure);			
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource3);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource14);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource15);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;           
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;   //�½��ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line14;           
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;   //�����ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line15;           
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;   //�����ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�11
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//�����ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); 
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�11
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//�����ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); 
}
//��������ʼ��
void StartMachineInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹ��PORTA,PORTCʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				 //LED0-->PD2 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	GPIO_ResetBits(GPIOC,GPIO_Pin_6);
}
//��������ֵ�ж�
void EXTI15_10_IRQHandler()
{
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntEnter();
#endif
	if(EXTI_GetITStatus(EXTI_Line14) != RESET)
    {
		delay_ms(10);
		EXTI->PR = EXTI_Line14;
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14) == 0)
		{
			Encoder_TYPE = Encoder_Forword;
		}
		
	}
	else if(EXTI_GetITStatus(EXTI_Line15) != RESET)
	{
		delay_ms(10);
		EXTI->PR = EXTI_Line15;
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_15) == 0)
		{
			Encoder_TYPE = Encoder_Backword;
		}
	}
#ifdef OS_TICKS_PER_SEC	 	
	OSIntExit();  					//�˳��жϱ���				 		 
#endif
}
//�����ж�
void EXTI3_IRQHandler(void)
{
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
OSIntEnter();
#endif 	
	if(EXTI_GetITStatus(EXTI_Line3) != RESET)
    {
		delay_ms(10);
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0)
		{
			Encoder_TYPE = Encoder_Pressed;
			//REncoderHandle(Encoder_TYPE);
		}
		EXTI_ClearITPendingBit(EXTI_Line3); 
	}
#ifdef OS_TICKS_PER_SEC	 	
OSIntExit();  					//�˳��жϱ���				 		 
#endif
}
//����ɨ�溯��
//֧�ֶ̰���������
//����ֵ��1����ֵ��0�̰�ֵ
uint8_t KeyScan(void)
{
	uint16_t Cnt = 0;
	if(KEY0==0)
	{
		delay_ms(15);	//ȥ���� 
		if(KEY0==0)
		{
			while(KEY0&&Cnt<=300)
			{
				Cnt++;
				delay_ms(10);
			}
			if(Cnt>300)
				return 0;
			else
				return 0;
		}
	}
	return 1;
}










