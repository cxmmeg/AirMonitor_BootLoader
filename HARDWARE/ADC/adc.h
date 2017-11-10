#ifndef __ADC_H
#define __ADC_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////
#define THRESHOLD   5  //�������仯��ֵ
#define PM2_5Ctrl  PCout(3)
extern uint8_t const ChalTab[4];  //Ҫת����ADC��ͨ��
extern uint8_t const JQCmd[5];
extern uint8_t const JQOFFSET[5];
//���������ݽṹ��
typedef struct  SENSOR
{		
	uint32_t Result_UPres;//��ѹ
	uint16_t Jiaquanmg;    //��ȩ����
	uint16_t Jiaquanppm;
	struct ANOSENOR   //ADCģ�⴫�������ݽṹ
	{
		uint16_t LightStren;  //��ǿ
		uint16_t PM2_5;			//PM2.5
		uint16_t FireGas;		//��ȼ����
		uint16_t BatVol;		//��ص�ѹ
	}AnoSenor;		
	uint8_t Temp;//�¶�����
	uint8_t Damp;//ʪ��  
}Sensor;
extern Sensor SesorData[2];
extern uint8_t const ThreVal[7];
void Adc_Init(void);
void ADC_DMAInit(DMA_Channel_TypeDef* DMA_CHx,u32 PeripAdd,u32 MemAdd,uint16_t BufLen);

uint16_t Get_Adc(uint8_t ch);
uint16_t Get_Adc_Average(uint8_t ch,uint8_t times);
uint16_t Pm2_5Read(uint8_t ch,uint8_t times);
uint8_t SensorDtaChageCheck(Sensor *Sensor);
#endif 


