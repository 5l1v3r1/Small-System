/*******************************************************************************
 * @file    : malloc.h
 * @author  : leon
 * @web     : www.ucortex.com
 * @version : V1.0
 * @date    : 2013-05-02
 * @brief   : 内存管理头文件
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
#ifndef __MALLOC_H
#define __MALLOC_H
#include "stm32f10x.h"  
#ifndef NULL
#define NULL 0
#endif

#define SRAMIN	0	//内部内存池
#define SRAMEX  1	//外部内存池


//mem1内存参数设定.mem1完全处于内部SRAM里面
#define MEM1_BLOCK_SIZE		32				//内存块大小为32字节
#define MEM1_MAX_SIZE			40*1024		//最大管理内存 40K
#define MEM1_ALLOC_TABLE_SIZE	MEM1_MAX_SIZE/MEM1_BLOCK_SIZE 	//内存表大小

//mem2内存参数设定.mem2的内存池处于外部SRAM里面,其他的处于内部SRAM里面
#define MEM2_BLOCK_SIZE		32				//内存块大小为32字节
#define MEM2_MAX_SIZE			200*1024	//最大管理内存200K
#define MEM2_ALLOC_TABLE_SIZE	MEM2_MAX_SIZE/MEM2_BLOCK_SIZE	//内存表大小
		 
		 
//内存管理控制器
struct _m_mallco_dev
{
	void (*init)(u8);		//初始化
	u8 (*perused)(u8);	//内存使用率
	u8 	*membase[2];		//内存池 管理2个区域的内存
	u16 *memmap[2];			//内存管理状态表
	u8  memrdy[2];			//内存管理是否就绪
};
extern struct _m_mallco_dev mallco_dev;	//在mallco.c里面定义

void mymemset(void *s,u8 c,u32 count);		//设置内存
void mymemcpy(void *des,void *src,u32 n);	//复制内存     
void mem_init(u8 memx);						//内存管理初始化函数(外/内部调用)
u32 mem_malloc(u8 memx,u32 size);	//内存分配(内部调用)
u8 mem_free(u8 memx,u32 offset);	//内存释放(内部调用)
u8 mem_perused(u8 memx);					//获得内存使用率(外/内部调用) 

//用户调用函数
void myfree(u8 memx,void *ptr);							//内存释放(外部调用)
void *mymalloc(u8 memx,u32 size);						//内存分配(外部调用)
void *myrealloc(u8 memx,void *ptr,u32 size);//重新分配内存(外部调用)
#endif
