#include "include.h"
#define FillColor  GRAY  //�ұ�����ĵ�ɫ
//#include <locale.h>
//AQI�����
const float AQITab[7][4]={

	{0,15.4,0,50},
	{15.5,40.4,51,100},
	{40.5,65.4,101,150},
	{60.5,150.4,151,200},
	{150.5,250.4,201,300},
	{250.5,350.4,301,400},
	{350.5,500.4,401,500}	
};

extern const unsigned char WifiLogo[32];
extern const unsigned char NoWifiLogo[32];
uint8_t orLoadUI[2] = {TRUE,FALSE};
uint8_t const Item_Id[7] = {HCHO_ID,PM2_5_ID,DangerGas_ID,Temp_ID,Dump_ID,UPres_ID,Illumn_ID};
unsigned char const *ItemName[] = {"�¶�","ʪ��","�ӷ�","��ѹ","�к�","��ǿ","PM2.5"};
uint8_t const *QualityGrade[]={"��","��","��","��","��","��","AQI"};
uint16_t const ColorTab[] = {GREEN,LIGHTGREEN,RED,BLUE,BROWN,LIGHTBLUE,YELLOW};//��ɫֵ
uint16_t const ValueTab[] = {0,35,75,115,150,250,500};//ɫ���ϵ���ֵ
volatile uint8_t CurrentuiID =1;	//��ǰUI��ID���
uint8_t ToDataSDCard_WriteFlag = 0;  //��SD��д�����ݱ�־
uint8_t Wave_Id = 0;
uint8_t PreWave_Id = 0XFF;

uint16_t R_init,G_init,B_init;   //������ʾ��ʱ�����ɫ״̬

