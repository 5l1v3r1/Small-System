/*******************************************************************************
 * @file    : 24cxx.c
 * @author  : leon
 * @web     : www.ucortex.com
 * @version : V1.0
 * @date    : 2013-05-02
 * @brief   : AT24Cxx底层驱动
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
#include "24cxx.h" 
#include "delay.h" 										 

/**
  * @brief: 在AT24CXX指定地址读出一个数据
  * @param: ReadAddr - 读数地址
  * @retval: 读到的数据
	* @note:
	*/
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);				//发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);	//发送高地址	    
	}
	else
		IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));	//发送器件地址0XA0,写数据 	   
	IIC_Wait_Ack(); 
	IIC_Send_Byte(ReadAddr%256);	//发送低地址
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);					//进入接收模式			   
	IIC_Wait_Ack();	 
	temp=IIC_Read_Byte(0);		   
	IIC_Stop();										//产生一个停止条件	    
	return temp;
}

/**
  * @brief: 在AT24CXX指定地址写入一个数据
  * @param: WriteAddr   - 写入数据的目的地址    
  * @param: DataToWrite - 要写入的数据
  * @retval: none
  * @note:
  */
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
	IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);				//发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//发送高地址	  
	}
	else
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));	//发送器件地址0XA0,写数据 	 
	IIC_Wait_Ack();	   
	IIC_Send_Byte(WriteAddr%256);	//发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);		//发送字节							   
	IIC_Wait_Ack();  		    	   
	IIC_Stop();										//产生一个停止条件 
	delay_ms(10);	 
}

/**
  * @brief: 在AT24CXX里面的指定地址开始写入长度为Len的数据,该函数用于
  *         写入16bit或者32bit的数据.
  * @param: WriteAddr  - 开始写入的地址  
  * @param: DataToWrite- 数据数组首地址
  * @param: Len        -要写入数据的长度2,4
  * @retval: none
  * @note:
  */
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

/**
  * @brief: 在AT24CXX里面的指定地址开始读出长度为Len的数据
  *         该函数用于读出16bit或者32bit的数据.
  * @param: ReadAddr - 开始读出的地址
  * @param: Len      - 要读出数据的长度2,4
  * @retval: 读出的数据
  */
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}

/**
  * @brief: 检查AT24CXX是否正常
  * @retval: 0 - 检测成功  !0 - 检测失败
  * @note:
  *   - 这里用了24XX的最后一个地址(255)来存储标志字.
  *     如果用其他24C系列,这个地址要修改
  */
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//避免每次开机都写AT24CXX			   
	if(temp==0X55)return 0;		   
	else//排除第一次初始化的情况
	{
		AT24CXX_WriteOneByte(255,0X55);
		temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

/**
  * @brief: 在AT24CXX里面的指定地址开始读出指定个数的数据
  * @param: ReadAddr - 开始读出的地址 对24c02为0~255
  * @param: pBuffer  - 数据数组首地址
  * @param: NumToRead- 要读出数据的个数
  * @retval: none
  * @note:
  */
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
/**
  * @brief: 在AT24CXX里面的指定地址开始写入指定个数的数据
  * @param: WriteAddr - 开始写入的地址 对24c02为0~255
  * @param: pBuffer   - 数据数组首地址
  * @param:NumToWrite - 要写入数据的个数
  * @retval:
  * @note:
  */
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}








