/*******************************************************************************
 * @file    : malloc.c
 * @author  : leon
 * @web     : www.ucortex.com
 * @version : V1.0
 * @date    : 2013-05-02
 * @brief   : 内存管理文件
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
 * 内存管理文件，当需要使用myfree，mymalloc，myrealloc时，需要加入此文件
 *-----------------------------------------------------------------------------
 *                                 更改记录
 * ----------------------------------------------------------------------------
 * 更改时间：2013-05-02    更改人：Leon
 * 版本记录：V1.0
 * 更改内容：新建
 * ----------------------------------------------------------------------------
 *
 ******************************************************************************/
#include "malloc.h"

//内存池(4字节对齐)
__align(4) u8 mem1base[MEM1_MAX_SIZE];																	//内部SRAM内存池
__align(4) u8 mem2base[MEM2_MAX_SIZE] __attribute__((at(0X68000000)));	//外部SRAM内存池
//内存管理表
u16 mem1mapbase[MEM1_ALLOC_TABLE_SIZE];																								//内部SRAM内存池MAP
u16 mem2mapbase[MEM2_ALLOC_TABLE_SIZE] __attribute__((at(0X68000000+MEM2_MAX_SIZE)));	//外部SRAM内存池MAP
//内存管理参数	   
const u32 memtblsize[2]={MEM1_ALLOC_TABLE_SIZE,MEM2_ALLOC_TABLE_SIZE};	//内存表大小
const u32 memblksize[2]={MEM1_BLOCK_SIZE,MEM2_BLOCK_SIZE};							//内存分块大小
const u32 memsize[2]={MEM1_MAX_SIZE,MEM2_MAX_SIZE};											//内存总大小


//内存管理控制器
struct _m_mallco_dev mallco_dev=
{
	mem_init,								//内存初始化			(init)
	mem_perused,						//内存使用率			(perused)
	mem1base,mem2base,			//内存池					(membase[2])
	mem1mapbase,mem2mapbase,//内存管理状态表	(memmap[2])
	0,0,										//内存管理未就绪	(memrdy[2])
};

/**
  * @brief: 复制内存
  * @param: *des:目的地址
  * @param: *src:源地址
  * @param: n:需要复制的内存长度(字节为单位)
  */
void mymemcpy(void *des,void *src,u32 n)  
{  
    u8 *xdes=des;
	u8 *xsrc=src; 
    while(n--)*xdes++=*xsrc++;  
}  
/**
  * @brief: 设置内存
  * @param: *s:内存首地址
  * @param: c :要设置的值
  * @param: count:需要设置的内存大小(字节为单位)
  */
void mymemset(void *s,u8 c,u32 count)  
{  
    u8 *xs = s;  
    while(count--)*xs++=c;  
}	   
/**
  * @brief: 内存管理初始化  
  * @param: memx:所属内存块
  */
void mem_init(u8 memx)  
{  
	mymemset(mallco_dev.memmap[memx], 0,memtblsize[memx]*2);//内存状态表数据清零  
	mymemset(mallco_dev.membase[memx], 0,memsize[memx]);		//内存池所有数据清零  
	mallco_dev.memrdy[memx]=1;								//内存管理初始化OK  
}  
/**
  * @brief: 获取内存使用率
  * @param: memx:所属内存块
  * @retval:使用率(0~100)
  */
u8 mem_perused(u8 memx)  
{  
	u32 used=0;  
	u32 i;  
	for(i=0;i<memtblsize[memx];i++)  
	{  
		if(mallco_dev.memmap[memx][i])used++; 
	} 
	return (used*100)/(memtblsize[memx]);  
}  
/**
  * @brief: 内存分配(内部调用)
  * @param: memx:所属内存块
  * @param: size:要分配的内存大小(字节)
  * @retval:0XFFFFFFFF,错误;    其他,内存偏移地址 
  */
u32 mem_malloc(u8 memx,u32 size)  
{  
	signed long offset=0;  
	u16 nmemb;	//需要的内存块数  
	u16 cmemb=0;//连续空内存块数
	u32 i; 
	
	if(!mallco_dev.memrdy[memx])mallco_dev.init(memx);	//未初始化,先执行初始化 
	if(size==0)return 0XFFFFFFFF;	//不需要分配
	
	nmemb=size/memblksize[memx];	//获取需要分配的连续内存块数
	if(size%memblksize[memx])nmemb++;  
	
	for(offset=memtblsize[memx]-1;offset>=0;offset--)	//搜索整个内存控制区  
	{     
		if(!mallco_dev.memmap[memx][offset])cmemb++;			//连续空内存块数增加
		else cmemb=0;			//连续内存块清零
		if(cmemb==nmemb)	//找到了连续nmemb个空内存块
		{
			for(i=0;i<nmemb;i++)	//标注内存块非空 
			{  
				mallco_dev.memmap[memx][offset+i]=nmemb;  
			}  
			return (offset*memblksize[memx]);//返回偏移地址
		}
	}  
	return 0XFFFFFFFF;//未找到符合分配条件的内存块  
}  
/**
  * @brief: 释放内存(内部调用) 
  * @param: memx:所属内存块
  * @param: offset:内存地址偏移
  * @retval:0,释放成功;    1,释放失败;
  */
u8 mem_free(u8 memx,u32 offset)  
{  
	int i;  
	if(!mallco_dev.memrdy[memx])	//未初始化,先执行初始化
	{
		mallco_dev.init(memx);    
		return 1;	//未初始化  
	}  
	if(offset<memsize[memx])	//偏移在内存池内. 
	{  
		int index=offset/memblksize[memx];				//偏移所在内存块号码  
		int nmemb=mallco_dev.memmap[memx][index];	//内存块数量
		for(i=0;i<nmemb;i++)	//内存块清零
		{  
				mallco_dev.memmap[memx][index+i]=0;  
		}  
		return 0;  
	}
	else return 2;//偏移超区了.  
}  
/**
  * @brief: 释放内存(外部调用) 
  * @param: memx:所属内存块
  * @param: ptr:内存首地址
  */
void myfree(u8 memx,void *ptr)  
{  
	u32 offset;  
    if(ptr==NULL)return;//地址为0.  
 	offset=(u32)ptr-(u32)mallco_dev.membase[memx];  
    mem_free(memx,offset);//释放内存     
}  
/**
  * @brief: 分配内存(外部调用)
  * @param: memx:所属内存块
  * @param: size:内存大小(字节)
  * @retval:分配到的内存首地址
  */
void *mymalloc(u8 memx,u32 size)  
{  
	u32 offset;  									      
	offset=mem_malloc(memx,size);  	   				   
	if(offset==0XFFFFFFFF)return NULL;  
	else return (void*)((u32)mallco_dev.membase[memx]+offset);  
}  
/**
  * @brief: 重新分配内存(外部调用)
  * @param: memx:所属内存块
  * @param: *ptr:旧内存首地址
  * @param: size:要分配的内存大小(字节)
  * @retval:新分配到的内存首地址.
  */
void *myrealloc(u8 memx,void *ptr,u32 size)  
{  
	u32 offset;  
	offset=mem_malloc(memx,size);  
	if(offset==0XFFFFFFFF)return NULL;     
	else  
	{  									   
		mymemcpy((void*)((u32)mallco_dev.membase[memx]+offset),ptr,size);	//拷贝旧内存内容到新内存   
		myfree(memx,ptr);	//释放旧内存
		return (void*)((u32)mallco_dev.membase[memx]+offset);	//返回新内存首地址
	}  
}












