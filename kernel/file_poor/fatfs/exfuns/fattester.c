/*******************************************************************************
 * @file    : fattester.c
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
#include "fattester.h"	 
#include "sd_device//sdio_sdcard.h"
#include "stdio.h"
#include "exfuns.h"
#include "malloc.h"		  
#include "src//ff.h"
#include "string.h"
    
/**
  * @brief: 为磁盘注册工作区	 
  * @param: drv:盘符
  * @retval:执行结果
	*/
u8 mf_mount(u8 drv)
{		   
	return f_mount(drv,fs[drv]); 
}
/**
  * @brief: 打开路径下的文件
  * @param: path:路径+文件名
  * @param: mode:打开模式
  * @retval:执行结果
  */
u8 mf_open(u8*path,u8 mode)
{
	u8 res;	 
	res=f_open(file,(const TCHAR*)path,mode);//打开文件夹
	return res;
} 
/**
  * @brief: 关闭文件
  * @retval:执行结果
  */
u8 mf_close(void)
{
	f_close(file);
	return 0;
}
/**
  * @brief: 读出数据
  * @param: len:读出的长度
  * @retval:执行结果
  */
u8 mf_read(u16 len)
{
	u16 i,t;
	u8 res=0;
	u16 tlen=0;
	printf("\r\nRead file data is:\r\n");
	for(i=0;i<len/512;i++)
	{
		res=f_read(file,fatbuf,512,&br);
		if(res)
		{
			printf("Read Error:%d\r\n",res);
			break;
		}else
		{
			tlen+=br;
			for(t=0;t<br;t++)printf("%c",fatbuf[t]); 
		}
	}
	if(len%512)
	{
		res=f_read(file,fatbuf,len%512,&br);
		if(res)	//读数据出错了
		{
			printf("\r\nRead Error:%d\r\n",res);   
		}else
		{
			tlen+=br;
			for(t=0;t<br;t++)printf("%c",fatbuf[t]); 
		}	 
	}
	if(tlen)printf("\r\nReaded data len:%d\r\n",tlen);//读到的数据长度
	printf("Read data over\r\n");	 
	return res;
}
/**
  * @brief: 写入数据
  * @param: dat:数据缓存区
  * @param: len:写入长度
  * @retval:执行结果
  */
u8 mf_write(u8*dat,u16 len)
{			    
	u8 res;	   					   

	printf("\r\nBegin Write file...\r\n");
	printf("Write data len:%d\r\n",len);	 
	res=f_write(file,dat,len,&bw);
	if(res)
	{
		printf("Write Error:%d\r\n",res);   
	}else printf("Writed data len:%d\r\n",bw);
	printf("Write data over.\r\n");
	return res;
}

/**
  * @brief: 打开文件夹
  * @param: path:路径
  * @retval:执行结果
  */
u8 mf_opendir(u8* path)
{
	return f_opendir(&dir,(const TCHAR*)path);	
}
/**
  * @brief: 打读取文件夹
  * @retval:执行结果
  */
u8 mf_readdir(void)
{
	u8 res;
	char *fn;			 
#if _USE_LFN
 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
	fileinfo.lfname = mymalloc(SRAMEX,fileinfo.lfsize);
#endif		  
	res=f_readdir(&dir,&fileinfo);//读取一个文件的信息
	if(res!=FR_OK||fileinfo.fname[0]==0)
	{
		myfree(SRAMIN,fileinfo.lfname);
		return res;//读完了.
	}
#if _USE_LFN
	fn=*fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
#else
	fn=fileinfo.fname;;
#endif	
	printf("\r\n DIR info:\r\n");

	printf("dir.id:%d\r\n",dir.id);
	printf("dir.index:%d\r\n",dir.index);
	printf("dir.sclust:%d\r\n",dir.sclust);
	printf("dir.clust:%d\r\n",dir.clust);
	printf("dir.sect:%d\r\n",dir.sect);	  

	printf("\r\n");
	printf("File Name is:%s\r\n",fn);
	printf("File Size is:%d\r\n",fileinfo.fsize);
	printf("File data is:%d\r\n",fileinfo.fdate);
	printf("File time is:%d\r\n",fileinfo.ftime);
	printf("File Attr is:%d\r\n",fileinfo.fattrib);
	printf("\r\n");
	myfree(SRAMIN,fileinfo.lfname);
	return 0;
}			 

/** 
  * @brief: 遍历文件
  * @param: path:路径
  * @retval:执行结果
  */
