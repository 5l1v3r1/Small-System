/*******************************************************************************
 * @file    : malloc.c
 * @author  : leon
 * @web     : www.ucortex.com
 * @version : V1.0
 * @date    : 2013-05-02
 * @brief   : �ڴ�����ļ�
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
 * �ڴ�����ļ�������Ҫʹ��myfree��mymalloc��myreallocʱ����Ҫ������ļ�
 *-----------------------------------------------------------------------------
 *                                 ���ļ�¼
 * ----------------------------------------------------------------------------
 * ����ʱ�䣺2013-05-02    �����ˣ�Leon
 * �汾��¼��V1.0
 * �������ݣ��½�
 * ----------------------------------------------------------------------------
 *
 ******************************************************************************/
#include "malloc.h"

//�ڴ��(4�ֽڶ���)
__align(4) u8 mem1base[MEM1_MAX_SIZE];																	//�ڲ�SRAM�ڴ��
__align(4) u8 mem2base[MEM2_MAX_SIZE] __attribute__((at(0X68000000)));	//�ⲿSRAM�ڴ��
//�ڴ�����
u16 mem1mapbase[MEM1_ALLOC_TABLE_SIZE];																								//�ڲ�SRAM�ڴ��MAP
u16 mem2mapbase[MEM2_ALLOC_TABLE_SIZE] __attribute__((at(0X68000000+MEM2_MAX_SIZE)));	//�ⲿSRAM�ڴ��MAP
//�ڴ�������	   
const u32 memtblsize[2]={MEM1_ALLOC_TABLE_SIZE,MEM2_ALLOC_TABLE_SIZE};	//�ڴ���С
const u32 memblksize[2]={MEM1_BLOCK_SIZE,MEM2_BLOCK_SIZE};							//�ڴ�ֿ��С
const u32 memsize[2]={MEM1_MAX_SIZE,MEM2_MAX_SIZE};											//�ڴ��ܴ�С


//�ڴ���������
struct _m_mallco_dev mallco_dev=
{
	mem_init,								//�ڴ��ʼ��			(init)
	mem_perused,						//�ڴ�ʹ����			(perused)
	mem1base,mem2base,			//�ڴ��					(membase[2])
	mem1mapbase,mem2mapbase,//�ڴ����״̬��	(memmap[2])
	0,0,										//�ڴ����δ����	(memrdy[2])
};

/**
  * @brief: �����ڴ�
  * @param: *des:Ŀ�ĵ�ַ
  * @param: *src:Դ��ַ
  * @param: n:��Ҫ���Ƶ��ڴ泤��(�ֽ�Ϊ��λ)
  */
void mymemcpy(void *des,void *src,u32 n)  
{  
    u8 *xdes=des;
	u8 *xsrc=src; 
    while(n--)*xdes++=*xsrc++;  
}  
/**
  * @brief: �����ڴ�
  * @param: *s:�ڴ��׵�ַ
  * @param: c :Ҫ���õ�ֵ
  * @param: count:��Ҫ���õ��ڴ��С(�ֽ�Ϊ��λ)
  */
void mymemset(void *s,u8 c,u32 count)  
{  
    u8 *xs = s;  
    while(count--)*xs++=c;  
}	   
/**
  * @brief: �ڴ�����ʼ��  
  * @param: memx:�����ڴ��
  */
void mem_init(u8 memx)  
{  
	mymemset(mallco_dev.memmap[memx], 0,memtblsize[memx]*2);//�ڴ�״̬����������  
	mymemset(mallco_dev.membase[memx], 0,memsize[memx]);		//�ڴ��������������  
	mallco_dev.memrdy[memx]=1;								//�ڴ�����ʼ��OK  
}  
/**
  * @brief: ��ȡ�ڴ�ʹ����
  * @param: memx:�����ڴ��
  * @retval:ʹ����(0~100)
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
  * @brief: �ڴ����(�ڲ�����)
  * @param: memx:�����ڴ��
  * @param: size:Ҫ������ڴ��С(�ֽ�)
  * @retval:0XFFFFFFFF,����;    ����,�ڴ�ƫ�Ƶ�ַ 
  */
