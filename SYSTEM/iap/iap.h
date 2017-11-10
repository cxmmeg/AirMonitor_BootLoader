#ifndef __IAP__H
#define __IAP__H
#include "sys.h"

typedef void (*iapfun)(void);  //���庯��ָ�����ڳ�����ת
	
#define FLASH_APP1_ADDR	  0x0800E000  	//��һ��Ӧ�ó�����ʼ��ַ(�����FLASH)
											//����0X08000000~0X0800FFFF�Ŀռ�ΪBootloaderʹ�ã�64KB��

void iap_load_app(uint32_t appxaddr);			//��ת��APP����ִ��
void iap_write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t applen);	//��ָ����ַ��ʼ,д��bin

uint8_t IAP_Update_Data(uint8_t *filename,uint8_t x0,uint8_t y0);
#endif






