DispCirDef const AllDispCirInfo[7]=
{      //���ﳤ�Ϳ�����Բ����Բ����
   //x0  y0 r�� r�� XoffYoff C1clr�� C2clr�� Valuecolor Textcolor TextBackcolor ValueBackcolor Text_Name Text_UnitMeas
	{0,18,88,110,   22,  85,  {RED,DARKBLUE,   RED,  WHITE,   DARKBLUE,FillColor},	 "��ȩ","mg/m3",36},	
	{92,18,92,110,  27,  85,  {RED,GRAYBLUE,   RED,  WHITE,   DARKBLUE,FillColor},     "PM2.5","ug/m3",36},
	{189,18, 90,110,20,  85,  {RED,GRAYBLUE,   RED,  WHITE,   DARKBLUE,FillColor},    "��ȼ","mg/m3",36},	
	
	{28,168, 25,22,22,  28,  {WHITE,GRAYBLUE, WHITE,WHITE, BLACK,GRAYBLUE},  "�¶�(��)",    "deg",16},
	{95,168, 25,22,21,  28,  {WHITE,GRAYBLUE, WHITE,WHITE, BLACK,GRAYBLUE},  "ʪ��(%)",     "  %",16},	
	{160,168, 25,22,27, 28,  {WHITE,GRAYBLUE, WHITE,WHITE, BLACK,GRAYBLUE},  "��ѹ(KPa)",   "KPa",16},	
	{232,168,25,22,25,  28,  {WHITE,GRAYBLUE, WHITE,WHITE, BLACK,GRAYBLUE},  "����(Lux)",   "Lux",16},	
};
//�����������е����ֵ
uint16_t MAX(uint16_t val1,uint16_t val2)
{
	return (val1>val2)?val1:val2;
}
//�����������е���Сֵ
uint16_t MIN(uint16_t val1,uint16_t val2)
{
	return (val1>val2)?val2:val1;	
}
//��ֵ��Χ�ж�
uint16_t DeadScale(uint16_t val,uint16_t min,uint16_t max)
{
	if(val>min&&val<max)
		return val;
	else if(val<min)
		return min;
	else if(val>max)
		return max;
}
//DispCirDef *pCirInfo[7]={&AllDispCirInfo[Temp_ID],&AllDispCirInfo[Dump_ID],
//					  &AllDispCirInfo[HCHO_ID],&AllDispCirInfo[UPres_ID],
//					  &AllDispCirInfo[DangerGas_ID],&AllDispCirInfo[Illumn_ID],
//					  &AllDispCirInfo[PM2_5_ID]};
void TestPm2_5(void)
{
	PM2_5Ctrl = 1;	
	delay_ms(1);
	SesorData[0].AnoSenor.PM2_5 =  Pm2_5Read(ChalTab[2],2);
    PM2_5Ctrl = 0;
	delay_ms(7);
	LCD_ShowNum(0,100,SesorData[0].AnoSenor.PM2_5,5,24,BLACK,LIGHTBLUE); 
}
//���ش���������
void LoadSensorData(Sensor *sensordata)
{
	memset((void *)&SesorData[0],0,sizeof(SesorData[0]));
	//����������ת��
	bmp180Convert(&sensordata->Result_UPres);        //��ѹ��������ȡ(����Ҫ�˲�)
    SendJQCmd(JQCmd);  //���ͼ�ȩ��ȡ��������
    sensordata->AnoSenor.FireGas = Get_Adc_Average(ChalTab[0],10);  	//ģ�������������ݼ���(�Ѿ��˲���)
	sensordata->AnoSenor.LightStren = Get_Adc_Average(ChalTab[1],10)/2;
	PM2_5Ctrl = 1;	
	//delay_ms(1);
	delay_us(280);
	SesorData[0].AnoSenor.PM2_5 = Get_Adc(ChalTab[2]);  // Pm2_5Read(ChalTab[2],2);
	delay_us(40);
    PM2_5Ctrl = 0;
	delay_ms(7);
	DHT11_Read_Data(&sensordata->Temp,&sensordata->Damp); //�¶ȴ�������ȡ(�˲��������)	  
	sensordata->AnoSenor.FireGas = sensordata->AnoSenor.FireGas/41-5;  //��λ�����30Ϊ���ֵ
	sensordata->AnoSenor.LightStren = sensordata->AnoSenor.LightStren/58;
	sensordata->AnoSenor.PM2_5 = sensordata->AnoSenor.PM2_5*50*10/(4095*3.3);      //0.5V/0.1mg/m3 ��10Ϊ����
}
//
//����һ��ɫ��
//
void LoadColorPat(void)
{ 
	uint16_t i=0;
	uint16_t color;
	uint16_t Ri=0,Gi=0,Bi=0;
	uint16_t ystep = (lcddev.height-13)/6;  	//�ı�ɫ�׵ĳ��ȸ�����
	Show_Str(CURSOR-21,20,30,16,(uint8_t*)QualityGrade[6],16,0,BLACK,FillColor);
	Show_Str(CURSOR-21,225,18,16,(uint8_t*)QualityGrade[0],16,0,DARKBLUE,FillColor);
	Ri = 0;Gi = 0;Bi = 0;
	for(i=240;i>240-ystep;i--)  //�\����
	{
		color  = (unsigned long) Bi*1000/4047<<5;
		color |= (unsigned long) Gi*1000/8225;
		color |= (unsigned long) Ri*1000/8225<<11;
		LCD_DrawLine(310,i,320,i,color);
		Gi = Gi+6;
	}
	Show_Str(CURSOR-21,225-ystep,18,16,(uint8_t*)QualityGrade[1],16,0,color,FillColor);
	Ri = 0;Gi = 0;Bi = 240;
	for(i=240-ystep;i>240-ystep*2;i--)	//����ǳ��
	{
		color = (unsigned long) Gi*1000/8225<<11;
		color |= (unsigned long) Ri*1000/4047<<5;	  
		color |= (unsigned long) Bi*1000/8225;	
		LCD_DrawLine(310,i,320,i,color);
		Ri = Ri+6;
	}
	Show_Str(CURSOR-21,225-ystep*2,18,16,(uint8_t*)QualityGrade[2],16,0,color,FillColor);
	Ri = 0;Gi = 240;Bi = 240; 
	for(i=240-ystep*2;i>240-ystep*3;i--)   //ǳ������
	{
		color = (unsigned long) Ri*1000/8225<<11;			   	
		color |= (unsigned long) Bi*1000/4047<<5;	  
		color |= (unsigned long) Gi*1000/8225;
		LCD_DrawLine(310,i,320,i,color);
		Gi = Gi-6;
	}
	Show_Str(CURSOR-21,225-ystep*3,18,16,(uint8_t*)QualityGrade[3],16,0,color,FillColor);
	Ri = 0;Gi = 240;Bi = 0;
	for(i=240-ystep*3;i>240-ystep*4;i--)//�̵���
	{
		color  = (unsigned long) Gi*1000/4047<<5;
		color |= (unsigned long) Ri*1000/8225;		
		color |= (unsigned long) Bi*1000/8225<<11;
		LCD_DrawLine(310,i,320,i,color);		
		Bi = Bi+6;
	}
	Show_Str(CURSOR-21,225-ystep*4,18,16,(uint8_t*)QualityGrade[4],16,0,color,FillColor);
	Ri = 240;Gi = 0;Bi = 240;
	for(i=240-ystep*4;i>240-ystep*5;i--)//�Ƶ���
	{
		color = (unsigned long) Ri*1000/8225<<11;
		color |= (unsigned long) Bi*1000/4047<<5;	  
		color |= (unsigned long) Gi*1000/8225;
		LCD_DrawLine(310,i,320,i,color);
		Bi = Bi-6;
	}
	Show_Str(CURSOR-21,225-ystep*5,18,16,(uint8_t*)QualityGrade[5],16,0,color,FillColor);
	Ri = 240;Gi = 0;Bi = 0;
	for(i=240-ystep*5;i>240-ystep*6;i--)//�쵽��
	{
		color  = (unsigned long) Bi*1000/8225;
		color |= (unsigned long) Ri*1000/8225<<11;
		color |= (unsigned long) Gi*1000/4047<<5;
		LCD_DrawLine(310,i,320,i,color);
		Bi = Bi+6;
	}
	POINT_COLOR = RED;
}
//���ƶ�̬�ĺ�ɫָʾ��
void DrawDynamicCir(uint16_t x0,uint16_t y0,uint16_t r,uint16_t color)
{
	uint16_t static Prex0,Prey0;
	LCDDrawCir(Prex0,Prey0,r,FillColor,0,1);//�����һ��Բ
	LCDDrawCir(x0,y0,r,color,0,1);//�ػ���Բ
	Prex0 = x0;
	Prey0 = y0;
}
//����AQI��ֵ
uint16_t Cal_AQI_Value(uint16_t Cvalue)
{
	float AQIValue;
	float Ihigh,Ilow,Chigh,Clow;
	AQIValue = Cvalue/1000;
	if(Cvalue>AQITab[0][0]&&Cvalue<AQITab[0][1]){
		Clow = AQITab[0][0];Chigh = AQITab[0][1];Ilow =  AQITab[0][2];Ihigh = AQITab[0][3];}
	
	else if(Cvalue>AQITab[1][0]&&Cvalue<AQITab[1][1]){
		Clow = AQITab[1][0];Chigh = AQITab[1][1];Ilow =  AQITab[1][2];Ihigh = AQITab[1][3];}

	else if(Cvalue>AQITab[2][0]&&Cvalue<AQITab[2][1]){
		Clow = AQITab[2][0];Chigh = AQITab[2][1];Ilow =  AQITab[2][2];Ihigh = AQITab[2][3];}
		
	else if(Cvalue>AQITab[3][0]&&Cvalue<AQITab[3][1]){
		Clow = AQITab[3][0];Chigh = AQITab[3][1];Ilow =  AQITab[3][2];Ihigh = AQITab[3][3];}
		
	else if(Cvalue>AQITab[4][0]&&Cvalue<AQITab[4][1]){
		Clow = AQITab[4][0];Chigh = AQITab[4][1];Ilow =  AQITab[4][2];Ihigh = AQITab[4][3];}	
		
	else if(Cvalue>AQITab[5][0]&&Cvalue<AQITab[5][1]){
		Clow = AQITab[5][0];Chigh = AQITab[5][1];Ilow =  AQITab[5][2];Ihigh = AQITab[5][3];}
		
	else if(Cvalue>AQITab[6][0]&&Cvalue<AQITab[6][1]){
		Clow = AQITab[6][0];Chigh = AQITab[6][1];Ilow =  AQITab[6][2];Ihigh = AQITab[6][3];}	
		
		
	AQIValue = (uint16_t)((Ihigh - Ilow)*(Cvalue - Clow)/(Chigh - Clow) + Ilow);
	return AQIValue;
}
//��������жϣ������������϶�����ʾ
void EnvirQuaty(uint16_t x,uint16_t y)
{
	uint16_t y0;
	uint16_t AQIValue;
	AQIValue = (uint16_t)Cal_AQI_Value(SesorData[0].AnoSenor.PM2_5);
	Show_Str(0,215,300,24,"AQI:",24,0,BLUE,BLACK);
    LCD_ShowNum(45,y,AQIValue,3,24,RED,DARKBLUE);
	y0 = AQIValue/1.3;
	if(y0>219)
		y0 = 219;
	if(AQIValue<100)
	{	
		DrawDynamicCir(CURSOR,240-y0,5,RED);
		SetDriverPWM(0,0);
		if(SesorData[0].Jiaquanmg<5)
		{
			if(SesorData[0].AnoSenor.FireGas<50)
				Show_Str(x,y,180,24,"      ��������",24,0,GREEN,DARKBLUE);
			else if(SesorData[0].AnoSenor.FireGas>70)
				Show_Str(x,y,180,24,"      ȼ��й©",24,0,GREEN,DARKBLUE);
		}
		else if(SesorData[0].Jiaquanmg>8)
			Show_Str(x,y,180,24,"      ��ȩ����",24,0,GREEN,DARKBLUE);
	}
	else if(AQIValue>=100)
	{
		if(AQIValue>100&&AQIValue<200)
		{
			DrawDynamicCir(CURSOR,DeadScale(240-y0,17,120),5,RED);			
			SetDriverPWM(500,500);
			if(SesorData[0].Jiaquanmg<5)
			{
				if(SesorData[0].AnoSenor.FireGas<50)
					Show_Str(x,y,180,24,"      �ж���Ⱦ",24,0,BRRED,DARKBLUE);
				else if(SesorData[0].AnoSenor.FireGas>70)
					Show_Str(x,y,180,24,"      ȼ��й©",24,0,GREEN,DARKBLUE);
			}
			else if(SesorData[0].Jiaquanmg>8)
				Show_Str(x,y,180,24,"����,��ȩ����",24,0,GREEN,DARKBLUE);				
		}
		else 
		{
			DrawDynamicCir(CURSOR,DeadScale(240-y0,17,120),5,RED);
			SetDriverPWM(800,800);
			if(SesorData[0].Jiaquanmg<5)
			{
				if(SesorData[0].AnoSenor.FireGas<50)
					Show_Str(x,y,180,24,"      ��Ⱦ����",24,0,RED,DARKBLUE);
				else if(SesorData[0].AnoSenor.FireGas>70)
					Show_Str(x,y,180,24,"      ȼ��й©",24,0,GREEN,DARKBLUE);
			}
			else if(SesorData[0].Jiaquanmg>8)
				Show_Str(x,y,180,24,"����,��ȩ����",24,0,GREEN,DARKBLUE);					
		}
	}
}
void LoadShowUI(DispCirDef const *DispCirInfo,uint8_t Item)
{	
	switch(Item)
	{
		case Temp_ID:
	    case Dump_ID:
		case UPres_ID:
		case Illumn_ID:
			LCDDrawCir(DispCirInfo->x0,DispCirInfo->y0,DispCirInfo->r1,DispCirInfo->ColorInfo.Cir1color,0,1);  	 //��Բ
			LCDDrawCir(DispCirInfo->x0,DispCirInfo->y0,DispCirInfo->r2,DispCirInfo->ColorInfo.Cir2color,0,1);      //��Բ
			Show_Str(DispCirInfo->x0-DispCirInfo->Xoff,DispCirInfo->y0+DispCirInfo->Yoff,100,12,DispCirInfo->Text_Name,12,0,DispCirInfo->ColorInfo.Textcolor,DispCirInfo->ColorInfo.TextBackcolor);
			break;
		case HCHO_ID:
		case PM2_5_ID:
		case DangerGas_ID:
			LCD_Fill(DispCirInfo->x0,DispCirInfo->y0,DispCirInfo->x0+DispCirInfo->r1,DispCirInfo->y0+DispCirInfo->r2,DispCirInfo->ColorInfo.ValueBackcolor);
			Show_Str(DispCirInfo->x0+DispCirInfo->Xoff,DispCirInfo->y0+8,100,16,DispCirInfo->Text_Name,16,0,DispCirInfo->ColorInfo.Textcolor,DispCirInfo->ColorInfo.ValueBackcolor);
			Show_Str(DispCirInfo->x0+DispCirInfo->Xoff,DispCirInfo->y0+DispCirInfo->Yoff,100,16,DispCirInfo->Text_UnitMeas,16,0,DispCirInfo->ColorInfo.Textcolor,DispCirInfo->ColorInfo.ValueBackcolor);
		break;
	}
}
//������ɫ
uint16_t CalColorValue(uint16_t Value,uint8_t IdItem)
{
	uint16_t R,G,B;
	uint16_t RGB = 0x0000;
	switch(IdItem)
	{
		case HCHO_ID:
		{
			if(Value<5){
				R_init = 0;G_init = 0;B_init = 13;}
			else if(Value>=5&&Value<10){
				R_init = 0;G_init = 13;B_init = 0;}
			else{
				R_init = 13;G_init = 0;B_init = 0;}
				
		}break;
		case PM2_5_ID:
		{
			if(Value<16){
				R_init = 0;G_init = 0;B_init = 50;}
			else if(Value>=16&&Value<32){
				R_init = 0;G_init = 50;B_init = 0;}
			else{
				R_init = 50;G_init = 0;B_init = 0;}
		}break;
		case DangerGas_ID:
		{
			if(Value<30){
				R_init = 0;G_init = 0;B_init = 100;}
			else if(Value>=30&&Value<60){
				R_init = 0;G_init = 100;B_init = 0;}
			else{
				R_init = 100;G_init = 0;B_init = 0;}
		}break;
	}
	R=(uint16_t) Value*240/R_init;if(R>240)R=240;
	G=(uint16_t) Value*240/G_init;if(G>240)G=240;
	B=(uint16_t) Value*240/B_init;if(B>240)B=240;		
	RGB |=(unsigned long) R*1000/8225<<11;			   
	RGB |=(unsigned long) G*1000/4047<<5;	  //ת������Ļ��4Wɫ��ʾ
	RGB |=(unsigned long) B*1000/8225;
	return RGB;
}
//������ʾ���Բ��
void DispCirInfo(DispCirDef const *DispCirInfo,uint8_t Item)
{
	char buf[20];			
	uint16_t RGB;
	switch(Item)
	{
		case Temp_ID:
		{
			sprintf(buf,"%d",SesorData[0].Temp);
			Show_Str(DispCirInfo->x0-5,DispCirInfo->y0-10,35,DispCirInfo->FontSize,buf,DispCirInfo->FontSize,0,DispCirInfo->ColorInfo.Valuecolor,DispCirInfo->ColorInfo.ValueBackcolor);
		}break;
		case Dump_ID:
		{
			sprintf(buf,"%d%",SesorData[0].Damp);
			Show_Str(DispCirInfo->x0-5,DispCirInfo->y0-10,35,DispCirInfo->FontSize,buf,DispCirInfo->FontSize,0,DispCirInfo->ColorInfo.Valuecolor,DispCirInfo->ColorInfo.ValueBackcolor);
		}break;
		case UPres_ID:
		{ 
			sprintf(buf,"%d.%d%d",SesorData[0].Result_UPres/1000,SesorData[0].Result_UPres%1000/100,SesorData[0].Result_UPres%100/10);
			Show_Str(DispCirInfo->x0-20,DispCirInfo->y0-10,100,DispCirInfo->FontSize,buf,DispCirInfo->FontSize,0,DispCirInfo->ColorInfo.Valuecolor,DispCirInfo->ColorInfo.ValueBackcolor);
		}break;
		case HCHO_ID:
		{
			RGB = CalColorValue(SesorData[0].Jiaquanmg,HCHO_ID);
			sprintf(buf,"%d.%d%d",SesorData[0].Jiaquanmg/100,SesorData[0].Jiaquanmg%100/10,SesorData[0].Jiaquanmg%10);
			Show_Str(DispCirInfo->x0+5,DispCirInfo->y0+36,105,DispCirInfo->FontSize,buf,DispCirInfo->FontSize,0,RGB,DispCirInfo->ColorInfo.ValueBackcolor);
		}break;
		case DangerGas_ID:
		{
			RGB = CalColorValue(SesorData[0].AnoSenor.FireGas,DangerGas_ID);
			sprintf(buf,"%.2d",SesorData[0].AnoSenor.FireGas);
			Show_Str(DispCirInfo->x0+25,DispCirInfo->y0+36,100,DispCirInfo->FontSize,buf,DispCirInfo->FontSize,0,RGB,DispCirInfo->ColorInfo.ValueBackcolor);
		}break;
		case Illumn_ID:
		{
			sprintf(buf,"%d",SesorData[0].AnoSenor.LightStren);
			Show_Str(DispCirInfo->x0-5,DispCirInfo->y0-10,35,DispCirInfo->FontSize,buf,DispCirInfo->FontSize,0,DispCirInfo->ColorInfo.Valuecolor,DispCirInfo->ColorInfo.ValueBackcolor);
		}break;
		case PM2_5_ID:
		{			
			RGB = CalColorValue(SesorData[0].AnoSenor.PM2_5,PM2_5_ID);
			sprintf(buf,"%.3d",SesorData[0].AnoSenor.PM2_5*10);
			Show_Str(DispCirInfo->x0+21,DispCirInfo->y0+36,100,DispCirInfo->FontSize,buf,DispCirInfo->FontSize,0,RGB,DispCirInfo->ColorInfo.ValueBackcolor);
		}break;
		default:break;
	}
}
//����ʾ���ݵĽ���1
void LoadUI_1(void)
{
	uint8_t i;
	if(orLoadUI[1] != TRUE) //��һ�ε�
	{		
		//LCD_Clear(BLACK);//����
		LCD_Fill(0,0,CURSOR-12,240,BLACK);  //������ʾ����ĵ�ɫ
		LCD_Fill(CURSOR-21,19,320,240,FillColor);  //�ұߵĳ�����ɫ
		LoadColorPat();   //����ɫ�׺ͱ��
		for(i=0;i<7;i++)
		{
			LoadShowUI(&AllDispCirInfo[Item_Id[i]],Item_Id[i]);
		}
		LCD_DrawLine(0,210,CURSOR-22,210,BLUE);
		orLoadUI[1] = orLoadUI[0];
		orLoadUI[0] = FALSE;
	}
	else
	{
		for(i=0;i<7;i++)
		{
			DispCirInfo(&AllDispCirInfo[Item_Id[i]],Item_Id[i]);
		}
		EnvirQuaty(113,215);  //��������ж�
	}
}
//����ʾ���ݵĽ���2
//��ʾ���ı�����
void LoadUI_2(uint16_t x0,uint16_t y0,uint8_t ystep,uint8_t fontsize)
{
	uint8_t i = 0;
	char buf[20];
	if(orLoadUI[1] != TRUE) //��һ�ε�
	{		
		//LCD_Clear(BLACK);//����	
		LCD_Fill(0,0,CURSOR-13,209,BLACK);
		LoadColorPat();  //����ɫ��
		LCD_DrawLine(0,40,CURSOR-13,40,BLUE);
		LCD_DrawLine(0,145,CURSOR-13,145,BLUE);
		LCD_DrawLine(0,210,CURSOR-13,210,BLUE);
		orLoadUI[1] = orLoadUI[0];
		orLoadUI[0] = FALSE;
		//POINT_COLOR = RED;
		//BACK_COLOR = BLACK;
		//TestSDCard();
	}
	sprintf(buf,"�¶�: %d��",SesorData[0].Temp);
	Show_Str(x0,y0+13,80,16,buf,16,0,AllDispCirInfo[Temp_ID].ColorInfo.Textcolor,AllDispCirInfo[Temp_ID].ColorInfo.TextBackcolor);
	i++;
	sprintf(buf,"ʪ��: %d%%",SesorData[0].Damp);
	Show_Str(x0+135,y0+13,80,16,buf,16,0,AllDispCirInfo[Dump_ID].ColorInfo.Textcolor,AllDispCirInfo[Dump_ID].ColorInfo.TextBackcolor);
	i++;
	
	sprintf(buf,"HCHO:%d.%d%d",SesorData[0].Jiaquanmg/100,SesorData[0].Jiaquanmg%100/10,SesorData[0].Jiaquanmg%10);
	Show_Str(x0,y0+(i++)*ystep,280,AllDispCirInfo[HCHO_ID].FontSize,buf,AllDispCirInfo[HCHO_ID].FontSize,0,RED,BLACK);
	Show_Str(x0+188,y0+(--i)*ystep+16,80,16,"mg/m3",16,0,WHITE,BLACK);
	i++;	
	sprintf(buf,"Gas:%.3d",SesorData[0].AnoSenor.FireGas);
	Show_Str(x0,y0+(i++)*ystep+17,280,36,buf,36,0,RED,BLACK);
	Show_Str(x0+170,y0+(--i)*ystep+33,280,16,"mg/m3",16,0,WHITE,BLACK);
	i++;
	sprintf(buf,"PM2.5:%.3d",SesorData[0].AnoSenor.PM2_5*10);
	Show_Str(x0,y0+(i++)*ystep+32,280,36,buf,36,0,RED,BLACK);
	Show_Str(x0+188,y0+(--i)*ystep+48,280,16,"ug/m3",16,0,WHITE,BLACK);
	i++;
	
	sprintf(buf,"��ѹ:%d.%d%d KPa",SesorData[0].Result_UPres/1000,SesorData[0].Result_UPres%1000/100,SesorData[0].Result_UPres%100/10);
	Show_Str(x0,y0+(i++)*ystep+65,300,16,buf,16,0,AllDispCirInfo[UPres_ID].ColorInfo.Textcolor,AllDispCirInfo[UPres_ID].ColorInfo.TextBackcolor);	
	sprintf(buf,"��ǿ:%3d Lux",SesorData[0].AnoSenor.LightStren);
	Show_Str(x0+135,y0+(--i)*ystep+65,250,16,buf,16,0,AllDispCirInfo[Illumn_ID].ColorInfo.Textcolor,AllDispCirInfo[Illumn_ID].ColorInfo.TextBackcolor);
	
	EnvirQuaty(113,215);
}
//����UI3����,������ʾ
//ˮƽ�ָ��ֱ�ָ��Ŀ����
//x0: 20  y0:190
void LoadUI_3(uint16_t x0,uint16_t y0,uint16_t HScalUnit,uint16_t VScalUnit,uint8_t Item_Id)
{
	uint16_t i;
	uint8_t HCell,VCell;
	uint8_t buf[5];
	HCell = H_LEN/HScalUnit;  //ˮƽ����
	VCell = V_LEN/VScalUnit;  //��ֱ����
	if(orLoadUI[1]!=TRUE)
	{
		Wave_Id = 1;
		LCD_Clear(BLACK);		//����
		BACK_COLOR = BLACK;
		Show_Str(86,0,130,24,"����ͳ��",24,0,GREEN,BLACK);
		Show_Str(285,y0+5,50,16,"ʱ��",16,0,BLUE,BLACK);	
		LCD_DrawLine(x0,y0,320,y0,BLUE);		//��ˮƽ������(50,180) (300,180)
		LCD_DrawLine(x0,y0,x0,y0-170,BLUE);		//����ֱ������(50,180) (50,20)
		//����ˮƽ��ȣ�ʱ�����
		for(i=0;i<HScalUnit;i++)
		{
			LCD_DrawLine(x0+HCell*(i+1),y0-1,x0+HCell*(i+1),y0-170,GRAY);
			if(i%2==0)
			{
				LCD_ShowNum(x0+HCell*i-6,y0+5,i,2,12,BLUE,BLACK);//��ʾһ������
			}
		}
		//���ش�ֱ��ȣ���Ŀ��λ����
		for(i=0;i<VScalUnit;i++)
			LCD_DrawLine(x0+1,y0-(i+1)*VCell,320,y0-(i+1)*VCell,GRAY);
		for(i=0;i<7;i++)
		{
			Show_Str(40*i,y0+20,40,20,(uint8_t *)ItemName[i],16,0,(uint16_t)ColorTab[i],BLACK);
		}
		orLoadUI[1] = orLoadUI[0];
		orLoadUI[0] = FALSE;
	}		
	switch(Item_Id)
	{
		case Temp_ID:
		{
			Show_Str(0,y0-188,100,20,"�¶�(��)   ",16,0,(uint16_t)ColorTab[Temp_ID],BLACK);
			for(i=0;i<VScalUnit;i++)
			{
				sprintf(buf,"  %2d",i*5);
				LCD_ShowString(0,y0-(i+1)*VCell+8,20,12,12,buf,(uint16_t)ColorTab[Temp_ID],BLACK);//��ʾһ���ַ���,12/16����
			}
		}break;
		case Dump_ID:
		{
			//LCD_ShowNum(7,190-(i+1)*VCell+8,i*5,2,12);//��ʾһ������
			Show_Str(0,y0-188,100,20,"ʪ��(%)   ",16,0,(uint16_t)ColorTab[Dump_ID],BLACK);
			for(i=0;i<VScalUnit;i++)
			{				
				sprintf(buf,"  %2d",i*5);
				LCD_ShowString(0,y0-(i+1)*VCell+8,20,12,12,buf,(uint16_t)ColorTab[Dump_ID],BLACK);
			}
		}break;
		case HCHO_ID:
		{
			//LCD_ShowNum(7,190-(i+1)*VCell+8,i,2,12);//��ʾһ������
			Show_Str(0,y0-188,100,20,"��ȩ(ug/m3)",16,0,(uint16_t)ColorTab[HCHO_ID],BLACK);
			for(i=0;i<VScalUnit;i++)
			{
				sprintf(buf,"0.%d%d",i/10,i%10);
				LCD_ShowString(0,y0-(i+1)*VCell+8,20,12,12,buf,(uint16_t)ColorTab[HCHO_ID],BLACK);
			}
		}break;
		case UPres_ID:
		{
			//LCD_ShowNum(7,190-(i+1)*VCell+8,i,2,12);//��ʾһ������
			Show_Str(0,y0-188,100,20,"��ѹ(KPa)  ",16,0,(uint16_t)ColorTab[UPres_ID],BLACK);
			for(i=0;i<VScalUnit;i++)
			{			
				sprintf(buf,"  %2d",i*10);
				LCD_ShowString(0,y0-(i+1)*VCell+8,20,12,12,buf,(uint16_t)ColorTab[UPres_ID],BLACK);
			}
		}break;
		case DangerGas_ID:
		{
			//LCD_ShowNum(7,190-(i+1)*VCell+8,i,2,12);//��ʾһ������
			Show_Str(0,y0-188,100,20,"����(%)   ",16,0,(uint16_t)ColorTab[DangerGas_ID],BLACK);
			for(i=0;i<VScalUnit;i++)
			{				
				sprintf(buf,"  %2d",i*10);
				LCD_ShowString(0,y0-(i+1)*VCell+8,20,12,12,buf,(uint16_t)ColorTab[DangerGas_ID],BLACK);
			}
		}break;
		case Illumn_ID:
		{
			//LCD_ShowNum(7,190-(i+1)*VCell+8,i,2,12);//��ʾһ������
			Show_Str(0,y0-188,100,20,"��ǿ(Lux)  ",16,0,(uint16_t)ColorTab[Illumn_ID],BLACK);
			for(i=0;i<VScalUnit;i++)
			{
				sprintf(buf,"  %2d",i*100);
				LCD_ShowString(0,y0-(i+1)*VCell+8,20,12,12,buf,(uint16_t)ColorTab[Illumn_ID],BLACK);
			}
		}break;
		case PM2_5_ID:
		{
			//LCD_ShowNum(7,190-(i+1)*VCell+8,i,2,12);//��ʾһ������
			Show_Str(0,y0-188,100,20,"PM2.5(mg/m3)",16,0,(uint16_t)ColorTab[PM2_5_ID],BLACK);
			for(i=0;i<VScalUnit;i++)
			{
				sprintf(buf,"  %2d",i*10);
				LCD_ShowString(0,y0-(i+1)*VCell+8,20,12,12,buf,(uint16_t)ColorTab[PM2_5_ID],BLACK);
			}
		}break;
		default :break;
	}
	LoadDataWaveFromSD("0:/Sensordata",Item_Id);  //��ȡSD���Ĵ��������ݣ����ؿ�ʼ��¼ʱ�̵�ʱ��ֵ
}
//�����¼�����ݵ�SD��
//����ֵ��0��ȷ 1
uint8_t SaveDataToSDCard(uint8_t *filename)
{
	UINT bw;
	FIL *f_Savesenordata;
	uint8_t res = 0;
	uint16_t *p=0;   //����д��ָ��
	static uint32_t offset = 0;   //Ҫƫ�ƵĽṹ���ݳ���
	DataRemTypedef HourData;
	uint16_t Writelen = 0;
	Writelen = sizeof(DataRemTypedef);
	f_Savesenordata  = (FIL *)mymalloc(sizeof(FIL));
	if(f_Savesenordata == NULL)
	{
		return 1;
	}
	//�����ļ�
	res = f_open(f_Savesenordata,(const TCHAR*)filename,FA_READ|FA_WRITE);  //�����µ��ļ���SD����
	if(res==0X04)
		res = f_open(f_Savesenordata,(const TCHAR*)filename,FA_WRITE|FA_CREATE_NEW);
	if(res==FR_OK)  //�����ɹ��Ļ�ִ��
	{
		HourData.Time = calendar.hour;
		HourData.Item_Id[Temp_ID] = SesorData[0].Temp;
		HourData.Item_Id[Dump_ID] = SesorData[0].Damp;
		HourData.Item_Id[HCHO_ID] = SesorData[0].Jiaquanmg;
		HourData.Item_Id[UPres_ID] = SesorData[0].Jiaquanppm;
		HourData.Item_Id[DangerGas_ID] = SesorData[0].AnoSenor.FireGas;
		HourData.Item_Id[Illumn_ID] = SesorData[0].AnoSenor.LightStren;
		HourData.Item_Id[PM2_5_ID] = SesorData[0].AnoSenor.PM2_5;
		p = (uint16_t*)&HourData.Time;   		//ָ��ָ�����ݽṹ��
		res = f_lseek (f_Savesenordata,offset);
		res = f_write(f_Savesenordata,p,Writelen,&bw);  //д���½����ļ���
		offset += Writelen;
		f_close(f_Savesenordata); //�ر��ļ�
	}
	myfree(f_Savesenordata);
	return FR_OK;
}
//��ʾʱ�䵽������ͬʱ���洫�����ɼ���������
void DispTimeToUI(void)
{
	uint16_t BatADCVal = 0;
	uint8_t BatVal = 0;
	char buf[10];
	sprintf(buf,"������%2d:%02d", calendar.hour, calendar.min);
	Show_Str(lcddev.width-128,0,100,16,buf,16,0,GREEN,BLACK);//��ʾ�Ѿ����е�ʱ��
	if(Esp8266Config.WifiLinkSuccFlag==1)
	{
		Show_DrawBMP(lcddev.width-235,0,16,16,(uint8_t*)WifiLogo,WHITE);
		Show_Str(lcddev.width-213,6,180,12,Esp8266Config.WireNet[0],12,0,GREEN,BLACK);		
	}
	else if(Esp8266Config.WifiLinkSuccFlag==0)
	{
		Show_DrawBMP(lcddev.width-235,0,16,16,(uint8_t*)NoWifiLogo,WHITE);
		Show_Str(lcddev.width-213,6,180,12,"��ǰWIFIδ����",12,0,RED,BLACK);
	}
	
	if(calendar.hour==23)  //ÿ��һ������SD��������
	{
		f_unlink((const TCHAR*)"0:/Sensordata");  //ɾ��ָ�����ļ���Ŀ¼
	}
	if((calendar.min%5==0) && calendar.sec<2 && ToDataSDCard_WriteFlag==0)  //ÿһ��Сʱ��SD������д��ָ��������
	{
		SaveDataToSDCard("0:/Sensordata.txt");
		ToDataSDCard_WriteFlag = 1;
	}
	if(calendar.min%5 != 0)
		ToDataSDCard_WriteFlag = 0;
	BatADCVal = Get_Adc_Average(ChalTab[3],5);  //�õ���ص�ѹֵ��ģ����
	BatVal = (uint8_t)(BatADCVal*4096*(BAT_RES1+BAT_RES2))/(3.3*BAT_RES1);  //��������ΪBAT_RES1
	LCD_DrawRectangle(lcddev.width-30,3,lcddev.width-3,13,GRAY);
	LCD_Fill(lcddev.width-3,5,lcddev.width,10,GRAY);
	LCD_Fill(lcddev.width-29,4,lcddev.width-29+BatVal,12,LGRAY);  //�����겻�ܴ���35����ĺ������ط�Χ
}
//��SD�������ȡ����Ĵ���������
//�ļ�·�� ID�� ���ݱ�����
void LoadDataWaveFromSD(uint8_t *filename,uint8_t Item)
{
	UINT br;
	FIL *f_Readsenordata;
	uint8_t res = 0;
	uint16_t i;
	uint16_t ReadLen = 0;
	uint16_t TempData;  //280Ϊ���ص�
	uint16_t *Databuf;  //����ָ����
	uint8_t *Senorbuf = 0;
	DataRemTypedef *tempdaydata;
	ReadLen = sizeof(DataRemTypedef);     //15*24*3;  
	Databuf = (uint16_t *)mymalloc(ReadLen);
	if(Databuf==NULL)
		return;
	f_Readsenordata = (FIL *)mymalloc(sizeof(FIL));
	if(f_Readsenordata==NULL)
	{
		myfree(Databuf);  //�ͷŵ��ڴ�
		return;
	}
	res=f_open(f_Readsenordata,(const TCHAR*)filename,FA_READ);
	if(res==0)  		  //��ȡ�ɹ��Ļ�
	{
		f_read(f_Readsenordata,Databuf,ReadLen,(UINT*)&br);
	}
	tempdaydata = (DataRemTypedef*)Databuf;  //ָ��ת��Ϊ�ṹ����õ�����
	f_lseek(f_Readsenordata,tempdaydata->Time);  //�ļ�ָ��ƫ�Ƶ����ݵ���ʼ��
	for(i=0;i<280;i++)  //����Ǵ�һ���ĳ��ʱ�̿��������ȡ��ʱ�ĵ�23ʱ������
	{
		switch(Item)
		{
			case Temp_ID:
			{
				 TempData = DeadScale(tempdaydata->Item_Id[Temp_ID],0,50);
				 LCD_Fast_DrawPoint(X0+i,Y0-TempData*3.4,(uint16_t)ColorTab[Temp_ID]);
			}break;
			case Dump_ID:
			{
				 TempData = DeadScale(tempdaydata->Item_Id[Dump_ID],0,50);
				 LCD_Fast_DrawPoint(X0+i,Y0-TempData*3.4,(uint16_t)ColorTab[Dump_ID]);
			}break;
			case HCHO_ID:
			{
				 TempData = DeadScale(tempdaydata->Item_Id[HCHO_ID],0,20);
				 LCD_Fast_DrawPoint(X0+i,Y0-TempData*17,(uint16_t)ColorTab[HCHO_ID]);
			}break;
			case UPres_ID:
			{
				 TempData = DeadScale(tempdaydata->Item_Id[UPres_ID],0,10000);
				LCD_Fast_DrawPoint(X0+i,Y0-TempData*1.7/100,(uint16_t)ColorTab[UPres_ID]);
			}break;
			case DangerGas_ID:
			{
				 TempData = DeadScale(tempdaydata->Item_Id[DangerGas_ID],0,100);
				 LCD_Fast_DrawPoint(X0+i,Y0-TempData*1.7,(uint16_t)ColorTab[DangerGas_ID]);
			}break;
			case PM2_5_ID:
			{
				TempData = DeadScale(tempdaydata->Item_Id[PM2_5_ID],0,20);
				LCD_Fast_DrawPoint(X0+i,Y0-TempData*1.7,(uint16_t)ColorTab[PM2_5_ID]);
			}break;
			case Illumn_ID:
			{
				TempData = DeadScale(tempdaydata->Item_Id[Illumn_ID],0,150);
				LCD_Fast_DrawPoint(X0+i,Y0-TempData*1.7,(uint16_t)ColorTab[Illumn_ID]);
			}break;			
		    default:break;
		}
		f_read(f_Readsenordata,Databuf,sizeof(DataRemTypedef),(UINT*)&br);
		tempdaydata = (DataRemTypedef*)Databuf;
		f_lseek(f_Readsenordata,sizeof(DataRemTypedef));  //�ļ�ָ��ƫ��
	}
	f_close(f_Readsenordata);  //�ر��ļ�,�ͷ��ڴ�
	myfree(Databuf);
	myfree(f_Readsenordata);
}
//�л�������Ϣ
//������
void ChangeUI(uint8_t UInum)
{
	LoadSensorData(&SesorData[0]);  //���ش���������
	switch(UInum)
	{
		case 1:
		{
			LoadUI_1();
		}break;
		case 2:
		{
			LoadUI_2(0,5,18,16);			
		}break;
		case 3:
		{
			if(PreWave_Id!=Wave_Id)
			{
				LoadUI_3(X0,Y0,HSCAL_UNIT,VSCAL_UNIT,(uint8_t)Item_Id[Wave_Id]);				
				PreWave_Id = Wave_Id;
				if(Wave_Id==0)
				{
					//LCD_Fill(CURSOR-22,16,320,240,FillColor);
					PreWave_Id = 0XFF;
				}
			}
//			LoadUI_3(X0,Y0,HSCAL_UNIT,VSCAL_UNIT,Item_Id[UPres_ID]);
		}break;
		default :break;
	}
	DispTimeToUI();  //����ʱ��
}

