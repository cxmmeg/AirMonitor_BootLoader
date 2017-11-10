#ifndef __DHT11_H
#define __DHT11_H 
#include "sys.h"   
//IO��������  C5
#define DHT11_IO_IN()  {GPIOC->CRL&=0XFF0FFFFF;GPIOC->CRL|=8<<20;}    //����Ϊ����
#define DHT11_IO_OUT() {GPIOC->CRL&=0XFF0FFFFF;GPIOC->CRL|=3<<20;}    //����Ϊ���
////IO��������											   
#define	DHT11_DQ_OUT PCout(5) //���ݶ˿�	PC5
#define	DHT11_DQ_IN  PCin(5)  //���ݶ˿�	PC5


uint8_t DHT11_Init(void);//��ʼ��DHT11
uint8_t DHT11_Read_Data(uint8_t *temp,uint8_t *humi);//��ȡ��ʪ��
uint8_t DHT11_Read_Byte(void);//����һ���ֽ�
uint8_t DHT11_Read_Bit(void);//����һ��λ
uint8_t DHT11_Check(void);//����Ƿ����DHT11
void DHT11_Rst(void);//��λDHT11   
void DHT11_Filted(uint8_t *temp,uint8_t *humi);
#endif


