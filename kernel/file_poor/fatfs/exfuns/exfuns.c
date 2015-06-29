/*******************************************************************************
 * @file    : exfuns.c
 * @author  : leon
 * @web     : www.ucortex.com
 * @version : V1.0
 * @date    : 2013-05-02
 * @brief   : fatfs扩展文件
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
#include "string.h"
#include "exfuns.h"
#include "fattester.h"	
#include "malloc.h"

//文件类型列表
const u8 *FILE_TYPE_TBL[6][13]=
{
{"BIN"},			//BIN文件
{"LRC"},			//LRC文件
{"NES"},			//NES文件
{"TXT","C","H"},	//文本文件
{"MP1","MP2","MP3","MP4","M4A","3GP","3G2","OGG","ACC","WMA","WAV","MID","FLAC"},//音乐文件
{"BMP","JPG","JPEG","GIF"},//图片文件
};
/*------------------------------------公共文件区,使用malloc的时候------------------------------------*/
FATFS *fs[2];			//逻辑磁盘工作区.	 
FIL *file;				//文件1
FIL *ftemp;				//文件2.
UINT br,bw;				//读写变量
FILINFO fileinfo;	//文件信息
DIR dir;					//目录

u8 *fatbuf;				//SD卡数据缓存区
/*--------------------------------------------------------------------------------------------------*/
/**
  * @brief: 为exfuns申请内存
  * @retval:0,成功    1,失败
  */
u8 exfuns_init(void)
{
	fs[0]=(FATFS*)mymalloc(SRAMEX,sizeof(FATFS));	//为磁盘0工作区申请内存	
	file=(FIL*)mymalloc(SRAMEX,sizeof(FIL));			//为file申请内存
	ftemp=(FIL*)mymalloc(SRAMEX,sizeof(FIL));			//为ftemp申请内存
	fatbuf=(u8*)mymalloc(SRAMEX,512);							//为fatbuf申请内存
	if(fs[0]&&file&&ftemp&&fatbuf)return 0;//申请有一个失败,即失败.
	else return 1;	
}

/**
  * @brief: 将小写字母转为大写字母,如果是数字,则保持不变
  */
u8 char_upper(u8 c)
{
	if(c<'A')return c;//数字,保持不变.
	if(c>='a')return c-0x20;//变为大写.
	else return c;//大写,保持不变
}	      
/**
  * @brief: 报告文件的类型
  * @param: fname:文件名
  * @retval:0XFF - 表示无法识别的文件类型编号.
  *         其他 - 高四位表示所属大类,低四位表示所属小类.
  */
u8 f_typetell(u8 *fname)
{
	u8 tbuf[5];
	u8 *attr='\0';//后缀名
	u8 i=0,j;
	while(i<250)
	{
		i++;
		if(*fname=='\0')break;//偏移到了最后了.
		fname++;
	}
	if(i==250)return 0XFF;	//错误的字符串.
 	for(i=0;i<5;i++)				//得到后缀名
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
 	for(i=0;i<4;i++)tbuf[i]=char_upper(tbuf[i]);//全部变为大写 
	for(i=0;i<6;i++)
	{
		for(j=0;j<13;j++)
		{
			if(*FILE_TYPE_TBL[i][j]==0)break;//此组已经没有可对比的成员了.
			if(strcmp((const char *)FILE_TYPE_TBL[i][j],(const char *)tbuf)==0)//找到了
			{
				return (i<<4)|j;
			}
		}
	}
	return 0XFF;//没找到		 			   
}	 

/**
  * @brief: 得到磁盘剩余容量
  * @param: drv:磁盘编号("0:")
  * @param: total:总容量	 （单位KB）
  * @param: free:剩余容量	 （单位KB）
  * @retval:0 - OK    !0 - 错误码
  */
u8 exf_getfree(u8 *drv,u32 *total,u32 *free)
{
	FATFS *fs1;
	u8 res;
	u32 fre_clust=0, fre_sect=0, tot_sect=0;
	//得到磁盘信息及空闲簇数量
	res = f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
	if(res==0)
	{											   
		tot_sect=(fs1->n_fatent-2)*fs1->csize;	//得到总扇区数
		fre_sect=fre_clust*fs1->csize;			//得到空闲扇区数	   
#if _MAX_SS!=512				  				//扇区大小不是512字节,则转换为512字节
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		*total=tot_sect>>1;	//单位为KB
		*free=fre_sect>>1;	//单位为KB 
	}
	return res;
}
