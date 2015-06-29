/*******************************************************************************
 * @file    : diskio.c
 * @author  : leon
 * @web     : www.ucortex.com
 * @version : V1.0
 * @date    : 2013-05-02
 * @brief   : fatfs文件
 * ---------------------------------------------------------------------------- 
 *
 *                                重要的声明
 *
 * 本文件所涉及的内容是免费提供，请各位用于研究学习，禁止用于商业目的，学习结束
 * 后，请在24小时之内删除。文件内部分代码引用于网上开源的例程，部分代码经过改写。
 * 对于因为参考本文件内容，导致您的产品直接或间接受到破坏，或涉及到法律问题，作
 * 者不承担任何责任。对于使用过程中发现的问题，如果您能在www.ucortex.com网站反馈
 * 给我们，我们会非常感谢，并将对问题及时研究并作出改善。例程的版本更新，将不做
 * 特别通知，请您自行到www.ucortex.com下载最新版本，谢谢。
 * 对于以上声明，UCORTEX保留进一步解释的权利！
 * ----------------------------------------------------------------------------
 * 说明:
 *-----------------------------------------------------------------------------
 *                                 更改记录
 * ----------------------------------------------------------------------------
 * 更改时间：2013-05-02    更改人：Leon
 * 版本记录：V1.0
 * 更改内容：新建
 * ----------------------------------------------------------------------------
 *
 ******************************************************************************/
#include "sd_device//sdio_sdcard.h"
#include "malloc.h"	
#include "diskio.h"	 		   	 		   
 
#define SD_CARD	 0	//SD卡,卷标为0

/**
  * @brief: 初始化磁盘
	*/
DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{	
	u8 res=0;	    
	switch(drv)
	{
		case SD_CARD://SD卡
			res = SD_Init(); 	 
 			if(res==SD_OK)res=0;
			else res=1;		
 			break;
		default:
			res=1; 
	}		 
	if(res)return  STA_NOINIT;
	else return 0; //初始化成功
}   
/**
  * @brief: 获得磁盘状态
  */
DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{		   
    return 0;
} 
 /**
  * @brief: 读扇区
  * @param: drv:磁盘编号0~9
  * @param: *buff:数据接收缓冲首地址
  * @param: sector:扇区地址
  * @param: count:需要读取的扇区数
  */
DRESULT disk_read (
	BYTE drv,			/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
	u8 res=0; 
    if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误		 	 
	switch(drv)
	{
		case SD_CARD://SD卡
			res=SD_ReadDisk(buff,sector,count);
			break;
		default:
			res=1; 
	}
   //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if(res==0x00)return RES_OK;	 
    else return RES_ERROR;	   
}
/**
	* @brief: 写扇区
  * @param: drv:磁盘编号0~9
  * @param: *buff:发送数据首地址
  * @param: sector:扇区地址
  * @param: count:需要写入的扇区数	 
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
	u8 retry=0X1F;		//写入失败的时候,重试次数
    if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误		 	 
	switch(drv)
	{
		case SD_CARD://SD卡
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
    //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if(res == 0x00)return RES_OK;	 
    else return RES_ERROR;		 
}
#endif /* _READONLY */

/**
  * @brief: 其他表参数的获得
  * @param: drv:磁盘编号0~9
  * @param: ctrl:控制代码
  * @param: *buff:发送/接收缓冲区指针
  */
DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,	/* Control code */
	void *buff	/* Buffer to send/receive control data */
)
{	
	DRESULT res;						  			     
	if(drv==SD_CARD)//SD卡
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
	}else res=RES_ERROR;//其他的不支持
    return res;
}   
/**
  * @brief: 获得时间
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
  * @brief: 动态分配内存
  */
void *ff_memalloc (UINT size)			
{
	return (void*)mymalloc(SRAMIN,size);
}
/**
  * @brief: 释放内存
  */
void ff_memfree (void* mf)		 
{
	myfree(SRAMIN,mf);
}
