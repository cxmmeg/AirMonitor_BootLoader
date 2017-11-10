#ifndef _UARTSCREEN_H
#define _UARTSCREEN_H
#include "include.h"
#pragma pack(1)
/*����֡�ṹ*/
typedef struct SENDPKG
{
   uint8_t  head;        //EE  ֡ͷ
   uint8_t  FrameId[4];  //B1 10 00 00  �����л�ָ�� 
   uint8_t  WidgetId[2];   //00 00 �ؼ�Id
   uint8_t  Data[4];     //��������
   uint8_t  Tail[4];     //֡β
}SendPkg;
typedef struct DATADEAL
{
   uint8_t Temp[4];  //�¶�
   uint8_t Damp[4];  //ʪ��
   uint8_t JiaQuan_mg[4];  //��ȩ
   uint8_t JiaQuan_ppm[4]; //��ȩ
   uint8_t Presure[4];    //��ѹ
   uint8_t AnoSensor[4][4];  //��·ģ�⴫����
}DataDeal;

//ϵͳ״ָ̬�����ݽṹ
typedef struct SCRESTAT
{
   uint8_t Frame[4];  //��Ļ����
   uint8_t aWake[5];  //�����¼�
}ScreStat;
extern SendPkg SendSensorData;  //���ʹ���������
extern DataDeal TrnasfData;   //��������������
void UartScreenInit(void);
void aWake(uint16_t FrameId,uint8_t Enable,uint8_t BL_ON,uint8_t BL_OFF,uint8_t BL_ON_Time);

void SendDataToScreen(uint16_t FrameId,uint16_t WidgetId,uint8_t *DataBuf);
void DataTransf(void);
void ScreenDispData(void);
#endif


