uint16_t Testbuf[10]={12345,56789,23546,65519,58754,25345,61257,25643,23453,45332};
//SD����д����.��,˫�ֽ����Ѿ����Գɹ�
void SDWiriteDataTest(uint8_t *filename,uint8_t mode)
{
	UINT bw;
	FIL *F_testdata;
	uint16_t *databuf;
	uint8_t res;
	databuf = (uint16_t *)mymalloc(sizeof(Testbuf));  //����50���ֽڵĿռ�
	if(databuf==NULL)
	{
		Show_Str(0,100,300,16,"�ڴ�����ʧ��",16,0,GREEN,BLACK);
		return;
	}
	databuf = Testbuf;
	F_testdata = (FIL *)mymalloc(sizeof(FIL));
	if(F_testdata==NULL)
	{
		myfree(databuf);
		return;
	}
	if(mode==1)
		res = f_open(F_testdata,(const TCHAR*)filename,FA_READ|FA_WRITE);  	//��֮ǰ�е��ļ�
	if(mode==0||res==0x04)  //�����ʧ���򴴽����ļ�
		res = f_open(F_testdata,(const TCHAR*)filename,FA_WRITE|FA_CREATE_NEW);
	//f_sync(F_testdata);
	if(res==FR_OK)
	{
		res = f_write(F_testdata,databuf,sizeof(Testbuf),(UINT *)&bw); 
		f_close(F_testdata);
	}
	myfree(databuf);
	myfree(F_testdata);
}
void SDReadDataTest(uint8_t *filename)
{
	uint8_t i;
	UINT br;
	FIL *F_testdata;
	uint16_t *databuf;
	uint8_t res;
	databuf = (uint16_t *)mymalloc(sizeof(Testbuf));  //����50���ֽڵĿռ�
	if(databuf==NULL)
	{
		Show_Str(0,100,300,16,"�ڴ�����ʧ��",16,0,GREEN,BLACK);
		return;
	}
	F_testdata = (FIL *)mymalloc(sizeof(FIL));
	if(F_testdata==NULL)
	{
		myfree(databuf);
		return;
	}
	res=f_open(F_testdata,(const TCHAR*)filename,FA_READ);
	if(res==0)
	{
		f_read(F_testdata,databuf,sizeof(Testbuf),(UINT *)&br);
		for(i=0;i<10;i++)
		{
			LCD_ShowNum(0,i*18,databuf[i],6,16,GREEN,BLACK);//LCD_ShowNum(uint16_t x,uint16_t y,u32 num,uint8_t len,uint8_t size);
		}
		f_close(F_testdata);
	}
	myfree(databuf);
	myfree(F_testdata);	
}
void TestSDCard(void)
{
	SDWiriteDataTest("0:/guchenglong.txt",1);
	SDReadDataTest("0:/guchenglong.txt");
	Show_Str(0,200,300,16,"���",16,0,GREEN,BLACK);
}



