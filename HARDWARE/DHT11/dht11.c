//DHT11��ʪ�ȴ�������������
#include "include.h"
//��ʼ��DHT11��IO�� DQ ͬʱ���DHT11�Ĵ���
//����1:������
//����0:����    	 
uint8_t DHT11_Init(void)
{	 
 	GPIO_InitTypeDef  GPIO_InitStructure;
	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 //PC11�˿�����
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);				 //��ʼ��IO��
 	GPIO_SetBits(GPIOC,GPIO_Pin_5);						 //PC11 �����		    
	DHT11_Rst();  //��λDHT11
	return DHT11_Check();//�ȴ�DHT11�Ļ�Ӧ
} 

//��λDHT11
void DHT11_Rst(void)	   
{                 
    DHT11_IO_OUT(); 	//SET OUTPUT
    DHT11_DQ_OUT=0; 	//����DQ
    delay_ms(30);    	//��������18ms
    DHT11_DQ_OUT=1; 	//DQ=1 
    delay_us(40);     	//��������20~40us
}
//�ȴ�DHT11�Ļ�Ӧ
//����1:δ��⵽DHT11�Ĵ���
//����0:����
uint8_t DHT11_Check(void) 	   
{   
	uint8_t retry=0;
	DHT11_IO_IN();       //SET INPUT
    while(DHT11_DQ_IN&&retry<100)//DHT11������40~80us
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=100)
        return 1;
	else 
        retry=0;
    while(!DHT11_DQ_IN&&retry<100)//DHT11���ͺ���ٴ�����40~80us
	{
		retry++;
		delay_us(1);
	};
	if(retry>=100)
        return 1;	    
	return 0;
}
//��DHT11��ȡһ��λ
//����ֵ��1/0
uint8_t DHT11_Read_Bit(void)
{
 	uint8_t retry=0;
	while(DHT11_DQ_IN&&retry<100)//�ȴ���Ϊ�͵�ƽ
	{
		retry++;
		delay_us(1);
	}
	retry=0;
	while(!DHT11_DQ_IN&&retry<100)//�ȴ���ߵ�ƽ
	{
		retry++;
		delay_us(1);
	}
	delay_us(40);//�ȴ�40us
	if(DHT11_DQ_IN)return 1;
	else return 0;		   
}
//��DHT11��ȡһ���ֽ�
//����ֵ������������
uint8_t DHT11_Read_Byte(void)    
{
    uint8_t i,dat;
    dat=0;
    for (i=0;i<8;i++) 
    {
        dat<<=1; 
        dat|=DHT11_Read_Bit();
    }
    return dat;
}
#define ErrorAllow  10   //��Χ��5��
//��DHT11��ȡһ������
//temp:�¶�ֵ(��Χ:0~50��)
//humi:ʪ��ֵ(��Χ:20%~90%)if((((TempData[1]<=TempData[0])&&(TempData[0]<TempData[1]+ErrorAllow))||((TempData[0]<=TempData[1])&&(TempData[1]<TempData[0]+ErrorAllow)))
	//		&&(((HumiData[1]<=HumiData[0])&&(HumiData[0]<HumiData[1]+ErrorAllow))||((HumiData[0]<=HumiData[1])&&(HumiData[1]<HumiData[0]+ErrorAllow))))
//����ֵ��0,����;1,��ȡʧ��
uint8_t DHT11_Read_Data(uint8_t *temp,uint8_t *humi)    
{        
 	uint8_t buf[5];
	uint8_t j;
	DHT11_Rst();
	if(DHT11_Check()==0)
	{		
		for(j=0;j<5;j++)//��ȡ4λ����
		{
			buf[j]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3]) == buf[4])
		{											
			*temp = buf[2];
			*humi = buf[0];	
		}
	}
	else 
		return 1;
	return 0;	    
}
//�����������˲�������ȥ���˵����ݣ��м��ȡƽ��
//ת��5�Σ��������У�
void DHT11_Filted(uint8_t *temp,uint8_t *humi)
{
  uint8_t i,j;
  uint8_t Link;
  uint8_t Val[2][5];  //�����ݴ�
  uint16_t Res[2]={0,0};  
  for(i=0;i<5;i++)
  {
      DHT11_Read_Data(&Val[0][i],&Val[1][i]);    
  }	 		    
	for(i=0;i<4; i++)//����
	{
		for(j=i+1;j<5;j++)
		{
			if(Val[0][i]>Val[0][j])//��������
			{
				Link = Val[0][i];
				Val[0][i] = Val[0][j];
				Val[0][j] = Link;
			}
            if(Val[1][i]>Val[1][j])//��������
			{
				Link = Val[1][i];
				Val[1][i] = Val[1][j];
				Val[1][j] = Link;
			}
		}
	}
	for(i=1;i<4;i++)
    {
        Res[0] += Val[0][i];
        Res[1] += Val[1][i];
    }
    *temp = (uint8_t)(Res[0]/3)&0XFF;
    *humi = (uint8_t)(Res[1]/3)&0XFF;
}






