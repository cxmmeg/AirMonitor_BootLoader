#include "sys.h"
#include "ff.h"
#include "w25q64.h"
#include "malloc.h"
#include "ili93xx.h"
#include "updatefont.h"
#define FONTINFOADDR 	(4916+100)*1024 
//�ֿ���Ϣ�ṹ��. 
//���������ֿ������Ϣ����ַ����С��
_font_info ftinfo;

const uint8_t *GBK24_PATH = "0:/SYSTEM/FONT/GBK24.FON";		//GBK24�Ĵ��λ��
const uint8_t *GBK16_PATH = "0:/SYSTEM/FONT/GBK16.FON";		//GBK24�Ĵ��λ��
const uint8_t *GBK12_PATH = "0:/SYSTEM/FONT/GBK12.FON";		//GBK24�Ĵ��λ��
const uint8_t *UNIGBK_PATH = "0:/SYSTEM/FONT/UNIGBK.BIN";		//GBK24�Ĵ��λ��
//����ֿ�
//����ֵ 1 �ֿⶪʧ����
//       0 �ֿ�ɹ�
uint8_t Font_Init(void)
{
	W25Q64_Init();	//��ʼ��SPI flash
	SPI_Flash_Read((uint8_t*)&ftinfo,FONTINFOADDR,sizeof(ftinfo));//�����ṹ�������
	if(ftinfo.fontok!=0XAA)
		return 1;
	else
		return 0;
}
//��ʾ��ǰ������½���
//x,y:����
//size:�����С
//fsize:�����ļ���С
//pos:��ǰ�ļ�ָ��λ��
uint32_t fupd_prog(uint16_t x,uint16_t y,uint8_t size,uint32_t fsize,uint32_t pos)
{
	float prog;
	uint8_t t=0XFF;
	prog=(float)pos/fsize;
	prog*=100;
	if(t!=prog)
	{
		LCD_ShowString(x+3*size/2,y,240,320,size,"%",POINT_COLOR,BACK_COLOR);		
		t=prog;
		if(t>100)t=100;
		LCD_ShowNum(x,y,t,3,size,BLACK,LIGHTBLUE);//��ʾ��ֵ
	}
	return 0;					    
} 
//����ĳһ������
//x,y���꣬size�����С��fxpath·����fx���µ�����
//����ֵ:0�ɹ� 1ʧ��
//
uint8_t update_fontx(uint16_t x,uint16_t y,uint8_t size,uint8_t *ftpath,uint8_t fx)
{
	uint32_t flashaddr = 0;
	FIL *fftemp;
	uint8_t *tempbuf;		//Ϊ�ֿ�д�����ڴ�ռ�
	uint8_t res;
	uint16_t bread;
	uint32_t offx = 0;
	uint8_t rval = 0;
	fftemp = (FIL*)mymalloc(sizeof(FIL));
	tempbuf = mymalloc(4096);	//����4k���ֽڿռ�
	if(fftemp==NULL)
		rval = 1;
	res = f_open(fftemp,(const TCHAR*)ftpath,FA_READ);
	if(res)
		rval = 2;	//���ļ�ʧ��
	if(rval==0)
	{
		switch(fx)
		{
			case 0:
				ftinfo.ugbkaddr = FONTINFOADDR+sizeof(ftinfo);	//��Ϣͷ֮�󣬽���UNIGBKת�����
				ftinfo.ugbksize = fftemp->fsize;					//UNIGBK��С
				flashaddr = ftinfo.ugbkaddr;
				break;
			case 1:
				ftinfo.f12addr = ftinfo.ugbkaddr+ftinfo.ugbksize;	//UNIGBK֮�󣬽���GBK12�ֿ�
				ftinfo.gbk12size = fftemp->fsize;					//GBK12�ֿ��С
				flashaddr = ftinfo.f12addr;						//GBK12����ʼ��ַ
				break;
			case 2:
				ftinfo.f16addr = ftinfo.f12addr+ftinfo.gbk12size;	//GBK12֮�󣬽���GBK16�ֿ�
				ftinfo.gbk16size = fftemp->fsize;					//GBK16�ֿ��С
				flashaddr = ftinfo.f16addr;						//GBK16����ʼ��ַ
				break;
			case 3:
				ftinfo.f24addr = ftinfo.f16addr+ftinfo.gbk16size;	//GBK16֮�󣬽���GBK24�ֿ�
				ftinfo.gkb24size = fftemp->fsize;					//GBK24�ֿ��С
				flashaddr = ftinfo.f24addr;						//GBK24����ʼ��ַ
				break;				
		}
		while(res==FR_OK)
		{
			res = f_read(fftemp,tempbuf,4096,(UINT *)&bread);		//��ȡ4K�����ݵ�����
			if(res!=FR_OK)break;//ִ�д���
			SPI_Flash_Write(tempbuf,flashaddr+offx,4096);
			offx+=bread;
			if(bread!=4096)break;			
		}
		f_close(fftemp);	//�ر��ļ�
	}
	myfree(fftemp);			//�ͷ��ڴ�
	myfree(tempbuf);		//�ͷ��ڴ�
	return res;	
}

//
//�����ֿ��ļ���UNIGBK,GBK12,GBK16,GBK24һ�����
//x,y����Ҫ��ʾ��λ��
//
uint8_t update_font(uint16_t x,uint16_t y,uint8_t size)
{
	uint8_t *gbk24_path = (uint8_t*)GBK24_PATH;
	uint8_t *gbk16_path = (uint8_t*)GBK16_PATH;
	uint8_t *gbk12_path = (uint8_t*)GBK12_PATH;
	uint8_t *unigbk_path = (uint8_t*)UNIGBK_PATH;
	uint8_t Res = 0XFF;
	ftinfo.fontok = 0XFF;
	
	SPI_Flash_Write((uint8_t *)&ftinfo,FONTINFOADDR,sizeof(ftinfo));//���֮ǰ�ֿ�ı�־����ֹ���µ�һ�������������ֿⲿ�ֶ�ʧ
	SPI_Flash_Read((uint8_t*)&ftinfo,FONTINFOADDR,sizeof(ftinfo));
	
	LCD_ShowString(x,y,400,320,size,"Updating UNIGBK.BIN......",BLACK,LIGHTBLUE);
	Res = update_fontx(x+20*size/2,y,size,unigbk_path,0);			//����UNIGBK.BIN
	if(Res)  return 1;
    LCD_ShowString(x,y+30,400,320,size,"Updating GBK12.BIN......",BLACK,LIGHTBLUE);
	
	Res=update_fontx(x+20*size/2,y,size,gbk12_path,1);			//����GBK12.FON
	if(Res)  return 2;
	LCD_ShowString(x,y+60,400,320,size,"Updating GBK16.BIN......",BLACK,LIGHTBLUE);
	
	Res=update_fontx(x+20*size/2,y,size,gbk16_path,2);			//����GBK16.FON
	if(Res)	 return 3;
	LCD_ShowString(x,y+85,400,320,size,"Updating GBK24.BIN......",BLACK,LIGHTBLUE);
	
	Res=update_fontx(x+20*size/2,y,size,gbk24_path,3);			//����GBK24.FON
	if(Res)  return 4;
	//����ȫ���������
	ftinfo.fontok = 0XAA;
	SPI_Flash_Write((uint8_t *)&ftinfo,FONTINFOADDR,sizeof(ftinfo));//����д���ֿ���Ϣ
	LCD_ShowString(x,y+80,400,320,size,"Font Update Finish",BLACK,LIGHTBLUE);
	return 0;//���³ɹ�����
}














































