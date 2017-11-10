#ifndef _UARTSCREEN_C
#define _UARTSCREEN_C	
//��������������Դ�ļ�
#include "include.h"
//////////////////��������������////////////////////////////////////////////////////////////////////////
//��ʾ����������֡���ֳ�ʼ��
DataDeal TrnasfData;    //����������ת��
SendPkg SendSensorData;   //���ʹ���������
ScreStat SetOraWake;      //���ô������Ƿ�Ϊ��ʾģʽ
//�����﷨�β���
/*************************************
struct Test
{
	uint8_t i;
	uint8_t j;
	uint16_t k;
};
void TestCode(struct Test *test)
{
	test->i = 1;
	test->j = 2;
	test->k = 3;
}************************************/
//��������س�ʼ��
void UartScreenInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 	
       
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB,GPIO_Pin_6); 
}
//�������ݵ�������
//����Id ,�ؼ�Id
//Ҫ���͵�����
void SendDataToScreen(uint16_t FrameId,uint16_t WidgetId,uint8_t *DataBuf)
{
  uint8_t i;
  uint8_t len;
  uint8_t *buf;
//  Data&=0XFFFF;
  SendSensorData.head = 0XEE;   //֡ͷ
  SendSensorData.FrameId[0] = 0XB1;
  SendSensorData.FrameId[1] = 0X10; 
  SendSensorData.FrameId[2] = (FrameId>>8)&0XFF;   //�õ��߰�λ
  SendSensorData.FrameId[3] = FrameId&0XFF;        //�õ��Ͱ�λ 
  SendSensorData.WidgetId[0] = (WidgetId>>8)&0XFF;  //�õ��ؼ���Id��
  SendSensorData.WidgetId[1] = WidgetId&0XFF;
  ///////////////////////////////////////////
  for(i=0;i<4;i++)
    SendSensorData.Data[i] = *(DataBuf+i);
  /////////////// ////////////////////////////
  SendSensorData.Tail[0] = 0XFF;
  SendSensorData.Tail[1] = 0XFC;
  SendSensorData.Tail[2] = 0XFF;
  SendSensorData.Tail[3] = 0XFF;
  len = sizeof(SendSensorData);
  buf = (uint8_t*)(&SendSensorData.head);
  for(i=0;i<len;i++)
     UsartSendDex(3,*(buf+i));
}
//����ת��
void DataTransf(void)
{
    uint8_t i;
    uint8_t ThousBit,HundreBit,TenBit,IndiBit;  //ǧ �� ʮ �� λ
    //����������ת��
    SendJQCmd(JQCmd);  //���ͼ�ȩ��ȡ��������
    delay_ms(50);
    PM2_5Ctrl = 1;
    SesorData[0].AnoSenor.FireGas = Get_Adc_Average(ChalTab[0],5);  	//ģ�������������ݼ���(�Ѿ��˲���)
	SesorData[0].AnoSenor.LightStren = Get_Adc_Average(ChalTab[1],5);
	SesorData[0].AnoSenor.PM2_5 = Get_Adc_Average(ChalTab[2],5);
    PM2_5Ctrl = 0;
    DHT11_Read_Data(&SesorData[0].Temp,&SesorData[0].Damp); //�¶ȴ�������ȡ(�˲��������)	
//    bmp180Convert(&SesorData[0].Result_UPres);        //��ѹ��������ȡ(����Ҫ�˲�)

    SesorData[0].Temp = SesorData[0].Temp;//�¶�
    SesorData[0].Damp = SesorData[0].Damp;//ʪ��
    SesorData[0].Result_UPres = SesorData[0].Result_UPres;     //��ѹ
    SesorData[0].Jiaquanmg = SesorData[0].Jiaquanmg;           //��ȩmg
    SesorData[0].Jiaquanppm = SesorData[0].Jiaquanppm;         //��ȩppm
    SesorData[0].AnoSenor.FireGas = SesorData[0].AnoSenor.FireGas/20;     //ͨ��0���ݽ��㣨��ȼ�����壩0-4095������41��ʾת��Ϊ0-100
    SesorData[0].AnoSenor.LightStren = SesorData[0].AnoSenor.LightStren;     //ͨ��1���ݽ��㣨���մ�������0-4095
    SesorData[0].AnoSenor.PM2_5= SesorData[0].AnoSenor.PM2_5/60;     //ͨ��2���ݽ��㣨PM2.5��     0-4095
    SesorData[0].AnoSenor.BatVol = SesorData[0].AnoSenor.BatVol;    //ͨ��3���ݽ��㣨����-PHˮ�ʼ�⣩0-4095��ת��Ϊ0-15��Χ֮��
//	  printf("�¶�Ϊ :%d\r\n",SensorData.Temp);
//		printf("ʪ��Ϊ :%d\r\n",SensorData.Damp);
//		printf("��ѹΪ :%d\r\n",SensorData.Result_UPres);
//		printf("��ȩmΪ :%d\r\n",SensorData.Jiaquanmg);
//		printf("��ȩppmΪ :%d\r\n",SensorData.Jiaquanppm);
//		printf("ģ����1Ϊ :%d\r\n",SensorData.ADC_ChangeValue[0]);
//		printf("ģ����2Ϊ :%d\r\n",SensorData.ADC_ChangeValue[1]);	
//		printf("ģ����3Ϊ :%d\r\n",SensorData.ADC_ChangeValue[2]);
//		printf("ģ����4Ϊ :%d\r\n",SensorData.ADC_ChangeValue[3]);
		//    //�¶�
    TenBit = SesorData[0].Temp/10%10%16;
    IndiBit = SesorData[0].Temp%10%16;
    TrnasfData.Temp[0] = 0X30|TenBit;
    TrnasfData.Temp[1] = 0X30|IndiBit; 
    //ʪ��
    TenBit = SesorData[0].Damp/10%10%16;
    IndiBit = SesorData[0].Damp%10%16;
    TrnasfData.Damp[0] = 0X30|TenBit;
    TrnasfData.Damp[1] = 0X30|IndiBit;
    //��ȩmg   
    TenBit = SesorData[0].Jiaquanmg/10%10%16;
    IndiBit = SesorData[0].Jiaquanmg%10%16;
    TrnasfData.JiaQuan_mg[0] = 0X30|TenBit;
    TrnasfData.JiaQuan_mg[1] = 0X30|IndiBit;
    //��ȩPPM   
    TenBit = SesorData[0].Jiaquanppm/10%10%16;
    IndiBit = SesorData[0].Jiaquanppm%10%16;
    TrnasfData.JiaQuan_ppm[0] = 0X30|TenBit;
    TrnasfData.JiaQuan_ppm[1] = 0X30|IndiBit;
    //��ѹ
    SesorData[0].Result_UPres = SesorData[0].Result_UPres/1000;  //ת��ΪKPa
    HundreBit = SesorData[0].Result_UPres/100%10%16;
    TenBit = SesorData[0].Result_UPres/10%10%16;
    IndiBit = SesorData[0].Result_UPres%10%16;
    TrnasfData.Presure[0] = 0X30|HundreBit;
    TrnasfData.Presure[1] = 0X30|TenBit;
    TrnasfData.Presure[2] = 0X30|IndiBit;
   
    for(i=0;i<4;i++)  //ѭ��ת�洫��������������
    {
        SesorData[0].AnoSenor.FireGas = SesorData[0].AnoSenor.FireGas%10000;      //�ָ�����õ�����
        ThousBit = SesorData[0].AnoSenor.FireGas/1000%10%16;   //����λ����ת��Ϊʮ�����Ƶ�ֵ
        HundreBit = SesorData[0].AnoSenor.LightStren/100%10%16;
        TenBit = SesorData[0].AnoSenor.LightStren/10%10%16;
        IndiBit = SesorData[0].AnoSenor.PM2_5%10%16;
        TrnasfData.AnoSensor[i][0] = 0x30|ThousBit;        //�����ں������������ݽṹ
        TrnasfData.AnoSensor[i][1] = 0x30|HundreBit;
        TrnasfData.AnoSensor[i][2] = 0x30|TenBit;
        TrnasfData.AnoSensor[i][3] = 0x30|IndiBit;
   }
}
//��������Ļ����ʾ��ȡ�������
void ScreenDispData(void)
{
   DataTransf();   //����������ת��
   SendDataToScreen(0,37,TrnasfData.Temp);  //���ʹ���ID���ؼ�ID���� �¶�
   SendDataToScreen(0,38,TrnasfData.Damp);  //ʪ��
   SendDataToScreen(0,40,TrnasfData.JiaQuan_mg);//��ȩmg
   SendDataToScreen(0,41,TrnasfData.JiaQuan_ppm);//��ȩppm
   SendDataToScreen(0,43,TrnasfData.Presure);      //��ѹ
   SendDataToScreen(0,44,TrnasfData.AnoSensor[0]); //��ȼ����
   SendDataToScreen(0,39,TrnasfData.AnoSensor[1]); //����                       
   SendDataToScreen(0,42,TrnasfData.AnoSensor[2]); //PM2.5
   SendDataToScreen(0,45,TrnasfData.AnoSensor[3]); //����.ˮ��
}
//�Զ�����ģʽ
//����������л������Id��  ���Ѵ�����
//{0XEE, 0XB1, 0X00, 0X00, 0X00, 0XFF, 0XFC, 0XFF, 0XFF}  �����л�
//EE [77 Enable BL_ON BL_OFF BL_ON_Time] FF FC FF FF  �Զ�����ģʽ
void aWake(uint16_t FrameId,uint8_t Enable,uint8_t BL_ON,uint8_t BL_OFF,uint8_t BL_ON_Time)
{
   uint8_t i;
   SendSensorData.head = 0XEE;  //����֡ͷ
   UsartSendDex(3,SendSensorData.head);
   SetOraWake.aWake[0] = 0X77;
   SetOraWake.aWake[1] = Enable;
   SetOraWake.aWake[2] = BL_ON;
   SetOraWake.aWake[3] = BL_OFF;
   SetOraWake.aWake[4] = BL_ON_Time;
   for(i=0;i<4;i++)
      UsartSendDex(3,SetOraWake.aWake[i]);     //���Ϳ�����������
   SendSensorData.Tail[0] = 0XFF;
   SendSensorData.Tail[1] = 0XFC;
   SendSensorData.Tail[2] = 0XFF;
   SendSensorData.Tail[3] = 0XFF;
   for(i=0;i<4;i++)
      UsartSendDex(3,SendSensorData.Tail[i]);  //����֡β
   UsartSendDex(3,SendSensorData.head);        
   SetOraWake.Frame[0] =  0XB1;                //��������
   SetOraWake.Frame[1] =  0X00;
   SetOraWake.Frame[2] =  (FrameId>>8)&0XFF;   //ȡ�ø߰�λ
   SetOraWake.Frame[3] =  FrameId&0XFF;        //ȡ�õͰ�λ
   for(i=0;i<4;i++)     //ͨ�����ڷ�������
      UsartSendDex(3,SetOraWake.Frame[i]);  //�����л�����ָ�� 
   for(i=0;i<4;i++)
      UsartSendDex(3,SendSensorData.Tail[i]);  //����֡β  
}
//ʱ�任��
/*void TimeCalcu(void)
{
	 uint8_t i;
	 uint8_t shiwei,gewei;
	 RTC_Get();  //�õ���ǰ��ʱ��
   //if(Rem1Time)  //���ʹ����ʱ�������־
	 {
			 if(MainFacePar.Pre1Sec != calendar.sec)
			 {
				 MainFacePar.Pre1Sec = calendar.sec;
				 MainFacePar.Rem1Sec++;
				 if(MainFacePar.Rem1Sec==60)
				 {
					 MainFacePar.Rem1Sec=0;
					 MainFacePar.Rem1Min++;
					 if(MainFacePar.Rem1Min==60)
					 {
							MainFacePar.Rem1Min = 0;
							MainFacePar.Rem1Hour++;
							if(MainFacePar.Rem1Hour==60)
								MainFacePar.Rem1Hour=0;
					 }
				 }
			 }
   }
	 shiwei = MainFacePar.Rem1Min/10%16;
	 gewei = MainFacePar.Rem1Min%10%16;
	 Fen[7] = 0x30|shiwei;
	 Fen[8] = 0x30|gewei;
	 //�뻻��
	 shiwei = MainFacePar.Rem1Sec/10%16;
	 gewei = MainFacePar.Rem1Sec%10%16;
	 Miao[7] = 0x30|shiwei;
	 Miao[8] = 0x30|gewei;
	 for(i=0;i<13;i++)
	 {
			UsartSendDex(3,Fen[i]);  //���ͷ�������
		  //delay_ms(2);
	 }
	 for(i=0;i<13;i++)
	 {
			UsartSendDex(3,Miao[i]); //����������
		  //delay_ms(2);
	 }
}  */





#endif




