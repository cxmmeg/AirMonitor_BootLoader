#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "rtc.h" 		    
//////////////////////////////////////////////////////////////////////////////////	 								  	   
_calendar_obj calendar;//ʱ�ӽṹ�� 	
//�ж����ȼ�����
static void RTC_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;		//RTCȫ���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�1λ,�����ȼ�3λ
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//��ռ���ȼ�0λ,�����ȼ�4λ
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//ʹ�ܸ�ͨ���ж�
	NVIC_Init(&NVIC_InitStructure);		//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}
//ʵʱʱ������
//��ʼ��RTCʱ��,ͬʱ���ʱ���Ƿ�������
//BKP->DR1���ڱ����Ƿ��һ�����õ�����
//����0:����
//����:�������
uint8_t RTC_init(void)
{
	//����ǲ��ǵ�һ������ʱ��
	uint8_t temp=0;
	if(BKP->DR1!=0X5050)//��һ������
	{	 
	  	RCC->APB1ENR|=1<<28;     //ʹ�ܵ�Դʱ��	    
		RCC->APB1ENR|=1<<27;     //ʹ�ܱ���ʱ��	    
		PWR->CR|=1<<8;           //ȡ��������д����
		RCC->BDCR|=1<<16;        //����������λ	   
		RCC->BDCR&=~(1<<16);     //����������λ����	  	 
	    RCC->BDCR|=1<<0;         //�����ⲿ�������� 
	    while((!(RCC->BDCR&0X02))&&temp<250)//�ȴ��ⲿʱ�Ӿ���	 
		{
			temp++;
			delay_ms(10);
		};
		if(temp>=250)
		{
			LCD_ShowString(50,50,200,16,16,"RTC_init Failed!",GREEN,BLACK);
			return 1;//��ʼ��ʱ��ʧ��,����������	 
		}			

		RCC->BDCR|=1<<8; //LSI��ΪRTCʱ�� 	    
		RCC->BDCR|=1<<15;//RTCʱ��ʹ��	  
	  	while(!(RTC->CRL&(1<<5)));//�ȴ�RTC�Ĵ����������	 
    	while(!(RTC->CRL&(1<<3)));//�ȴ�RTC�Ĵ���ͬ��  
    	RTC->CRH|=0X01;  		  //�������ж�
    	while(!(RTC->CRL&(1<<5)));//�ȴ�RTC�Ĵ����������	 
		RTC->CRL|=1<<4;              //��������	 
		 
		RTC->PRLH=0X0000;
		RTC->PRLL=32767;          //ʱ����������(�д��۲�,���Ƿ�������?)����ֵ��32767										 
											    
		RTC_Set(2017,1,8,0,0,0);  //����ʱ��	  
		RTC->CRL&=~(1<<4);           //���ø���
		while(!(RTC->CRL&(1<<5)));   //�ȴ�RTC�Ĵ����������		 									  
		BKP->DR1=0X5050;  
	}
	else//ϵͳ������ʱ
	{
    	while(!(RTC->CRL&(1<<3)));//�ȴ�RTC�Ĵ���ͬ��  
    	RTC->CRH|=0X01;  		  //�������ж�
    	while(!(RTC->CRL&(1<<5)));//�ȴ�RTC�Ĵ����������
	}		    				  
	 RTC_NVIC_Config();
	RTC_Get();//����ʱ�� 
	return 0; //ok
}		 				    
//RTCʱ���ж�
//ÿ�봥��һ��  	 
void RTC_IRQHandler(void)
{		 
	if(RTC->CRL&0x0001)//�����ж�
	{							
		RTC_Get();//����ʱ��   
		//printf("sec:%d\r\n",calendar.sec);
 	}
	if(RTC->CRL&0x0002)//�����ж�
	{
		RTC->CRL&=~(0x0002);		//�������ж�	  
  		//printf("Alarm!\n");		   
  	} 				  								 
    RTC->CRL&=0X0FFA;         //�������������жϱ�־
	while(!(RTC->CRL&(1<<5)));//�ȴ�RTC�Ĵ����������	  	    						 	   	 
}
//�ж��Ƿ������꺯��
//�·�   1  2  3  4  5  6  7  8  9  10 11 12
//����   31 29 31 30 31 30 31 31 30 31 30 31
//������ 31 28 31 30 31 30 31 31 30 31 30 31
//year:���
//����ֵ:������ǲ�������.1,��.0,����
uint8_t Is_Leap_Year(uint16_t year)
{			  
	if(year%4==0) //�����ܱ�4����
	{ 
		if(year%100==0) 
		{ 
			if(year%400==0)return 1;//�����00��β,��Ҫ�ܱ�400���� 	   
			else return 0;   
		}else return 1;   
	}else return 0;	
}	 			   
//����ʱ��
//�������ʱ��ת��Ϊ����
//��1970��1��1��Ϊ��׼
//1970~2099��Ϊ�Ϸ����
//����ֵ:0,�ɹ�;����:�������.
//�·����ݱ�											 
uint8_t const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //���������ݱ�	  
//ƽ����·����ڱ�
const uint8_t mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
//syear,smon,sday,hour,min,sec��������ʱ����
//����ֵ�����ý����0���ɹ���1��ʧ�ܡ�
uint8_t RTC_Set(uint16_t syear,uint8_t smon,uint8_t sday,uint8_t hour,uint8_t min,uint8_t sec)
{
	uint16_t t;
	uint32_t seccount=0;
	if(syear<1970||syear>2099)return 1;	   
	for(t=1970;t<syear;t++)	//��������ݵ��������
	{
		if(Is_Leap_Year(t))seccount+=31622400;//�����������
		else seccount+=31536000;			  //ƽ���������
	}
	smon-=1;
	for(t=0;t<smon;t++)	   //��ǰ���·ݵ����������
	{
		seccount+=(uint32_t)mon_table[t]*86400;//�·����������
		if(Is_Leap_Year(syear)&&t==1)seccount+=86400;//����2�·�����һ���������	   
	}
	seccount+=(uint32_t)(sday-1)*86400;//��ǰ�����ڵ���������� 
	seccount+=(uint32_t)hour*3600;//Сʱ������
    seccount+=(uint32_t)min*60;	 //����������
	seccount+=sec;//�������Ӽ���ȥ
													    
	//����ʱ��
    RCC->APB1ENR|=1<<28;//ʹ�ܵ�Դʱ��
    RCC->APB1ENR|=1<<27;//ʹ�ܱ���ʱ��
	PWR->CR|=1<<8;    //ȡ��������д����
	//���������Ǳ����!
	RTC->CRL|=1<<4;   //�������� 
	RTC->CNTL=seccount&0xffff;
	RTC->CNTH=seccount>>16;
	RTC->CRL&=~(1<<4);//���ø���
	while(!(RTC->CRL&(1<<5)));//�ȴ�RTC�Ĵ���������� 
	RTC_Get();//������֮�����һ������ 	
	return 0;	    
}
//�õ���ǰ��ʱ�䣬���������calendar�ṹ������
//����ֵ:0,�ɹ�;����:�������.
uint8_t RTC_Get(void)
{
	static uint16_t daycnt=0;
	uint32_t timecount=0; 
	uint32_t temp=0;
	uint16_t temp1=0;	  
 	timecount=RTC->CNTH;//�õ��������е�ֵ(������)
	timecount<<=16;
	timecount+=RTC->CNTL;			 

 	temp=timecount/86400;   //�õ�����(��������Ӧ��)
	if(daycnt!=temp)//����һ����
	{	  
		daycnt=temp;
		temp1=1970;	//��1970�꿪ʼ
		while(temp>=365)
		{				 
			if(Is_Leap_Year(temp1))//������
			{
				if(temp>=366)temp-=366;//�����������
				else break;  
			}
			else temp-=365;	  //ƽ�� 
			temp1++;  
		}   
		calendar.w_year=temp1;//�õ����
		temp1=0;
		while(temp>=28)//������һ����
		{
			if(Is_Leap_Year(calendar.w_year)&&temp1==1)//�����ǲ�������/2�·�
			{
				if(temp>=29)temp-=29;//�����������
				else break; 
			}
			else 
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];//ƽ��
				else break;
			}
			temp1++;  
		}
		calendar.w_month=temp1+1;	//�õ��·�
		calendar.w_date=temp+1;  	//�õ����� 
	}
	temp=timecount%86400;     		//�õ�������   	   
	calendar.hour=temp/3600;     	//Сʱ
	calendar.min=(temp%3600)/60; 	//����	
	calendar.sec=(temp%3600)%60; 	//����
	calendar.week=RTC_Get_Week(calendar.w_year,calendar.w_month,calendar.w_date);//��ȡ����   
	return 0;
}	 
//������������ڼ�
//��������:���빫�����ڵõ�����(ֻ����1901-2099��)
//year,month,day������������ 
//����ֵ�����ں�																						 
uint8_t RTC_Get_Week(uint16_t year,uint8_t month,uint8_t day)
{	
	uint16_t temp2;
	uint8_t yearH,yearL;
	
	yearH=year/100;	yearL=year%100; 
	// ���Ϊ21����,�������100  
	if (yearH>19)yearL+=100;
	// ����������ֻ��1900��֮���  
	temp2=yearL+yearL/4;
	temp2=temp2%7; 
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	return(temp2%7);
}			  



