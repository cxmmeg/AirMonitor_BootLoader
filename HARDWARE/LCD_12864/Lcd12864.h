#ifndef _LCD12864__H
#define _LCD12864__H
#include "include.h"
//I\O���Ӷ˿ڶ���
#define SET_DATA    PCout(0)=1     //RS=1
#define SET_INC     PCout(0)=0;    //RS=0

#define SET_READ    PCout(1)=1     //RW=1
#define SET_WRITE   PCout(1)=0     //RW = 0

#define SET_EN      PCout(2)=1     //EN = 1
#define CLR_EN      PCout(2)=0     //EN = 0

//void Lcd12864CheckBusy( void );      //���æ����
void Lcd12864SendCmd( u8 byCmd );      //��������
void Lcd12864SendData( u8 byData );    //��������
void Lcd12864Init( void );                  //��ʼ��
void Lcd12864SetAddress( unsigned char x,unsigned char  y );  //��ַת��
void Lcd12864PutString( unsigned char x, unsigned char y, unsigned char *pData );//��ʾ�ַ���
void lcd12864_IO_init(void);
void Lcd12864PutNum(u8 x, u8 y, long num);
void display_12864(void);
void Lcd12864ClerScreen(void);
void Draw_pic(const unsigned char *p);





#endif










