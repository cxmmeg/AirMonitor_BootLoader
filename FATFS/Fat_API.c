#include "sys.h"
#include "sd.h"
#include  "ff.h"
//#include "ucos_ii.h"
#include "Fat_API.h"
#include "malloc.h"
//#include "include.h"
FATFS *fs[2];  //Ϊ�߼��������빤����

//�ļ������б�
const uint8_t *FILE_TYPE_TBL[4][13]=
{
{"BIN"},			//BIN�ļ�
{"INI"},			//LRC�ļ�
{"TXT","C","H"},	//�ı��ļ�
{"BMP","JPG","JPEG","GIF"},//ͼƬ�ļ�
};

//Ϊ�ļ�ϵͳ�����ڴ�
uint8_t exfuns_init(void)
{
	fs[0] =  (FATFS*)mymalloc(sizeof(FATFS)); //Ϊsd�������������ڴ�	
	fs[1] =  (FATFS*)mymalloc(sizeof(FATFS));//ΪFLASH�����������ڴ�	
	if(fs[0]&&fs[1])
		return 0;
	else
		return 1;
}
//��Сд��ĸתΪ��д��ĸ,���������,�򱣳ֲ���.
uint8_t char_upper(uint8_t c)
{
	if(c<'A')return c;//����,���ֲ���.
	if(c>='a')return c-0x20;//��Ϊ��д.
	else return c;//��д,���ֲ���
}
//�����ļ�������
//fname:�ļ���
//����ֵ:0XFF,��ʾ�޷�ʶ����ļ����ͱ��.
//		 ����,����λ��ʾ��������,����λ��ʾ����С��.
uint8_t f_typetell(uint8_t *fname)
{
	uint8_t tbuf[5];
	uint8_t *attr='\0';//��׺��
	uint8_t i=0,j;
	while(i<250)
	{
		i++;
		if(*fname=='\0')
			break;//ƫ�Ƶ��������.
		fname++;
	}
	if(i==250)
		return 0XFF;//������ַ���.
 	for(i=0;i<5;i++)//�õ���׺��
	{
		fname--;
		if(*fname=='.')
		{
			fname++;
			attr=fname;
			break;
		}
  	}
	strcpy((char *)tbuf,(const char*)attr);//copy
 	for(i=0;i<4;i++)
		tbuf[i]=char_upper(tbuf[i]);//ȫ����Ϊ��д 
	for(i=0;i<4;i++)
	{
		for(j=0;j<13;j++)
		{
			if(*FILE_TYPE_TBL[i][j]==0)break;//�����Ѿ�û�пɶԱȵĳ�Ա��.
			if(strcmp((const char *)FILE_TYPE_TBL[i][j],(const char *)tbuf)==0)//�ҵ���
			{
				return (i<<4)|j;   //0x00 0x10 0x20 0x30 0x40 0x50 �����ļ�
			}
		}
	}
	return 0XFF;//û�ҵ�		 			   
}	 
//�õ�����ʣ������
//drv:���̱��("0:"/"1:")
//total:������	 ����λKB��
//free:ʣ������	 ����λKB��
//����ֵ:0,����.����,�������
uint8_t exf_getfree(uint8_t *drv,uint32_t *total,uint32_t *free)
{
	FATFS *fs1;
	uint8_t res;
    uint32_t fre_clust=0, fre_sect=0, tot_sect=0;
    //�õ�������Ϣ�����д�����
    res =(uint32_t)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect=(fs1->n_fatent-2)*fs1->csize;	//�õ���������
	    fre_sect=fre_clust*fs1->csize;			//�õ�����������	   
#if _MAX_SS!=512				  				//������С����512�ֽ�,��ת��Ϊ512�ֽ�
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		*total=tot_sect>>1;	//��λΪKB
		*free=fre_sect>>1;	//��λΪKB 
 	}
	return res;
}	
//Ѱ���ļ�·������
void FilePath_process(char * source,char *addstring)
{
	uint8_t count = 0;
	while(*source != '\0')
	{
		source++;
		count++;
	}
	if(*addstring == '.' && *(addstring+1) == '.')
	{
		if(*(source-1) == ':' && *(source-2) == '0' && count == 2) return;
		while(*source != '/') 
		{
			*source = '\0';
			source--;
		}
		*source = '\0';
		source--;
	} 
	else
	{
		*source = '/';
		source++;
		while(*addstring != '\0') 
		{
			*source = *addstring;
			source++;
			addstring++;
		}
  }
}
void ClearPath(char *path)
{
	path += 2;
	while(*path)
	{
		*path = '\0';
		path++;
	}
}

//void FatFs_Open(char *filename)
//{
//	OS_CPU_SR  cpu_sr;
//// 	UINT ss;
//	uint8_t res = 0;
//// 	FIL infile;
//	DIR root_file;
//	FILINFO file_name;
//// 	uint32_t file_size = 0;
//	#if _USE_LFN
//	char lfn[30];
////	file_name.lfname = (const TCHAR*)lfn;
//	file_name.lfsize = 30;
//	#endif
//	 OS_ENTER_CRITICAL();                  
//	
//	res = f_opendir(&root_file,(const TCHAR*)filename);//��Ŀ¼��������""
//	filecount = 0;
//	while((f_readdir(&root_file, &file_name) == FR_OK) && file_name.fname[0])
//	{ 
//		if(filecount < SDMaxFileCount)
//		{
//			strcpy((char *)FileList[filecount],file_name.fname);//file_name.fname
//			filecount ++;
//		} 
//		else 
//		{
//			break;
//		}
//// 			printf((char*)FileList[filecount]);
//// 			printf("\r\n");
//    }
//	OS_EXIT_CRITICAL();
// 		printf("filecount = %d\r\n",filecount);
// printf("\r\n���ӵ�·:\r\n\r\n");
// res = f_opendir(&root_file, "���ӵ�·");        //��Ŀ¼	
// while((f_readdir(&root_file, &file_name) == FR_OK) && file_name.fname[0]){  
// 	printf((char *)file_name.fname);
// 	printf("\r\n");
// }
		
// retry = f_open(&infile,"SD.txt",FA_CREATE_NEW);    //�������ļ�
// 	res = f_open(&infile, "test.txt", FA_OPEN_EXISTING | FA_READ);
// // f_write(&infile, "abcdefg",8, &ss);
// 	if(res != FR_OK) return;
// 	file_size = infile.fsize;
// 		printf("test.txt size: %ld\r\n",file_size);
// // 		TIME = 0;
// 	    res = f_read(&infile, SD_Buffer, BufferSize-1, &ss);
// // 		printf((char *)SD_Buffer);
// 			if (res != FR_OK) {
// 				printf("File read error!\r\n");
// // // // 				break;
// 			} 
// // 			if(ss<512)break;//�����ļ�ĩβ
//   f_close(&infile);    //�ر��ļ�
//}
// void FileOpen_Txt(char *buf)
// {
// 	uint8_t res = 0;
// 	FIL infile;
//   uint32_t file_size = 0;	
// 	UINT ss;
// 	FilePath_process(FilePath,buf);
// 	res = f_open(&infile, FilePath, FA_OPEN_EXISTING | FA_READ);
// 	if(res != FR_OK) return;
// 	file_size = infile.fsize;
// 	printf("**.txt size: %ld\r\n",file_size);
// 	ClearPath((char *)SD_Buffer);
// 	res = f_read(&infile, SD_Buffer, BufferSize-1, &ss);
// 	if (res != FR_OK) {
// 		 printf("File read error!\r\n");
// 	} 
//   f_close(&infile);    //�ر��ļ�
// 	FilePath_process(FilePath,"..");
// }