u32 mem_malloc(u8 memx,u32 size)  
{  
	signed long offset=0;  
	u16 nmemb;	//��Ҫ���ڴ����  
	u16 cmemb=0;//�������ڴ����
	u32 i; 
	
	if(!mallco_dev.memrdy[memx])mallco_dev.init(memx);	//δ��ʼ��,��ִ�г�ʼ�� 
	if(size==0)return 0XFFFFFFFF;	//����Ҫ����
	
	nmemb=size/memblksize[memx];	//��ȡ��Ҫ����������ڴ����
	if(size%memblksize[memx])nmemb++;  
	
	for(offset=memtblsize[memx]-1;offset>=0;offset--)	//���������ڴ������  
	{     
		if(!mallco_dev.memmap[memx][offset])cmemb++;			//�������ڴ��������
		else cmemb=0;			//�����ڴ������
		if(cmemb==nmemb)	//�ҵ�������nmemb�����ڴ��
		{
			for(i=0;i<nmemb;i++)	//��ע�ڴ��ǿ� 
			{  
				mallco_dev.memmap[memx][offset+i]=nmemb;  
			}  
			return (offset*memblksize[memx]);//����ƫ�Ƶ�ַ
		}
	}  
	return 0XFFFFFFFF;//δ�ҵ����Ϸ����������ڴ��  
}  
/**
  * @brief: �ͷ��ڴ�(�ڲ�����) 
  * @param: memx:�����ڴ��
  * @param: offset:�ڴ��ַƫ��
  * @retval:0,�ͷųɹ�;    1,�ͷ�ʧ��;
  */
u8 mem_free(u8 memx,u32 offset)  
{  
	int i;  
	if(!mallco_dev.memrdy[memx])	//δ��ʼ��,��ִ�г�ʼ��
	{
		mallco_dev.init(memx);    
		return 1;	//δ��ʼ��  
	}  
	if(offset<memsize[memx])	//ƫ�����ڴ����. 
	{  
		int index=offset/memblksize[memx];				//ƫ�������ڴ�����  
		int nmemb=mallco_dev.memmap[memx][index];	//�ڴ������
		for(i=0;i<nmemb;i++)	//�ڴ������
		{  
				mallco_dev.memmap[memx][index+i]=0;  
		}  
		return 0;  
	}
	else return 2;//ƫ�Ƴ�����.  
}  
/**
  * @brief: �ͷ��ڴ�(�ⲿ����) 
  * @param: memx:�����ڴ��
  * @param: ptr:�ڴ��׵�ַ
  */
void myfree(u8 memx,void *ptr)  
{  
	u32 offset;  
    if(ptr==NULL)return;//��ַΪ0.  
 	offset=(u32)ptr-(u32)mallco_dev.membase[memx];  
    mem_free(memx,offset);//�ͷ��ڴ�     
}  
/**
  * @brief: �����ڴ�(�ⲿ����)
  * @param: memx:�����ڴ��
  * @param: size:�ڴ��С(�ֽ�)
  * @retval:���䵽���ڴ��׵�ַ
  */
void *mymalloc(u8 memx,u32 size)  
{  
	u32 offset;  									      
	offset=mem_malloc(memx,size);  	   				   
	if(offset==0XFFFFFFFF)return NULL;  
	else return (void*)((u32)mallco_dev.membase[memx]+offset);  
}  
/**
  * @brief: ���·����ڴ�(�ⲿ����)
  * @param: memx:�����ڴ��
  * @param: *ptr:���ڴ��׵�ַ
  * @param: size:Ҫ������ڴ��С(�ֽ�)
  * @retval:�·��䵽���ڴ��׵�ַ.
  */
void *myrealloc(u8 memx,void *ptr,u32 size)  
{  
	u32 offset;  
	offset=mem_malloc(memx,size);  
	if(offset==0XFFFFFFFF)return NULL;     
	else  
	{  									   
		mymemcpy((void*)((u32)mallco_dev.membase[memx]+offset),ptr,size);	//�������ڴ����ݵ����ڴ�   
		myfree(memx,ptr);	//�ͷž��ڴ�
		return (void*)((u32)mallco_dev.membase[memx]+offset);	//�������ڴ��׵�ַ
	}  
}












