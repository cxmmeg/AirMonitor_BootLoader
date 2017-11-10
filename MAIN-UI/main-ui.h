#include "sys.h"
//��ص�ѹ��������ΪBAT_RES1����λ����һ��������һ��Ҫ�����ͬһ��λ
#define BAT_RES1 100
#define BAT_RES2 10
#define PI 3.1415

#define HCHO_ID			0
#define PM2_5_ID		1
#define DangerGas_ID	2
#define Temp_ID			3
#define Dump_ID			4
#define UPres_ID		5
#define Illumn_ID		6

//�������ʾ�����Ϣ
#define X0 30          //ԭ������X
#define Y0 200         //ԭ������Y
#define H_LEN 280  	    //x��ĳ���
#define V_LEN 170  	   //Y��ĳ���
#define HSCAL_UNIT 24  //X�ᵥԪ����Ŀ
#define VSCAL_UNIT 10  //Y�ᵥԪ�����Ŀ

#define CURSOR    305  //��ʾ�����������α�


//��ʾ����UI
#define UI_1  1
#define UI_2  2
#define UI_3  3

//���������ýṹ��
//һ����ʾԲ��Ļ�����Ϣ
__packed typedef struct  DISPCIRDEF
{
	uint16_t x0;
	uint16_t y0;	
	uint16_t r1;			//�뾶
	uint16_t r2;
	int8_t Xoff;
	int8_t Yoff;
	__packed struct ColorInfo
	{
		uint16_t Cir1color;
		uint16_t Cir2color;
		uint16_t Valuecolor;
		uint16_t Textcolor;	
		uint16_t TextBackcolor;
		uint16_t ValueBackcolor;
	}ColorInfo;
	uint8_t *Text_Name;
	uint8_t *Text_UnitMeas;
	uint8_t FontSize;//�����С
}DispCirDef;
//����ļ�¼���ṹ����
__packed typedef struct  DAYDATA
{
	uint16_t Time;   	 //д���ʱ�䣬����дһ��
	uint16_t Item_Id[7];  //д�����Ŀ	
}DataRemTypedef;
extern volatile uint8_t CurrentuiID;
extern uint8_t orLoadUI[2];
extern uint8_t Wave_Id;
extern uint8_t PreWave_Id;
void ChangeUI(uint8_t UInum);

void LoadColorPat(void);
void DrawGradualColor(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t RangeMin,uint16_t RangeMax);
void DrawDynamicCir(uint16_t x0,uint16_t y0,uint16_t r,uint16_t color);
void DispCirInfo(DispCirDef const *DispCirInfo,uint8_t Item);
//void LoadInfoStruct(void);
void LoadSensorData(Sensor *sensordata);
void DataDispUI2(uint16_t x0,uint16_t y0,uint8_t ystep,uint8_t fontsize);
void DispTimeToUI(void);
void ClearDataArea(void);
void LoadDataWaveFromSD(uint8_t *filename,uint8_t Item);
void TestSDCard(void);

void TestPm2_5(void);























