/*
���˵��������оƬ�ڲ�flash��д���ܣ����ڲ�������
�����ļ�;eeprom.c   stm32f10x_flash.c

*/
//��������ʵ������Ĵ����Լ��������ʵ��
//�����ջ�Ĵ���
#include "include.h"
#include "ff.h"
uint8_t CntFlag = 0;   //ʱ�������־
//�õ�ָ��Ŀ¼�µ�bin�ļ�����
uint16_t APP_Get_Tnum(uint8_t *path)
{
	DIR tdir;  			 //��ʱĿ¼
	FILINFO tfileinfo;	//��ʱ�ļ���Ϣ
	uint8_t res;
	uint16_t rval = 0;  //��Ч���ļ�����	
	uint8_t *fn=0;
	
	tfileinfo.lfsize = _MAX_LFN*2+1;
	tfileinfo.lfname = mymalloc(tfileinfo.lfsize);
	res = f_opendir(&tdir,(const TCHAR*)path); 	      //��ָ��Ŀ¼
	if(res==FR_OK && tfileinfo.lfname != NULL)
	{
		while(f_readdir(&tdir,&tfileinfo) == FR_OK)
		{
			if(tfileinfo.fname[0]==0)
			{
				break;
			}
			fn = (uint8_t*)(*tfileinfo.lfname?tfileinfo.lfname:tfileinfo.fname);
			res= f_typetell(fn);
			if((res&0xf0)==0x00)    //�ж��Ƿ�Ϊbin�ļ���ֻ�ж�BIN�ļ�����
				rval++;		
			LED0 = !LED0;
		}
	}
	myfree(tfileinfo.lfname);
	return rval;
}
//�õ�SD����״̬
uint8_t GetSDCapcity(uint8_t x0,uint8_t y0,uint8_t width,uint8_t height,uint8_t DispFlag)
{
	BYTE res;
	uint32_t total=0,free=0;
	uint32_t Process;
	res = exf_getfree("0",&total,&free);  //�õ�SD����������С
	if(res)
	{
		Show_Str(x0,y0,100,20,"��SD������",16,0,RED,LIGHTBLUE);
		return 1;
	}
	else
	{
		total = total>>10;
		free = free>>10;
		if(DispFlag)
		{
			LCD_DrawRectangle(x0,y0, x0+width,y0+height,GRAY);  //������
			LCD_Fill(x0+1,y0+1,x0+width-1,y0+height-1,LGRAY);
			Process = (uint32_t)(total-free)*72/total;
			LCD_Fill(x0+1 ,y0+1 ,x0+1+Process+1,y0+height-1,DARKBLUE);
			
			Show_Str(x0+width,y0,100,12,"��:    MB",12,0,BLUE,LIGHTBLUE);
			LCD_ShowNum(x0+width+9,y0,total,5,12,BLACK,LIGHTBLUE);
			Show_Str(x0+width+59,y0,100,12,"ʣ:    MB",12,0,BLUE,LIGHTBLUE);
			LCD_ShowNum(x0+width+70,y0,free,5,12,BLACK,LIGHTBLUE);
		}
		return 0;		
	}
}
//����APP
void UpdataApp(uint8_t *path)
{
	uint8_t res=0;
	LCD_Fill(6,31,472,310,BLACK);
	Show_Str(195,220,300,16,"�̼����ڸ���",16,0,GREEN,BLACK);
	LCDDrawCir(240,140,40,RED,0,0);
	res = IAP_Update_Data(path,220,120);  //������Ϊ��ʾ����ֵ������
	switch(res)
	{
		case 0x00:
		{
			Show_Str(195,220,300,16,"�̼��������",16,0,GREEN,BLACK);
			if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
			{
				Show_Str(195,220,200,16,"APP������......",16,0,GREEN,BLACK);
				iap_load_app(FLASH_APP1_ADDR);
			}
			else
				Show_Str(195,220,200,16,"App����ʧ��",16,0,RED,BLACK);
		}break;
		case 0x01:Show_Str(195,220,300,16,"�ڴ�����ʧ��",16,0,RED,BLACK);break;
	    case 0x02:Show_Str(195,220,300,16,"�ļ�����ʧ��",16,0,RED,BLACK);break;
		case 0x03:Show_Str(195,220,300,16,"�ļ���ʧ�ܻ����ļ�",16,0,RED,BLACK);break;
	}
}
//������ת
void ProgramJump(void)
{                                     
	if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
	{
		Show_Str(195,220,200,16,"APP������......",16,0,GREEN,BLACK);
		iap_load_app(FLASH_APP1_ADDR);
	}	
	else
	{
		Show_Str(195,220,200,16,"APP��תʧ��",16,0,RED,BLACK);
		Close_FDN304P();
	}
}
void Load_AppUpdate_UI(void)
{
	//LCD_Clear(LIGHTBLUE);
	LCD_Fill(0,0,480,320,LIGHTBLUE);
	LCD_DrawRectangle(5,30,473,311,BLUE);  //������
	LCD_Fill(6,31,472,310,BLACK);
	LCD_DrawLine(240, 32, 240, 311,BLUE);
	Show_Str(1,0,200,24,"ϵͳAPP����",24,0,BLUE,LIGHTBLUE);
	Show_Str(350,16,200,12,"(�̰����£������˳�)",12,0,RED,LIGHTBLUE);
	GetSDCapcity(140,4,200,20,1);
}
//������
int main(void)
{
	DIR appdir;   //appĿ¼
	FILINFO appfileinfo;//app�ļ���Ϣ
	uint8_t *pname;  //��·�����ļ���
	uint8_t curindex = 0;  //��ǰ����
	uint8_t precurindex = 0;
	uint16_t totappnum = 0; 		//�ܵ�app�ļ�����
	uint8_t *appindextbl;
	uint8_t *fn=0;					//���ļ���    
	uint8_t res;
	uint8_t orUpdateCnt = 0;
	uint16_t Cnt = 0;
	uint8_t key;
	uint8_t PageNum = 0;  //ҳ��
	uint8_t FileExistFlag = 0;  //�Ƿ����ļ����ڱ�־
	sys_clock_init(9);   		             //��ʼ���׶�	
	delay_init();      		                //��ʱ��ʼ�� 
	StartMachineInit();                   //��������ʼ��
	uart_init(115200,9600,9600); 
	NVIC_Configuration();                    //�����жϷ���
	EncoderInit();
	if(!KeyScan())  //ֻ�ܷ���0
	{
		TFTLCD_Init();      					//������ʼ��    	
		//KEY_Init();
		LED_Init();
		mem_init();								//�ڴ�س�ʼ��
		exfuns_init();
		f_mount(fs[0],"0:",1); 	//����SD�� 
		f_mount(fs[1],"1:",1); 	//����FLASH.    
		while(Font_Init())						//�ֿ��ʼ������Ƿ�OK
		{
	UPD:	
			LCD_Fill(0,0,480,320,LIGHTBLUE);
			while(SD_Initialize()&&Cnt<40)	//���SD��
			{
				LCD_ShowString(180,80,200,24,24,"SD Card Check Failed!",GREEN,LIGHTBLUE); //��ʾSD�Ƿ������Ϣ
				delay_ms(200);
				LCD_Fill(175,75,175+150,75+40,LIGHTBLUE);
				delay_ms(200);
				Cnt++;
			}
			Cnt = 0;
			LCD_ShowString(180,80,200,24,24,"SD Card OK",BLACK,LIGHTBLUE);
			LCD_ShowString(180,120,200,24,24,"Font Updating...",BLACK,LIGHTBLUE);
			key=update_font(150,160,24);	//�����ֿ�
			while(key&&Cnt<10)			//����ʧ��
			{
				LCD_ShowString(180,120,200,24,24,"Font Update Failed!",BLACK,LIGHTBLUE);
				delay_ms(200);
				LCD_Fill(175,115,175+150,115+40,LIGHTBLUE);
				delay_ms(200);	
				Cnt++;			
			}
			LCD_ShowString(180,120,200,24,24,"Font Update Success!",BLACK,LIGHTBLUE);
			delay_ms(1500);	
			LCD_Clear(BLACK);//����
			break;
		}
		Cnt = 0;
		//��������ѡ��ģʽ
		if(!GetSDCapcity(195,195,200,30,0))  //����̼�����ģʽ
		{
			Load_AppUpdate_UI();
			totappnum = APP_Get_Tnum("0:/APPFILE");
			if(totappnum>34)  //�ܹ�������34��BIN�ļ�
				totappnum = 34;
			PageNum = totappnum/12+1;  //���ҳ��
			appfileinfo.lfsize = _MAX_LFN*2+1;						//���ļ�����󳤶�
			appfileinfo.lfname = mymalloc(appfileinfo.lfsize);
			pname = mymalloc(appfileinfo.lfsize);				//Ϊ��·�����ļ��������ڴ�		
			appindextbl = (uint8_t*)mymalloc(2*totappnum);				//����2*totpicnum���ֽڵ��ڴ�,���ڴ��app����
			if(appfileinfo.lfname==NULL||pname==NULL)//�ڴ�������
			{
				Show_Str(180,160,200,16,"�ڴ����ʧ��",16,0,RED,LIGHTBLUE);
				return 0;
			}
			curindex = 0;
			res=f_opendir(&appdir,"0:/APPFILE"); 	//��Ŀ¼
			while(res == FR_OK)
			{
				//dir_sdi(&appdir,appindextbl[curindex]);//�ı䵱ǰĿ¼����
				res = f_readdir(&appdir,&appfileinfo);		
				if(res!=FR_OK||appfileinfo.fname[0]==0)
				{
					break;
				}
				else 
				{
					fn = (uint8_t*)(*appfileinfo.lfname?appfileinfo.lfname:appfileinfo.fname);
					res = f_typetell(fn);
					if((res&0xf0)==0x00)  //��bin�ļ��Ļ�
					{				
						curindex ++;
						if(curindex>totappnum)
							break;
						appindextbl[curindex] = appdir.index;			
						if(curindex<=17)
						{
							strcpy((char*)pname,"SD:");				//����·��(Ŀ¼)
							strcat((char*)pname,(const char*)fn);  			//���ļ������ں���
							Show_Str(25,32+(Cnt++)*16,215,16,pname,16,0,GREEN,BLACK);	//��ʾ�ļ�����
							if(Cnt==17)
								Cnt = 0;
						}		
						else				
						{
							strcpy((char*)pname,"SD:");				//����·��(Ŀ¼)
							strcat((char*)pname,(const char*)fn);  			//���ļ������ں���
							Show_Str(266,32+(Cnt++)*16,215,16,pname,16,0,GREEN,BLACK);//��ʾ�ļ�����
						}
					}
				}
				res = 0;
			}
			if(curindex == 0)
				FileExistFlag = 0;
			else
				FileExistFlag = 1;
			curindex = 0;
			Show_Str(7,32,32,16,"->",16,0,RED,BLACK);
			while(FileExistFlag)
			{
				//key = KeyScan();
				if(Encoder_TYPE!=Encoder_Void && Encoder_TYPE!= Encoder_Pressed)  //���û�а�����ת���Ļ�
				{
					if(Encoder_TYPE==Encoder_Forword)
					{
						curindex ++;
						if(curindex>=totappnum)
						{
							curindex = 0;
							if(totappnum>17)
								Show_Str(243,32+(totappnum-1-17)*16,32,16,"  ",16,0,RED,BLACK);
						}
					}
					else if(Encoder_TYPE==Encoder_Backword)
					{
						curindex --;
						if(curindex==255)
							curindex = totappnum-1;
					}
					
					if(curindex <= 16)   //��ǰ����Ϊ��һҳ
					{
						if(totappnum >1)
						{
							Show_Str(7,32+curindex*16,32,16,"->",16,0,RED,BLACK);
							if(precurindex==17)
								Show_Str(243,32+(precurindex-17)*16,32,16,"  ",16,0,RED,BLACK);
							else
								Show_Str(7,32+precurindex*16,32,16,"  ",16,0,RED,BLACK);
						}
						else
							Show_Str(7,32,32,16,"->",16,0,RED,BLACK);
						Encoder_TYPE = Encoder_Void;
					}
					else if(curindex>16 && curindex<totappnum)
					{
						if(curindex==totappnum-1)  //�����ж��ڵ�һ����ת�����һ��ʱ�����һ������ʾ
						{
							if(Encoder_TYPE==Encoder_Backword)
								Show_Str(7,32+precurindex*16,32,16,"  ",16,0,RED,BLACK);
						}
						if(precurindex==16)
						{
							if(Encoder_TYPE == Encoder_Forword)
								Show_Str(7,32+precurindex*16,32,16,"  ",16,0,RED,BLACK);
						}
						else
							Show_Str(243,32+(precurindex-17)*16,32,16,"  ",16,0,RED,BLACK);
						if(precurindex == 17)
						{
							if(Encoder_TYPE == Encoder_Backword)
								Show_Str(243,32+(precurindex-17)*16,32,16,"  ",16,0,RED,BLACK);
						}
						Show_Str(243,32+(curindex-17)*16,32,16,"->",16,0,RED,BLACK);
						Encoder_TYPE = Encoder_Void;
					}
					else
					{
						//curindex = 0;
						if(totappnum>17)
						{
							Show_Str(7,32,32,16,"->",16,0,RED,BLACK);
							Show_Str(243,32+(totappnum-1-17)*16,32,16,"  ",16,0,RED,BLACK);
						}
						Encoder_TYPE = Encoder_Void;
					}
					precurindex = curindex;
				}
				else if(Encoder_TYPE == Encoder_Pressed)  //�����������ť�Ļ�
				{
					while(orUpdateCnt++<100 && GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0)
					{
						delay_ms(30);
					}
					if(orUpdateCnt >= 100)
					{
						FileExistFlag = 0;
						break;
					}
					dir_sdi(&appdir,appindextbl[curindex]);//�ı䵱ǰĿ¼����
					res = f_readdir(&appdir,&appfileinfo);
					fn = (uint8_t*)(*appfileinfo.lfname?appfileinfo.lfname:appfileinfo.fname);
					strcpy((char*)pname,"0:/APPFILE/");				//����·��(Ŀ¼)
					strcat((char*)pname,(const char*)fn);  			//���ļ������ں���
					break;   //����ִ��
				}
			}	
			if(FileExistFlag)
				UpdataApp(pname);    //����ϵͳappȻ����ת
			else
			{
				LCD_Fill(6,31,472,310,BLACK);
				if(orUpdateCnt < 100)
					Show_Str(180,160,200,24,"�ļ�������",24,0,RED,BLACK);
				else if(orUpdateCnt >= 100)
					Show_Str(180,160,200,24,"�޸���",24,0,RED,BLACK);
			}
			delay_ms(1000);
		}
	}
	ProgramJump();  //��ת��APP����
}
/*
if(curindex > totappnum-1)
{
	curindex = 0;//��ĩβ��ʱ��,�Զ���ͷ��ʼ
	if(totappnum>17)
		Show_Str(243,32+(totappnum-1-17)*16,32,16,"  ",16,0,RED,BLACK);
}
if(curindex<=16)
{					
	Show_Str(7,32+curindex*16,32,16,"->",16,0,RED,BLACK);
	Show_Str(7,32+precurindex*16,32,16,"  ",16,0,RED,BLACK);
}
else
{
	if(curindex==17)
		Show_Str(7,32+16*16,32,16,"  ",16,0,RED,BLACK);  //�����������һ��->
	else
		Show_Str(243,32+(precurindex-17)*16,32,16,"  ",16,0,RED,BLACK);
	Show_Str(243,32+(curindex-17)*16,32,16,"->",16,0,RED,BLACK);
}
precurindex = curindex;


if(totappnum>17)
	Show_Str(243,32+(totappnum-1-17)*16,32,16,"  ",16,0,RED,BLACK);


*/











//end of file....


