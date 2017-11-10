#ifndef _LCD12864__C
#define _LCD12864__C
#include "include.h"
#define   D    3               //��ʱ�궨��
u8 const table_num[]="0123456789abcdefg";    //��ʾ������
//���ܣ�12864I\O�ڳ�ʼ��
//��������
void lcd12864_IO_init(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;	
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PB,PC�˿�ʱ��
	 GPIO_InitStructure.GPIO_Pin =GPIO_Pin_All;               //���ݿ��ƶ˿�
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 				//���Ƶ��
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 					//����Ϊ�������
	 GPIO_Init(GPIOD, &GPIO_InitStructure);										//�ṹ���ʼ��	
	 GPIO_SetBits(GPIOD,GPIO_Pin_All);
	
	 GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;;//������ƶ˿�
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 				//���Ƶ��
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 					//����Ϊ�������
	 GPIO_Init(GPIOC, &GPIO_InitStructure);										//�ṹ���ʼ��	
	 GPIO_SetBits(GPIOC,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2);	
}
//���ܣ����12864�Ƿ���æ��״̬
//��������
/*void Lcd12864CheckBusy( void )      
{
    unsigned int nTimeOut = 0 ;
    SET_INC;
    SET_READ;
    CLR_EN;
    SET_EN;
	  GPIOB->CRH=0X00;
    while( ( GPIOB->IDR & 0x80 ) && ( ++nTimeOut != 0 ) ) ;
	  GPIOB->CRH=0XFF00;
    CLR_EN;
    SET_INC;
    SET_READ;
}*/
//���ܣ�12864��������
//������byCmd Ҫ���͵���������
void Lcd12864SendCmd(u8 byCmd )
{
    //v_Lcd12864CheckBusy_f() ;
	  delay_ms(D);
    SET_INC;
    SET_WRITE;
    CLR_EN;
    GPIOD->ODR=byCmd ;    //�����ݷ���˿�λ�ĵͰ�λ
    delay_ms(D);
    delay_ms(D);
    SET_EN;
    delay_ms(D);
    delay_ms(D);
    CLR_EN;
    SET_READ;
    SET_INC;
}
//���ܣ�12864��������
//������byData Ҫ���͵�����
void Lcd12864SendData( u8 byData )    
{
    //Lcd12864CheckBusy();
	  delay_ms(D);
    SET_DATA;
    SET_WRITE;
    CLR_EN;
    GPIOD->ODR=byData ;  //�����ݷ���Ͱ�λ���ݶ˿�
    delay_ms(D);
    delay_ms(D);
    SET_EN;  
    delay_ms(D);
    delay_ms(D);
    CLR_EN;
    SET_READ;
    SET_INC;
}
//���ܣ�12864�����ʼ��
//��������
void Lcd12864Init( void )                  //��ʼ��
{
	  lcd12864_IO_init();
    Lcd12864SendCmd( 0x30 ) ;              //����ָ�
    delay_ms(500);
    Lcd12864SendCmd( 0x01 ) ;              //����
    delay_ms(500);
    Lcd12864SendCmd( 0x06 ) ;              //�������
    delay_ms(500);
    Lcd12864SendCmd( 0x0c ) ;              //����ʾ
}
void Lcd12864ClerScreen(void)
{
    Lcd12864SendCmd( 0x01 ) ;
}
//���ܣ�����XY��ַת��
//��������
void Lcd12864SetAddress( u8 x, u8 y )  //��ַת��
{
    u8 byAddress ;
    switch( y )
    {
        case 0 :    byAddress = 0x80 + x ;
            break;
        case 1 :    byAddress = 0x90 + x ;
            break ;
        case 2 :    byAddress = 0x88 + x ;
            break ;
        case 3 :    byAddress = 0x98 + x ;
            break ;
        default :
            break ;
    }
    Lcd12864SendCmd( byAddress ) ;
}
//���ܣ�12864��ʾ�ַ���
//������X���꣬Y���꣬pData �ַ���ָ��������׵�ַ
//˵��:�����ո����һ���ո�
void Lcd12864PutString( u8 x, u8 y, u8 *pData )
{
    Lcd12864SetAddress( x, y );
    while( *pData != '\0' )
    {
        Lcd12864SendData( *pData++ ) ;
    }
}       
//���ܣ�12864��ʾһ��ʮ��������
//������XY���꣬num Ҫ��ʾ������
//��Χ��0-255
void Lcd12864PutNum(u8 x, u8 y, long num)
{
    u16 Temp;
    Temp=num%100000;  //ȡ��ʵ�ʵĲ���
		Lcd12864SetAddress(x,y);
		Lcd12864SendData(table_num[Temp/10000]);      //���Ҫ�õ�����
		Lcd12864SendData(table_num[Temp/1000%10]);
		Lcd12864SendData(table_num[Temp/100%10]);
    Lcd12864SendData(table_num[Temp/10%10]);
    Lcd12864SendData(table_num[Temp%10]);
}
//���ܣ�12864��ʾ��ʼ��
//����;��
void display_12864(void)
{
	  Lcd12864PutString(0,0,"IP:192.168.1.16");	 
	  Lcd12864PutString(0,1,"PORT:1400");	
	  Lcd12864PutString(0,2,"NowSTATUS:");	
	  Lcd12864PutString(0,3,"�ŵ�:");
}
//��ͼ���� ��ڲ���Ϊ����ָ��
void Draw_pic(const unsigned char *p)
{ 
	unsigned char i,j;

	Lcd12864SendCmd(0x34);   //������ָ����رջ�ͼ��ʾ
	for(j=0;j<32;j++)	 //һ������
	{
		Lcd12864SendCmd(0x80+j);   //��ֱ����
		Lcd12864SendCmd(0x80);     //ˮƽ���꣬ACָ����Զ����ż�һ
		for(i=0;i<16;i++)
		{
			Lcd12864SendData(*p++);  
		}
	}
	
	for(j=0;j<32;j++)	 //��������
	{
		Lcd12864SendCmd(0x80+j);   //��ֱ����
		Lcd12864SendCmd(0x88);     //ˮƽ���꣬ACָ����Զ����ż�һ
		for(i=0;i<16;i++)
		{
			Lcd12864SendData(*p++);  
		}
	}
	Lcd12864SendCmd(0x36);   //�򿪻�ͼ��ʾ
	Lcd12864SendCmd(0x30);   //�ص�����ָ�
}



#endif















