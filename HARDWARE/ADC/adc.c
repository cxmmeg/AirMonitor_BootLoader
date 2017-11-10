#ifndef __ADC_C
#define __ADC_C	
#include "include.h"	
uint8_t const ThreVal[7] = {0,0,0,0,0,0,0};//���������ݸı��ж���ֵ
uint8_t const ChalTab[4] = {ADC_Channel_0,ADC_Channel_1,ADC_Channel_2,ADC_Channel_3};  //Ҫת����ADC��ͨ��
uint8_t const JQCmd[5] = {0XA5,0X5A,0X02,0X80,0XAA};  //��ȡ����֡
uint8_t const JQOFFSET[5] = {0XA5,0X5A,0X02,0X85,0XAA};  //������У��
Sensor SesorData[2];   		//���д���������
void  Adc_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure; 
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE );	  //ʹ��ADC1ͨ��ʱ��
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

    //PA0---PA3��Ϊģ��ͨ����������
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOC, &GPIO_InitStructure);				
	GPIO_ResetBits(GPIOC,GPIO_Pin_3);						 
    
    ADC_DeInit(ADC1);       //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڶ�ͨ��ɨ��ģʽ,�����ͨ��
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת������������ת��ģʽ
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
    ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   

    ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1

    ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼  

    while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����

    ADC_StartCalibration(ADC1);	 //����ADУ׼

    while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����

//    ADC_Cmd(ADC1, ENABLE);		 
    //ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������
}				
 	    
//DMA1�ĸ�ͨ������
//����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
//����ģʽ->�Ӵ洢��/16λ���ݿ��/�洢������ģʽ
//DMA_CHx:DMAͨ��CHx
//PeripAdd:�����ַ
//MemAdd:�洢����ַ
//BufLen:���ݴ����� 
void ADC_DMAInit(DMA_Channel_TypeDef* DMA_CHx,u32 PeripAdd,u32 MemAdd,uint16_t BufLen)
{
	DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA����

    DMA_DeInit(DMA_CHx);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
    DMA_InitStructure.DMA_PeripheralBaseAddr = PeripAdd;  //DMA����ADC����ַ
    DMA_InitStructure.DMA_MemoryBaseAddr = MemAdd;  //DMA�ڴ����ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC ;  //���ݴ��䷽�򣬴������ȡ���͵��ڴ�
    DMA_InitStructure.DMA_BufferSize = BufLen;  //DMAͨ����DMA����Ĵ�С
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //���ݿ��Ϊ16λ
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //���ݿ��Ϊ16λ
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular ;  //��������������ģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMAͨ�� xӵ�������ȼ� 
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��	
    DMA_Init(DMA_CHx, &DMA_InitStructure);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���	
    DMA_Cmd(DMA_CHx, ENABLE);
} 

///���ADCֵ
//ch:ͨ��ֵ3 0--3
uint16_t Get_Adc(uint8_t ch)
{
//    uint16_t ADCVal;
    //ADC_Cmd(ADC1, ENABLE);
    ADC1->CR2 |= 0x00000001;//��ADC1
    //����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	

    //while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������
    while(!(ADC1->SR & 0x02));                       //�ȴ�ת������

    return  (uint16_t)ADC1->DR;                //ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}
//�õ�ADCת�������ֵ12λ����0--4095
//ת������Ϊ5��
uint16_t Get_Adc_Average(uint8_t ch,uint8_t times)
{
    uint8_t i;
//    uint16_t temp_val[5] = {0,0,0,0,0};
//    uint16_t Temp = 0;
    uint32_t Sum = 0;
    for(i=0;i<times;i++)
    {
        Sum += Get_Adc(ch);
//        temp_val[i] = Get_Adc(ch);
        delay_ms(5);
    }
//    for(i=0;i<times-1; i++)//����
//    {
//        for(j=i+1;j<times;j++)
//        {
//            if(temp_val[i]>temp_val[j])//��������
//            {
//                 Temp = temp_val[i];
//                 temp_val[i] = temp_val[j];
//                 temp_val[j] =  Temp;
//            }      
//        }
//    }
//    for(i=1;i<times-1;i++)  //��ȥ���˵ļ�ֵ
//    {
//        Sum += temp_val[i];
//    }
    //ADC_Cmd(ADC1, DISABLE);
    ADC1->CR2 &= 0xFFFFFFFE;     //�ر�ADC1
//    return Sum/(times-2); //����ƽ��ֵ,ת��Ϊ16λ������
    return Sum/times;
} 	 
uint16_t Pm2_5Read(uint8_t ch,uint8_t times)
{
	uint8_t i;
	uint32_t Sum = 0;
	for(i=0;i<times;i++)
    {
        Sum += Get_Adc(ch);
        delay_ms(1);
    }
	ADC1->CR2 &= 0xFFFFFFFE;     //�ر�ADC1
    return Sum/times;
}

/*
���ܣ����������ݱ仯��⣬���б䶯��һ����Χ���򲻷������ݣ������ʹ���������
�������;������������
����ֵ����ֵ״̬���ͻ򲻷���;
*/
uint8_t SensorDtaChageCheck(Sensor *Sensordata)
{
	if(abs(Sensordata->AnoSenor.FireGas - SesorData[1].AnoSenor.FireGas>ThreVal[0]))
        return 1;
    if(abs(Sensordata->AnoSenor.LightStren - SesorData[1].AnoSenor.LightStren)>ThreVal[1])
        return 1;
	if(abs(Sensordata->AnoSenor.PM2_5 - SesorData[1].AnoSenor.PM2_5)>ThreVal[2])
        return 1;
	if(abs(Sensordata->Temp - SesorData[1].Temp)>ThreVal[3])
        return 1;
	if(abs(Sensordata->Damp - SesorData[1].Damp)>ThreVal[4])
        return 1;
	if(abs(Sensordata->Result_UPres - SesorData[1].Result_UPres)>ThreVal[5])
        return 1;
	if(abs(Sensordata->Jiaquanmg - SesorData[1].Jiaquanmg)>ThreVal[6])
        return 1;
	SesorData[1].AnoSenor.FireGas = Sensordata->AnoSenor.FireGas;
	SesorData[1].AnoSenor.LightStren = Sensordata->AnoSenor.LightStren;
	SesorData[1].AnoSenor.PM2_5 = Sensordata->AnoSenor.PM2_5;
	SesorData[1].Temp = Sensordata->Temp;
	SesorData[1].Damp = Sensordata->Damp;
	SesorData[1].Result_UPres = Sensordata->Result_UPres;
	SesorData[1].Jiaquanmg = Sensordata->Jiaquanmg;
	SesorData[1].Jiaquanppm = Sensordata->Jiaquanppm;
   return 0;
}





#endif 

