u8 mf_scan_files(u8 * path)
{
	FRESULT res;	  
    char *fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
	fileinfo.lfname = mymalloc(SRAMEX,fileinfo.lfsize);
#endif		  

    res = f_opendir(&dir,(const TCHAR*)path); //打开一个目录
    if (res == FR_OK) 
	{	
		printf("\r\n"); 
		while(1)
		{
	        res = f_readdir(&dir, &fileinfo);                   //读取目录下的一个文件
	        if (res != FR_OK || fileinfo.fname[0] == 0) break;  //错误了/到末尾了,退出
	        //if (fileinfo.fname[0] == '.') continue;             //忽略上级目录
#if _USE_LFN
        	fn = *fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
#else							   
        	fn = fileinfo.fname;
#endif	                                              /* It is a file. */
			printf("%s/", path);//打印路径	
			printf("%s\r\n",  fn);//打印文件名	  
		} 
    }	  
	myfree(SRAMIN,fileinfo.lfname);
    return res;	  
}
/**
  * @brief: 显示剩余容量
  * @param: drv:盘符
  * @retval:剩余容量(字节)
  */
u32 mf_showfree(u8 *drv)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //得到磁盘信息及空闲簇数量
    res = f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect = (fs1->n_fatent - 2) * fs1->csize;//得到总扇区数
	    fre_sect = fre_clust * fs1->csize;			//得到空闲扇区数	   
#if _MAX_SS!=512
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		if(tot_sect<20480)//总容量小于10M
		{
		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
		    printf("\r\n磁盘总容量:%d KB\r\n"
		           "可用空间:%d KB\r\n",
		           tot_sect>>1,fre_sect>>1);
		}else
		{
		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
		    printf("\r\n磁盘总容量:%d MB\r\n"
		           "可用空间:%d MB\r\n",
		           tot_sect>>11,fre_sect>>11);
		}
	}
	return fre_sect;
}		    
/**
  * @brief: 文件读写指针偏移
  * @param: offset:相对首地址的偏移量
  * @retval:执行结果
  */
u8 mf_lseek(u32 offset)
{
	return f_lseek(file,offset);
}
/**
  * @brief: 读取文件当前读写指针的位置.
  * @retval:位置
  */
u32 mf_tell(void)
{
	return f_tell(file);
}

/**
  * @brief: 读取文件大小
  * @retval:文件大小
  */
u32 mf_size(void)
{
	return f_size(file);
} 
/**
  * @brief: 创建目录
  * @param: pname:目录路径+名字
  * @retval:执行结果
  */
u8 mf_mkdir(u8*pname)
{
	return f_mkdir((const TCHAR *)pname);
}
/**
  * @brief: 格式化
  * @param: drv:盘符
  * @param: mode:模式
  * @param: au:簇大小
  * @retval:执行结果
  */
u8 mf_fmkfs(u8 drv,u8 mode,u16 au)
{
	return f_mkfs(drv,mode,au);//格式化,drv:盘符;mode:模式;au:簇大小
} 

/**
  * @brief: 删除文件/目录
  * @param: pname:文件/目录路径+名字
  * @retval:执行结果
  */
u8 mf_unlink(u8 *pname)
{
	return  f_unlink((const TCHAR *)pname);
}

/**
  * @brief: 修改文件/目录名字(如果目录不同,还可以移动文件)
  * @param: oldname:之前的名字
  * @param: newname:新名字
  * @retval:执行结果
  */
u8 mf_rename(u8 *oldname,u8* newname)
{
	return  f_rename((const TCHAR *)oldname,(const TCHAR *)newname);
}

/**
  * @brief: 从文件里面读取一段字符串
  * @param: size:要读取的长度
  */
void mf_gets(u16 size)
{
 	TCHAR* rbuf;
	rbuf=f_gets((TCHAR*)fatbuf,size,file);
	if(*rbuf==0)return  ;//没有数据读到
	else
	{
		printf("\r\nThe String Readed Is:%s\r\n",rbuf);  	  
	}			    	
}

/**
  * @brief: 需要_USE_STRFUNC>=1
  * @param: 写一个字符到文件
  * @param: c:要写入的字符
  * @retval:执行结果
  */
u8 mf_putc(u8 c)
{
	return f_putc((TCHAR)c,file);
}

/**
  * @brief: 写字符串到文件
  * @param: c:要写入的字符串
  * @param: 
  * @param: 
  * @retval:写入的字符串长度
  */
u8 mf_puts(u8*c)
{
	return f_puts((TCHAR*)c,file);
}
