/*******************************************************************************
 * @file    : diskio.c
 * @author  : leon
 * @web     : www.ucortex.com
 * @version : V1.0
 * @date    : 2013-05-02
 * @brief   : fatfs�ļ�
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
#include "sd_device//sdio_sdcard.h"
#include "malloc.h"	
#include "diskio.h"	 		   	 		   
 
#define SD_CARD	 0	//SD��,���Ϊ0

/**
  * @brief: ��ʼ������
	*/
DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{	
	u8 res=0;	    
	switch(drv)
	{
		case SD_CARD://SD��
			res = SD_Init(); 	 
 			if(res==SD_OK)res=0;
			else res=1;		
 			break;
		default:
			res=1; 
	}		 
	if(res)return  STA_NOINIT;
	else return 0; //��ʼ���ɹ�
}   
/**
  * @brief: ��ô���״̬
  */
DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{		   
    return 0;
} 
 /**
  * @brief: ������
  * @param: drv:���̱��0~9
  * @param: *buff:���ݽ��ջ����׵�ַ
  * @param: sector:������ַ
  * @param: count:��Ҫ��ȡ��������
  */
DRESULT disk_read (
	BYTE drv,			/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
	u8 res=0; 
    if (!count)return RES_PARERR;//count���ܵ���0�����򷵻ز�������		 	 
	switch(drv)
	{
		case SD_CARD://SD��
			res=SD_ReadDisk(buff,sector,count);
			break;
		default:
			res=1; 
	}
   //������ֵ����SPI_SD_driver.c�ķ���ֵת��ff.c�ķ���ֵ
    if(res==0x00)return RES_OK;	 
    else return RES_ERROR;	   
}
/**
	* @brief: д����
  * @param: drv:���̱��0~9
  * @param: *buff:���������׵�ַ
  * @param: sector:������ַ
  * @param: count:��Ҫд���������	 
  */
#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,					/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,			/* Sector address (LBA) */
	BYTE count				/* Number of sectors to write (1..255) */
)
{
	u8 res=0;  
	u8 retry=0X1F;		//д��ʧ�ܵ�ʱ��,���Դ���
    if (!count)return RES_PARERR;//count���ܵ���0�����򷵻ز�������		 	 
	switch(drv)
	{
		case SD_CARD://SD��
			while(retry)
			{
				res=SD_WriteDisk((u8*)buff,sector,count);
 				if(res==0)break;
				retry--;
			}
			break;
		default:
			res=1; 
	}
    //������ֵ����SPI_SD_driver.c�ķ���ֵת��ff.c�ķ���ֵ
    if(res == 0x00)return RES_OK;	 
    else return RES_ERROR;		 
}
#endif /* _READONLY */

/**
  * @brief: ����������Ļ��
  * @param: drv:���̱��0~9
  * @param: ctrl:���ƴ���
  * @param: *buff:����/���ջ�����ָ��
  */
DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,	/* Control code */
	void *buff	/* Buffer to send/receive control data */
)
{	
	DRESULT res;						  			     
	if(drv==SD_CARD)//SD��
	{
	    switch(ctrl)
	    {
		    case CTRL_SYNC:	    
 		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff = 512;
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff = 8;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
 		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
	}else res=RES_ERROR;//�����Ĳ�֧��
    return res;
}   
/**
  * @brief: ���ʱ��
  * @note:
  *   - defined function to give a current time to fatfs module
  *   - 31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31)                                                                                                                                                                                                                                        
  *  - 15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2)
  */
DWORD get_fattime (void)
{				 
	return 0;
}			 
/**
  * @brief: ��̬�����ڴ�
  */
void *ff_memalloc (UINT size)			
{
	return (void*)mymalloc(SRAMIN,size);
}
/**
  * @brief: �ͷ��ڴ�
  */
void ff_memfree (void* mf)		 
{
	myfree(SRAMIN,mf);
}
