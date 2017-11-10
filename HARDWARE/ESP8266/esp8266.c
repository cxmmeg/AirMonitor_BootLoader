#ifndef _ESP8266_C
#define _ESP8266_C	
#include "include.h"	
//STMflash��д������������
uint8_t Flash_Buffer[50];      //flash������50���ֽ�
#define SIZE sizeof(Flash_Buffer)	 			  	//���鳤��

//��ΪIP:121.40.22.15
//esp8266Wifiģ����������
//��������ͨ��Э�飬IP��ַ���˿ں�

//��Ϊ��������Կ
uint8_t UserKey[60];
Esp8266 Esp8266Config={{"",""},{"TCP"},{""},8888,0,1,0};           //�ṹ��ʵ�廯
NET NetData;  				//������յ�������
uint8_t ReConfigFlag = 0XFF;  //�Ƿ���������WIFI
//��������Esp8266ģ�飬����Ƿ���λ�ɹ�
//����:�����ȶ�ȡFLASH���õ�ֵ��ʼ�������ӣ���������յ���λ�������������ݣ���ʼ����
void Esp8266_Config(void)
{
	uint8_t i;
	uint8_t *p=0;
	uint8_t *p1=0;
    uint8_t buf[20];
    uint8_t ConfigStep = 1;    //���ò���
	uint8_t ConfigWriteFlag = 0XF0;
	uint8_t ErrorType;
	uint8_t ConfigStat;
    uint8_t SuccesFlag = 0;  //���ӳɹ���־
    uint8_t LinkTimes = 0;
	//LCD_Clear(BLACK);   //����
	LCD_Fill(0,30,320,240,BLACK);
	ConfigStat = AT24CXX_ReadOneByte(0);
	if(KeyScan()||ConfigStat!=0XF7)  //���Ҫ�������õĻ�ִ��
	{
		switch(ConfigStat)
		{
			case 0xf6:ConfigStep =1;Show_Str(60,120,200,16,"����������WIFI->",16,0,GREEN,BLACK);break;
			case 0xf5:ConfigStep =2;Show_Str(60,140,200,16,"����������USERKEY->",16,0,GREEN,BLACK);break;
			case 0xf3:ConfigStep =3;Show_Str(60,160,200,16,"����������ServerIP->",16,0,GREEN,BLACK);break;
		}		
		ConfigServerMode();  //����Ϊ������ģʽ
		Show_Str(60,120,200,16,"����������WIFI->",16,0,GREEN,BLACK);  //
		p = UartRecBuf;
		while(1)
		{
			if(WIFIRecDataFlag)
			{
				WIFIRecDataFlag = 0;
				delay_ms(100);
				switch(ConfigStep)
				{
					case 1:
					{
						if((strstr(p,"%SSID")!=NULL)&&(strstr(p,"&#")!=NULL))
						{
							Show_Str(200,120,200,16,"д����...  ",16,0,GREEN,BLACK);
							p1 = strstr((const char*)p,"%SSID");
							AT24CXX_Write(1,p1,150);  		//����wifi��SSID������
							ConfigWriteFlag |= (1<<0);
							AT24CXX_WriteOneByte(0,ConfigWriteFlag);
							Show_Str(200,120,200,16,"WIFI������",16,0,GREEN,BLACK);
							Show_Str(60,140,200,16,"����������USERKEY->",16,0,GREEN,BLACK);
							ConfigStep ++;
						}
						else
						{
							Show_Str(60,120,200,16,"WIFIδ����",16,0,GREEN,BLACK);							
						}
						ClearBuf();
					}break;
					case 2:
					{
						if((strstr(p,"%USERKEY")!=NULL)&&(strstr(p,"&#")!=NULL))
						{
							Show_Str(210,140,200,16,"д����...  ",16,0,GREEN,BLACK);
							p1 = strstr((const char*)p,"%USERKEY");
							AT24CXX_Write(151,p1,50);
							ConfigWriteFlag |= (1<<1);
							AT24CXX_WriteOneByte(0,ConfigWriteFlag);
							Show_Str(210,140,200,16,"USERKEY������",16,0,GREEN,BLACK);
							Show_Str(60,160,200,16,"����������ServerIP->",16,0,GREEN,BLACK);
							ConfigStep ++;
						}
						else
						{
							Show_Str(210,140,200,16,"USERKEYδ����",16,0,GREEN,BLACK);						
						} 
						ClearBuf();												
					}break;
					case 3:
					{
						if((strstr(p,"%SVN")!=NULL)&&(strstr(p,"&#") != NULL))
						{
							Show_Str(210,160,200,16,"д����...  ",16,0,GREEN,BLACK);
							p1 = strstr((const char*)p,"%SVN");
							AT24CXX_Write(202,p1,30);
							ConfigWriteFlag |= (1<<2);
							AT24CXX_WriteOneByte(0,ConfigWriteFlag);
							Show_Str(210,160,200,16,"SVN������",16,0,GREEN,BLACK);
							Show_Str(60,180,200,16,"�������",16,0,GREEN,BLACK);
							ConfigStep ++;
						}
						else
						{
							Show_Str(210,160,200,16,"SVNδ����",16,0,GREEN,BLACK);						
						}
						ClearBuf();	
					}break;
					default:break;
				}
				if(ConfigStep>3)
					break;
				LED0 = !LED0;
			}
		}
	}
	//LCD_Clear(BLACK);
	LCD_Fill(0,20,320,240,BLACK);
    if(!Esp8266_ReStart())  //�������������ź�
    {
		Show_Str(60,40,200,16,"WIFI�����ɹ�.",16,0,GREEN,BLACK);
        Esp8266Config.WifiStartFlag = 1;
    }
	else
	{
		Show_Str(60,40,200,16,"WIFI����ʧ��.",16,0,RED,BLACK);
        Esp8266Config.WifiStartFlag = 0;
		return;
	}
	delay_ms(500);
    if(!Esp8266_SetMode(0))   //����ģʽ
		Show_Str(60,60,200,16,"TCPģʽ���óɹ�.",16,0,GREEN,BLACK);
	else
	{
		Show_Str(60,60,200,16,"TCPģʽ����ʧ��.",16,0,RED,BLACK);
        return;
	}
	delay_ms(500);
	if(!Esp8266_ReStart())
		Show_Str(60,80,200,16,"WIFI�����ɹ�.",16,0,GREEN,BLACK);
	else
	{
		Show_Str(60,80,200,16,"WIFI����ʧ��.",16,0,RED,BLACK);
        return;
	}
	delay_ms(500);
	if(!Esp8266_CheckNET())      //��鸽������
	{
		Show_Str(60,100,200,16,"�������ɹ�.",16,0,GREEN,BLACK);
	}
	else
	{
		Show_Str(60,100,200,16,"�������ʧ��.",16,0,RED,BLACK);
        return;
	}
    //��ʼ����WIFIģ��
	Show_Str(60,120,200,16,"WIFI������...",16,0,GREEN,BLACK);
	while(LinkTimes<3) //�������10��
	{
		 LinkTimes ++;
		 if(!WifiNETSet()&&SuccesFlag==0)      //�������ã�������óɹ��Ļ���ִ����Ӧ����
		 {
             SuccesFlag =1;
			 Show_Str(60,140,200,16,"WIFI���óɹ�",16,0,GREEN,BLACK);
		 }
		 else
		 {
			 Show_Str(60,140,200,16,"����������.",16,0,GREEN,BLACK);
		 }
		 if(SuccesFlag==1)             //���ӳɹ��Ļ�
		 {
			   	Show_Str(60,160,200,16,"WIFI������...",16,0,GREEN,BLACK);
				if(!Esp8266_ConnectWire())   //��ʼ����WIFI�����ӳɹ��Ļ�
				{
					Esp8266Config.WifiLinkSuccFlag = 1;  //��־WIFI���ӳɹ�
					Show_Str(60,180,200,16,"WIFI���ӳɹ�",16,0,GREEN,BLACK);
					for(i=0;i<2;i++)
					{
						 LED0 = !LED0;
						 delay_ms(500);
					}
					break;
				}
			 	else//��������δ�ɹ�
				{
					SuccesFlag=0;
					Esp8266Config.WifiLinkSuccFlag = 0;
					Show_Str(60,180,200,16,"WIFI����ʧ��",16,0,RED,BLACK);
				}
		 }
	}
	if(LinkTimes>=100)
		Show_Str(60,180,200,16,"WIFI����ʧ��",16,0,RED,BLACK);
////////////////////////////////////////////////////////////////////
	Esp8266_BackIPAdd(); //����ģ���IP��ַ
	if(!Esp8266_OpenMoreConect())
		Show_Str(60,200,200,16,"�����Ӵ򿪳ɹ�",16,0,GREEN,BLACK);
	else
	{
		Show_Str(60,200,200,16,"�����Ӵ�ʧ��",16,0,RED,BLACK);
		return;
	}
	delay_ms(500);
	//LCD_Clear(BLACK);   //����
	LCD_Fill(0,27,320,240,BLACK);
	Show_Str(60,30,200,16,"������������...",16,0,GREEN,BLACK);
	ErrorType = ReadServerPar();
	if(!ErrorType)   //��EEPROM��ȡ������������
	{
		if(!Esp8266_ConectServer())
		{
			Esp8266Config.ServerLinkStat = 1;
			Show_Str(60,30,200,16,"���������ӳɹ�",16,0,GREEN,BLACK);
		}
		else
		{
			Esp8266Config.ServerLinkStat = 0; 
			Show_Str(60,30,200,16,"����������ʧ��",16,0,RED,BLACK);
			delay_ms(500);
			return;
		}
	}
	else
	{
		//Show_Str(60,30,200,16,"������ȡ����  ",16,0,RED,BLACK);
		LCD_ShowNum(60,30,ErrorType,2,16,RED,BLACK);
	}
	sprintf(buf,"������IP:%s",Esp8266Config.ServerIP);
	Show_Str(60,50,200,16,buf,16,0,GREEN,BLACK);
	sprintf(buf,"������PORT:%d",8080);
	Show_Str(60,70,200,16,buf,16,0,GREEN,BLACK);
	ClearBuf();
	delay_ms(2000);
}
//�����������ӷ�����
void TryReConectServer(void)
{
	if(CurrentuiID==1||CurrentuiID==2)  //���������������֮��Ļ�����ʾ
		Show_Str(5,3,100,12,"������������",12,0,GREEN,BLACK);
	if(!Esp8266_ConectServer())
	{
		if(CurrentuiID==1||CurrentuiID==2)
			Show_Str(5,3,100,12,"������������",12,0,GREEN,BLACK);
		Esp8266Config.ServerLinkStat = 1;
	}
	else
	{
		if(CurrentuiID==1||CurrentuiID==2)
			Show_Str(5,3,100,12,"������δ����",12,0,RED,BLACK);
		Esp8266Config.ServerLinkStat = 0;
	}
}
//������������WIFI
void TryReConectWifi(void)
{
	if(!Esp8266_ConnectWire())  //���ӳɹ��Ļ�
	{
		Esp8266Config.WifiLinkSuccFlag = 1;  //��־���ӳɹ�
	}
	else
	{
		Esp8266Config.WifiLinkSuccFlag = 0;
	}
}
//���ESP8266�ĵ�ǰ����״̬
uint8_t GetEsp8266CurrStat(void)
{
	ClearBuf();
	printf("AT+CIPSTATUS\r\n");
	if(!Hand("OK",WaitTime))             //����ɹ�
	{
		return 0;
	}
   else
   {
	   return 1;
   }
}
//ģ���������� ATָ��   AT+RST
//����ֵ��0�ɹ�  1ʧ��
uint8_t Esp8266_ReStart(void)
{
	 ClearBuf();
	 printf("AT+RST\r\n");        //��������
	 if(!Hand("OK",WaitTime))             //����ɹ�
		 return 0;	   
   else
	   return 1;
}
//����ģ��Ĺ�����ʽ
//STģʽ
//APģʽ
//ST+APģʽ
//����ֵ�� 0�ɹ�  1ʧ��
uint8_t  Esp8266_SetMode(uint8_t Mode)
{
    ClearBuf();		
	if(Mode)
		printf("AT+CWMODE=2\r\n");    //����APģʽ
	else
		printf("AT+CWMODE=3\r\n");
    if(!Hand("OK",WaitTime))  //����ɹ�
    {
        delay_ms(500);
        ClearBuf();
        printf("AT+RST\r\n");         //������Ч
        if(!Hand("OK",WaitTime))  //���ֳɹ��Ļ�
        {
            return 0;
        }
        else
          return 1;  //ʧ��
    }	
    else
    return 1;			
}
//�鿴��������������
//����ֵ��0�ɹ�   1ʧ��
uint8_t Esp8266_CheckNET(void)
{
    ClearBuf();
    printf("AT+CWLAP\r\n");       //�鿴���õ����硣���ص�ǰ���ӵ�����
    if(!Hand("OK",WaitTime))
        return 0;
    else
        return 1; 
}
//����ģ���IP��ַ˵���Ѿ�����·���� ����OK
void Esp8266_BackIPAdd(void)
{
    ClearBuf();
    printf("AT+CIFSR\r\n");      //���ͷ���ģ���IP��ַ
    Hand("OK",WaitTime);         //�ȴ���Ӧ�ɹ�
}
//"%SSID=" + splitstring1 + "&%" + splitstring2 + "&#";
//����Ҫ���ӵ���������
//����ֵ�� 0�ɹ� 1ʧ��
uint8_t Esp8266_ConnectWire(void)
{
    ClearBuf();
    //sprintf();
    printf("AT+CWJAP=\"%s\",\"%s\"\r\n",Esp8266Config.WireNet[0],Esp8266Config.WireNet[1]);  //������������ISSD���������� 
    if(!Hand("OK",1000))
        return 0;
    else
        return 1; 
}
//ģ�鿪��������
//����ֵ�� 0�ɹ� 1ʧ��
uint8_t Esp8266_OpenMoreConect(void)
{
    ClearBuf();
    printf("AT+CIPMUX=1\r\n");
    if(!Hand("OK",WaitTime))
        return 0;
    else
        return 1;
}
//��ʼҪ���ӵķ�������IP��ַ�Ͷ˿ں�
//��ڲ�����Proto��ͨ��Э��  IP:������IP��ַ  Port �������˿ں�
//����ֵ; 0�ɹ� 1ʧ��
uint8_t Esp8266_ConectServer(void)
{
    ClearBuf();
    printf("AT+CIPSTART=%d,\"%s\",\"%s\",%d\r\n",Esp8266Config.SlrID,Esp8266Config.ComunitPro,Esp8266Config.ServerIP,Esp8266Config.ServerPort); //����ָ��
    if(!Hand("OK",1000))
        return 0;
    else
        return 1;
}
//�Ͽ�����
//����ֵ�� 0�ɹ� 1ʧ��
uint8_t Esp8266_DisConectServer(void)
{
    ClearBuf();
    printf("AT+CIPCLOSE=%d\r\n",Esp8266Config.SlrID);
    if(!Hand("OK",WaitTime))
        return 0;
    else
        return 1; 
}
//
//����ģ��ΪServerģʽ
//
uint8_t Esp8266_SetServerMode(uint16_t Port)
{
	ClearBuf();
	printf("AT+CIPSERVER=1,8080\r\n");
	if(!Hand("OK",WaitTime))
		return 0;
	else
		return 1; 
}
//192.168.4.1
uint8_t GetIPAddr(void)
{
	ClearBuf();
	printf("AT+CIFSR\r\n");
	if(!Hand("OK",WaitTime))
		return 0;
	else
		return 1;
}
//������������
//+IPD,0,10:1234567890
//UartRecBuf[50]
//����ֵ��ģ���IDֵ
uint8_t ListenRec(void)
{
    if(WIFIRecDataFlag)  //���յ��˷���������
    {
        WIFIRecDataFlag=0;
        if(strstr(UartRecBuf,"+IPD")!=NULL)
        {
             return 0;
        }
    }
    else
        return 1; //����ģ���IDֵ
}
//����������Ϣ
//���������*DataҪ���͵���������Len Ҫ���͵����ݵ��ֽڳ���
//����ֵ; 0�ɹ� 1ʧ��
uint8_t Esp8266_CIPSendData(char *Data,uint16_t Len)
{
    uint8_t key;
    //uint8_t MAClen;
    //MAClen = strlen(MAC);
    ClearBuf();
    printf("AT+CIPSEND=%d,%d\r\n",Esp8266Config.SlrID,Len);
    if(!Hand(">",WaitTime))  //���ֳɹ�
    {
        ClearBuf();
        printf("%s\r\n",Data);	 
        key=Hand("SEND OK",WaitTime);
        if(!key)  //ȷ�����ݷ��ͳɹ�	
        {
            ClearBuf();  //������ݻ�����
            return 0;
        }
        else
            return 1;
    }
    else
    return 1;
}
//������	���㺯��
//�����������������ַ
void ClearBuffer(uint8_t *data)
{
    uint8_t i,len;
    len=strlen(data);
    for(i=0;i<len;i++)
    {
        data[i]=0;
    }
}
//������������
//���������*aҪ�Ƚϵ��ı�ֵ    Time �ȴ���ʱʱ��
//����ֵ��0�ɹ�  1ʧ��
uint8_t Hand(char *a,uint16_t Time)
{
    while(Time--)
    {
        if(strstr(UartRecBuf,a)!=NULL)
        {
            return 0;
            break;
        }
        delay_ms(10);
    }
    return 1;
}
//��ȡ����������
uint8_t ReadServerPar(void)
{
	uint8_t i;
	uint8_t ConfigStat;		//����״̬
	//uint8_t buf[50];
	uint8_t *Start;   //�ַ�����ʼ�ͽ�����ָ��λ��
	uint8_t StrLen;
	ConfigStat = AT24CXX_ReadOneByte(0);
	if((ConfigStat&0x02)==0x02)
	{
		ClearBuf();
		delay_ms(100);
		AT24CXX_Read(151,UartRecBuf,50);
		if(strstr(UartRecBuf,"%USER")==NULL)  //�����û�з���ָ���ַ����򷵻�
		{ 
			return 0X01;
		}
		else
		{
			StrLen = (strstr(UartRecBuf,"&#")) - (strstr(UartRecBuf,"%USER")+9);
			Start = strstr(UartRecBuf,"%USER") + 9;
			memcpy(UserKey,Start,StrLen);
		}
	}
	if((ConfigStat&0x04)==0x04)
	{
		ClearBuf();		
		delay_ms(100);
		AT24CXX_Read(202,UartRecBuf,30);
		if(strstr(UartRecBuf,"%SVN")==NULL)
		{
			ClearBuf();
			return 0X02;
		}
		else
		{
			StrLen = (strstr(UartRecBuf,"&%")) - (strstr(UartRecBuf,"%SVN")+5);
			Start = strstr(UartRecBuf,"%SVN") +5;
			for(i=0;i<StrLen;i++)
			{
				Esp8266Config.ServerIP[i] = *Start++;
			}
			StrLen = (strstr(UartRecBuf,"&#")) - (strstr(UartRecBuf,"&%")+2);
			Start = strstr(UartRecBuf,"&%") +2;
			Esp8266Config.ServerPort = (*Start-0x30)*1000+(*(Start+1)-0x30)*100+(*(Start+2)-0x30)*10+(*(Start+3)-0x30);
		}
	}
	return 0;
}
//Wifi�������ô���
//���ݸ�ʽ��%SSID=guchenglong&%gcl19910621&#
//      7          18
//%SSID=TP-LINK_601&%qq18789463526qq&#
uint8_t WifiNETSet(void)
{
    uint8_t i;
	uint8_t ConfigStat;
    uint8_t *WifiSSID=0;                 //Wifi���ƺ�����ָ�����
    uint8_t *Wifisecret=0;
    uint8_t Dist1;
    uint8_t Dist2;
	ConfigStat = AT24CXX_ReadOneByte(0);  //��ȡ�Ƿ����ñ�־
	if((ConfigStat&0X01)==0X01)   				  //������
	{
		ReConfigFlag = 0;
		ClearBuf(); //��ջ���
		delay_ms(100);
		AT24CXX_Read(1,UartRecBuf,150);  		//����wifi��SSID������
	}
	else
		return 1; //����״̬
    if(strstr(UartRecBuf,"%SSID")==NULL)  //�����û�з���ָ���ַ����򷵻�
    {
         ClearBuf();            //���������
         return 1;
    }
    WifiSSID = strstr(UartRecBuf,"%SSID")+6;       //ָ��SSID��
    Wifisecret = strstr(UartRecBuf,"&%")+2;  //ָ�������ַ��
    Dist1 =  strstr(UartRecBuf,"&%") - (strstr(UartRecBuf,"%SSID")+6);    //������˺ó�ʱ�����ں��ˣ�SSID���ֵ�ֵ
    Dist2 = strstr(UartRecBuf,"&#") - (strstr(UartRecBuf,"&%")+2);      //���뵽������
    if(*(WifiSSID-6)=='%')
    {
        for(i=0;i<Dist1;i++)
        {
            Esp8266Config.WireNet[0][i] = *WifiSSID; //����SSID
            WifiSSID++;		
        }
        for(i=0;i<Dist2;i++)
        {
            Esp8266Config.WireNet[1][i] = *Wifisecret;  //��������
            Wifisecret++;
        }
        return 0;
    }
    else
    {
        ClearBuf();                   					//��ô�н��ܵ���־�ַ�������ջ�����
        return 1;
    }
}
//����ģ��Ϊ������ģʽ
void ConfigServerMode(void)
{	
	uint8_t *pIP =0;
	LCD_Clear(BLACK);//����
	if(!Esp8266_ReStart())  //�������������ź�
    {
		Show_Str(60,0,200,16,"WIFI�����ɹ�",16,0,GREEN,BLACK);
	}
	else
	{
		Show_Str(60,0,200,16,"WIFI����ʧ��",16,0,RED,BLACK);
		return;
	}
	delay_ms(500);
	if(!Esp8266_SetMode(1))   		//����ΪAPģʽ
		Show_Str(60,20,200,16,"APģʽ���óɹ�",16,0,GREEN,BLACK);
	else
	{
		Show_Str(60,20,200,16,"APģʽ����ʧ��",16,0,RED,BLACK);
		return;
	}
	delay_ms(500);
	if(!Esp8266_OpenMoreConect())	//�򿪶�����
		Show_Str(60,40,200,16,"�����Ӵ򿪳ɹ�",16,0,GREEN,BLACK);
	else
	{
		Show_Str(60,40,200,16,"�����Ӵ�ʧ��",16,0,RED,BLACK);
		return;
	}
	delay_ms(500);
	if(!Esp8266_SetServerMode(8080))  //���÷�����ģʽ	
		Show_Str(60,60,200,16,"���ط����������ɹ�",16,0,GREEN,BLACK);
	else
	{
		Show_Str(60,60,200,16,"���ط���������ʧ��.",16,0,RED,BLACK);
		return;
	}	
	delay_ms(500);
	GetIPAddr();
	delay_ms(500);
	pIP = strstr((char*)UartRecBuf,"+CIFSR:APIP") +12;
	Show_Str(110,80,200,16,pIP,16,0,GREEN,BLACK);
	Show_Str(60,80,100,16,"����IP:",16,0,GREEN,BLACK);
	Show_Str(60,100,200,16,"�˿�:8080.",16,0,GREEN,BLACK);
	ClearBuf();
}
//�������ݴ���
//����������ṹ��  ������  PWMֵ
//�������ݣ�+IPD,0,10:���������
//��Ϣ���ݸ�ʽ: <�������ݸ�ʽ>  �ָ���  &         %SSID=" + splitstring1 + "&%" + splitstring2 + "&#
void NETDataDeal(void)
{
    uint8_t i;
    uint8_t *DataP;
    uint16_t num=0;
    if(strstr(UartRecBuf,"S1ON")!=NULL)
    {		
		LCD_ShowString(128,190,200,16,16,"S1 ON",GREEN,BLACK);
    }
    if(strstr(UartRecBuf,"S1OFF")!=NULL)
    {
		LCD_ShowString(128,190,200,16,16,"S1 OFF",GREEN,BLACK);
    }
    if(strstr(UartRecBuf,"S2ON")!=NULL)
    {
		LCD_ShowString(128,190,200,16,16,"S2 ON",GREEN,BLACK);
    }
    if(strstr(UartRecBuf,"S2OFF")!=NULL)
    {
		LCD_ShowString(128,190,200,16,16,"S2 OFF",GREEN,BLACK);
    }  
    if(strstr(UartRecBuf,"Val1=")!=NULL)
    {
        DataP = strstr(UartRecBuf,"Val1=") +5;    //ָ���Ƶ����ݴ�
        for(i=0;i<strlen(DataP)-6;i++)
        {
            if(*DataP!='&')
            {
                num=num*10+*DataP-'0';
                DataP++;
            }
            else
                break;
        }			 
//        Lcd12864PutNum(4,3,num);
        num=0;
    } 
    if(strstr(UartRecBuf,"Val2=")!=NULL)
    {
        DataP = strstr(UartRecBuf,"Val2=") +5;    //ָ���Ƶ����ݴ�
        for(i=0;i<strlen(DataP)-6;i++)
        {
            if(*DataP!='&')
            {
                num=num*10+*DataP-'0';
                DataP++;
            }
        else
            break;
        }
//        Lcd12864PutNum(6,3,num);
        num=0;
    } 
    Esp8266_CIPSendData("Received Success",strlen("Received Success"));	 
    ClearBuf();
}
#endif

