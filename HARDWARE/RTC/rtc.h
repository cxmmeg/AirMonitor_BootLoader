#ifndef __RTC_H
#define __RTC_H	    
#include "include.h"

//ʱ��ṹ��
typedef struct 
{
	vuint8_t hour;
	vuint8_t min;
	vuint8_t sec;			
	//������������
	vuint16_t w_year;
	vuint8_t  w_month;
	vuint8_t  w_date;
	vuint8_t  week;		 
}_calendar_obj;					 
extern _calendar_obj calendar;	//�����ṹ��
												    
void Disp_Time(uint8_t x,uint8_t y,uint8_t size);			//���ƶ�λ�ÿ�ʼ��ʾʱ��
void Disp_Week(uint8_t x,uint8_t y,uint8_t size,uint8_t lang);	//��ָ��λ����ʾ����
uint8_t RTC_init(void);        //��ʼ��RTC,����0,ʧ��;1,�ɹ�;
uint8_t Is_Leap_Year(uint16_t year);//ƽ��,�����ж�
uint8_t RTC_Get(void);         //����ʱ��   
uint8_t RTC_Get_Week(uint16_t year,uint8_t month,uint8_t day);
uint8_t RTC_Set(uint16_t syear,uint8_t smon,uint8_t sday,uint8_t hour,uint8_t min,uint8_t sec);//����ʱ��			
static void RTC_NVIC_Config(void);
#endif




























 
