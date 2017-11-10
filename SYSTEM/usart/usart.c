#include "sys.h"
#include "usart.h"	  
#include <stdio.h>                                                                                            
//////////////////////////////////////////////////////////////////////////////////
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_UCOS
#include "include.h"					//ucos ʹ��
#endif
//********************************************************************************
//V1.3�޸�˵��
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
//////////////////////////////////////////////////////////////////////////////////
 
///�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//����1֧��printf����
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (uint8_t) ch;      
	return ch;
}
#endif 

/*ʹ��microLib�ķ���*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/

//�����ͻ�����
static uint8_t Step = 0;    //�������ݽ��ղ��� 
static uint8_t Step3 = 0;
uint8_t UartRecBuf[USART_REC_LEN];  //�ֽڴ�С100����
uint8_t WIFIRecDataFlag = 0;    //�������ݱ�־
//uint8_t WifiSetFlag = 0;   //������ʼ��ʱ��Ϊ��
uint8_t JQRec[15];       //��ȩ���������յ������ݻ�����
uint8_t jQRecSta = 0;     //��¼��ȩ���������յ������ݵ�ʵ��λ��
uint8_t JQRecFlag = 0;    //��ȩ���ݽ�����ɱ�־
uint8_t JQDataDealRes = 0; //��ȩ���ݴ�����
void PB11ConfigUart(uint8_t Mode)
{
	switch(Mode)
	{
		case UartMode:
		{    
			//����PB11Ϊ��������
			{GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=4<<12;}
			//����PB10Ϊ�����������ģʽ
			{GPIOB->CRH&=0XFFFFF0FF;GPIOB->CRH|=11<<8;}
		}break;
		case TFTMode:
		{    //����PB11Ϊͨ���������ģʽ
			{GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=3<<12;}
			//����PB10Ϊͨ���������ģʽ
			{GPIOB->CRH&=0XFFFFF0FF;GPIOB->CRH|=3<<8;}
		}break;
		default:break;
	}
}
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
//����״̬��,��ʼ�룬��ʼ�룬�ֽڳ��ȣ�
//uint8_t USART_TX_BUF[USART_TEC_LEN];         //���ͻ�����,��ʱ����  
//��ʼ��IO ����1 ����3 ����4
//bound:������
//����1�� TX PA.9   RX	PA.10  ����3��TX PB10   RX	 PB11  ����4�� TXPC10   RXPC11
//ע�⣺����1���ȼ����ڴ���3
void uart_init(u32 bound1,u32 bound2,u32 bound4)
{
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//ʹ��USART1��GPIOAʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB1Periph_UART4, ENABLE);	//ʹ��USART1��USART4ʱ��ʱ��
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
//    USART_DeInit(USART1);  //��λ����1
//    USART_DeInit(USART3);
//    USART_DeInit(UART4);
  
    //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9  
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10		
	//USART2_TX	  PA2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
	//USART2_RX   PA3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10	
    //USART4_TX PC10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOC, &GPIO_InitStructure); //��ʼ��C10	
		//USART4_RX	  PC11d
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOC, &GPIO_InitStructure);  //��ʼ��PC11
      
     //USART1 ��ʼ������
    USART_InitStructure.USART_BaudRate = bound1;//һ������Ϊ9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(USART1, &USART_InitStructure); //��ʼ������1
    //USART3 ��ʼ������
    USART_InitStructure.USART_BaudRate = bound2;//һ������Ϊ9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(USART2, &USART_InitStructure); //��ʼ������3
     //USART4 ��ʼ������
    USART_InitStructure.USART_BaudRate = bound4;//һ������Ϊ9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(UART4, &USART_InitStructure); //��ʼ������4   
    
	//Usart1 NVIC ����   WIFI���ݴ���
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2 ;//��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ��� 
    //Usart2 NVIC ����   WIFI����
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2 ;//��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ��� 
    //Usart4 NVIC ����   ��ȩ���������ݽ���
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2 ;//��ռ���ȼ�2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ��� 
	
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);//�����ж�1
    USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);//�����ж�3
    USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);//�����ж�4
    USART_Cmd(USART1, DISABLE);                    //ʹ�ܴ���1
    USART_Cmd(USART2, DISABLE);
    USART_Cmd(UART4, DISABLE);
//ʹ�ܴ���3
}
//����У�����
uint8_t CheckSum(uint8_t *buf,uint8_t len)
{
    uint8_t i;
    uint8_t temp=0;
    for(i=0;i<len;i++)
    {
       temp^=*(buf+i);
    }
    return temp;
}
//�ж�֡���ݰ��Ƿ�����ɽ���
uint8_t FindPkg(uint8_t *buf)
{
    uint8_t i;
    for(i=0;i<Step;i++)
    {
       if(*(buf+i)=='+'&&*(buf+i+1)=='I') //����֡ͷ
       {
          if(*(buf+i+3)=='D')    //���ݳ����Ƿ�Ϊ�˴ν��յ���ʵ�ʳ���
            return i;                    //�����濪ʼ�ĵط�
       }
    }
    return 0XFF;
}
//������ݰ�
uint8_t CheckPkg(uint8_t *buf)
{
   uint8_t i;
   for(i=0;i<Step;i++)
	{
		if(*(buf+i-1)=='&')
		{
			if(*(buf+i)=='#')
				return 0;
		}
	}
	return 1;
}
//�����жϽ��ճ���
//�жϽ��յ�������֡
//+IPD,0,7:������
//OK
uint8_t RemFlag = 0;
#if EN_USART1_RX   //���ʹ���˽���
void USART1_IRQHandler(void)                	//����1�жϷ������
{
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.�����жϱ���
   OSIntEnter();    //����OS�жϱ���
#endif
   if(USART1->SR&(1<<5))  //�ж��Ƿ�Ϊ����1�����ж�	
   {
      if(Step<USART_REC_LEN)  //С��ʵ�ʵĴ�С���������
      {
         UartRecBuf[Step]=USART1->DR;	//��ȡ���յ�������
         Step++;        //���ܴ���
      }
      else
      {
         Step=0;  
		 memset((void *)&JQRec[0],0,USART_REC_LEN);  //��ջ����� 		  
      }
      if(UartRecBuf[Step-1]=='D'&&RemFlag ==0)  //����������������
      {
		if(UartRecBuf[Step-4]=='+')
			RemFlag = 1;
      }
	  if(RemFlag)
	  {
		if(UartRecBuf[Step-1]=='#')
		{
			if(UartRecBuf[Step-2]=='&')
			{
				RemFlag = 0;
				WIFIRecDataFlag = 1;
			}
		}
	  }
   }
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntExit();  			//�˳��жϱ���									 
#endif
}
#endif
//����3�жϺ���
void USART3_IRQHandler(void)                	//����1�жϷ������
{
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.�����жϱ���
OSIntEnter();    
#endif	
	if(USART3->SR&(1<<5))  //�����ж�
	{
		if(Step3<USART_REC_LEN)
		{
			
		}
		else
		{
			memset((void *)&UartRecBuf[0],sizeof(UartRecBuf),0); //�������Ļ���ջ�����
		}
		Step3++;     //���ܴ���
		if(Step3==USART_REC_LEN)
			Step3=0;
	}
#ifdef OS_TICKS_PER_SEC	 	
	OSIntExit();  					//�˳��жϱ���				 		 
#endif	
}
//��ȩ�������������ݴ���
//��������ʾ��ת�ɸ�����
//����ֵ;0��ȷ  1����
//(0X00*256+0X05)/100=0.05  mg
uint8_t DealHCHOData(uint8_t *buf)
{
//	if((*buf==0XA5) && (*(buf+1)==0X5A))
//	{
//		if(*(buf+8) == 0XAA)
//		{
//			SesorData[0].Jiaquanmg =*(buf+4)*256 + *(buf+5);       //mgֵ��ȩ�������㣬����û�л��������ʵ��ֵ��ʵ��ֵҪ��100
//			SesorData[0].Jiaquanppm = *(buf+6)*256 + *(buf+7);     //ppmֵ
//		}
//		else
//		{
//			SesorData[0].Jiaquanmg = 0;
//			SesorData[0].Jiaquanppm = 0;
//			return 1;
//		}
//	}
//	else
//	{
//		SesorData[0].Jiaquanmg = 0;
//		SesorData[0].Jiaquanppm = 0;
//		return 1;
//	} 
//	memset((void *)JQRec[0],10,0);  //��ջ�����
	return 0;  
}
//���ͼ�ȩ���ݶ�ȡ����
//֡��ʽ��0XA5  0X5A 0X02 0X80 0XAA
void SendJQCmd(uint8_t const *buf)
{
  uint8_t i;
  for(i=0;i<5;i++)
  {    
	while((UART4->SR & 0x0040)==0x0000); 		 //�ȴ��������
	UART4->DR = *(buf+i); 					 //д������
	delay_ms(10);  
  }
}
uint8_t FindPkgHead(uint8_t *buf)
{
   uint8_t i;
   for(i=0;i<jQRecSta;i++)
   {
     if((*(buf+i)==0XA5)&&(*(buf+i+1)==0X5A))
     {
          //if(*(buf+i+8)==0XAA)
          if(jQRecSta==(i+9))
          {
             return i;
          }
     }
   }
   return 0XFF;
}
//����4�жϷ��������ڼ�ȩ���ݽ���
//0XA5  0X5A 0X06 0X80 0Xxx  0Xxx 0Xxx  0Xxx 0XAA
//   ��ʼ    ����  ָ��   ��ȩmg     ��ȩppm  ����
void UART4_IRQHandler(void)                	//����1�жϷ������
{
    uint8_t Inchar =0XFF;
    uint8_t Idx = 0;
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.�����жϱ���
	  OSIntEnter();    
#endif	
    if(UART4->SR&(1<<5))  //����ǽ��յ��ж�
    {
      Inchar = UART4->DR;
      if(jQRecSta<15)
      {        
        JQRec[jQRecSta] = Inchar;        //��������  
        jQRecSta++;   //�����ֽڽ���
      }
      else
      {
        jQRecSta=0;
        memset((void *)&JQRec[0],10,0);  //��ջ����� 
      }            
      Idx = FindPkgHead(JQRec);      //֡�ж�,Ѱ��֡ͷ
      if(Idx!=0XFF)
      {
            //�������ݰ�����
            JQDataDealRes = DealHCHOData(&JQRec[Idx]);  //������   
            if(JQDataDealRes == 0)
               JQDataDealRes = 1;  //������ȷ����
            else if(JQDataDealRes == 1)
               JQDataDealRes = 0;  //����δ��ȷ�������
            jQRecSta=0;     //���½���
      }     
      if(jQRecSta>15)
      {
        jQRecSta = 0; 
        memset((void *)&JQRec[0],10,0);  //��ջ����� 
      }
   }
#ifdef OS_TICKS_PER_SEC	 	
	OSIntExit();  					//�˳��жϱ���				 		 
#endif	
}

//#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
//	OSIntExit();  											 
//#endif
//#endif
/*���ڷ������ݺ���
SendPacket:Ҫ���͵����ݰ�
Len:���������С*/
void UartSendText(char *text,uint8_t uartNum)
{
	uint8_t i;
	uint8_t Len;
	char *p;
	p = text;
	PB11ConfigUart(UartMode);
	Len=strlen(text);
	switch(uartNum)
	{
		case 1:
		{
		  for(i=0;i<Len;i++)
		  {                          			  
				while((USART1->SR & 0x0040)==0x0000);//�ȴ����ڷ������ 			  
				USART1->DR = *p++;//ָ�����飬���ݰ�   
		  }
		}break;
		case 3:
		{
		  for(i=0;i<Len;i++)
		  {		   			  
				while((USART3->SR & 0x0040)==0x0000);
				USART3->DR = *p++;//ָ�����飬���ݰ�
		  }  
		}break;
		case 4:
		{
		  for(i=0;i<Len;i++)
		  {               			   
			   while((UART4->SR & 0x0040)==0x0000);  //�ȴ��������
			   UART4->DR = *p++;  //д������ 
		  }
		}break;
        default:break;
//      SendPacket=Point;//ָ������ָ����ʼ��
	}
    PB11ConfigUart(TFTMode);  
}
//���ڷ���ʮ����������
void UsartSendDex(uint8_t Uartnum,uint8_t data)
{
	switch(Uartnum)
	{
		case 1:
		{
			while((USART1->SR&0X40)==0);
			USART1->DR = data;
		}break;
		case 3:
		{
			while((USART3->SR&0X40)==0);
			USART3->DR = data;
		}break;
		default:break;
	}
}
//���ڻ���������
void ClearBuf(void)
{
	uint8_t i;
	for(i=0;i<USART_REC_LEN-1;i++)
		UartRecBuf[i]= ' ';
	UartRecBuf[USART_REC_LEN-1]= '\0';
	Step=0;
	Step3=0;
}
//������ݻ�����
void ClearDataBuffer(uint8_t *Data)
{
   memset((void *)Data,sizeof(Data),0);
}





