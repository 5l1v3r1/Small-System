/*******************************************************************************
 * @file    : exfuns.c
 * @author  : leon
 * @web     : www.ucortex.com
 * @version : V1.0
 * @date    : 2013-05-02
 * @brief   : fatfs��չ�ļ�
 * ---------------------------------------------------------------------------- 
 *
 *                                ��Ҫ������
 *
 * ���ļ����漰������������ṩ�����λ�����о�ѧϰ����ֹ������ҵĿ�ģ�ѧϰ����
 * ������24Сʱ֮��ɾ�����ļ��ڲ��ִ������������Ͽ�Դ�����̣����ִ��뾭����д��
 * ������Ϊ�ο����ļ����ݣ��������Ĳ�Ʒֱ�ӻ����ܵ��ƻ������漰���������⣬��
 * �߲��е��κ����Ρ�����ʹ�ù����з��ֵ����⣬���������www.ucortex.com��վ����
 * �����ǣ����ǻ�ǳ���л�����������⼰ʱ�о����������ơ����̵İ汾���£�������
 * �ر�֪ͨ���������е�www.ucortex.com�������°汾��лл��
 * ��������������UCORTEX������һ�����͵�Ȩ����
 * ----------------------------------------------------------------------------
 * ˵��:
 *-----------------------------------------------------------------------------
 *                                 ���ļ�¼
 * ----------------------------------------------------------------------------
 * ����ʱ�䣺2013-05-02    �����ˣ�Leon
 * �汾��¼��V1.0
 * �������ݣ��½�
 * ----------------------------------------------------------------------------
 *
 ******************************************************************************/
#include "string.h"
#include "exfuns.h"
#include "fattester.h"	
#include "malloc.h"

//�ļ������б�
const u8 *FILE_TYPE_TBL[6][13]=
{
{"BIN"},			//BIN�ļ�
{"LRC"},			//LRC�ļ�
{"NES"},			//NES�ļ�
{"TXT","C","H"},	//�ı��ļ�
{"MP1","MP2","MP3","MP4","M4A","3GP","3G2","OGG","ACC","WMA","WAV","MID","FLAC"},//�����ļ�
{"BMP","JPG","JPEG","GIF"},//ͼƬ�ļ�
};
/*------------------------------------�����ļ���,ʹ��malloc��ʱ��------------------------------------*/
FATFS *fs[2];			//�߼����̹�����.	 
FIL *file;				//�ļ�1
FIL *ftemp;				//�ļ�2.
UINT br,bw;				//��д����
FILINFO fileinfo;	//�ļ���Ϣ
DIR dir;					//Ŀ¼

u8 *fatbuf;				//SD�����ݻ�����
/*--------------------------------------------------------------------------------------------------*/
/**
  * @brief: Ϊexfuns�����ڴ�
  * @retval:0,�ɹ�    1,ʧ��
  */
u8 exfuns_init(void)
{
	fs[0]=(FATFS*)mymalloc(SRAMEX,sizeof(FATFS));	//Ϊ����0�����������ڴ�	
	file=(FIL*)mymalloc(SRAMEX,sizeof(FIL));			//Ϊfile�����ڴ�
	ftemp=(FIL*)mymalloc(SRAMEX,sizeof(FIL));			//Ϊftemp�����ڴ�
	fatbuf=(u8*)mymalloc(SRAMEX,512);							//Ϊfatbuf�����ڴ�
	if(fs[0]&&file&&ftemp&&fatbuf)return 0;//������һ��ʧ��,��ʧ��.
	else return 1;	
}

/**
  * @brief: ��Сд��ĸתΪ��д��ĸ,���������,�򱣳ֲ���
  */
u8 char_upper(u8 c)
{
	if(c<'A')return c;//����,���ֲ���.
	if(c>='a')return c-0x20;//��Ϊ��д.
	else return c;//��д,���ֲ���
}	      
/**
  * @brief: �����ļ�������
  * @param: fname:�ļ���
  * @retval:0XFF - ��ʾ�޷�ʶ����ļ����ͱ��.
  *         ���� - ����λ��ʾ��������,����λ��ʾ����С��.
  */
u8 f_typetell(u8 *fname)
{
	u8 tbuf[5];
	u8 *attr='\0';//��׺��
	u8 i=0,j;
	while(i<250)
	{
		i++;
		if(*fname=='\0')break;//ƫ�Ƶ��������.
		fname++;
	}
	if(i==250)return 0XFF;	//������ַ���.
 	for(i=0;i<5;i++)				//�õ���׺��
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
 	for(i=0;i<4;i++)tbuf[i]=char_upper(tbuf[i]);//ȫ����Ϊ��д 
	for(i=0;i<6;i++)
	{
		for(j=0;j<13;j++)
		{
			if(*FILE_TYPE_TBL[i][j]==0)break;//�����Ѿ�û�пɶԱȵĳ�Ա��.
			if(strcmp((const char *)FILE_TYPE_TBL[i][j],(const char *)tbuf)==0)//�ҵ���
			{
				return (i<<4)|j;
			}
		}
	}
	return 0XFF;//û�ҵ�		 			   
}	 

/**
  * @brief: �õ�����ʣ������
  * @param: drv:���̱��("0:")
  * @param: total:������	 ����λKB��
  * @param: free:ʣ������	 ����λKB��
  * @retval:0 - OK    !0 - ������
  */
u8 exf_getfree(u8 *drv,u32 *total,u32 *free)
{
	FATFS *fs1;
	u8 res;
	u32 fre_clust=0, fre_sect=0, tot_sect=0;
	//�õ�������Ϣ�����д�����
	res = f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
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
