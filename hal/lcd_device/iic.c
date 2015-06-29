/*******************************************************************************
 * @file    : myiic.c
 * @author  : leon
 * @web     : www.ucortex.com
 * @version : V1.0
 * @date    : 2013-05-02
 * @brief   : IIC协议底层驱动
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
 * SCL - PB10
 * SDA - PB11
 *-----------------------------------------------------------------------------
 *                                 更改记录
 * ----------------------------------------------------------------------------
 * 更改时间：2013-05-02    更改人：Leon
 * 版本记录：V1.0
 * 更改内容：新建
 * ----------------------------------------------------------------------------
 *
 ******************************************************************************/
#include "iic.h"
#include "delay.h"
 
/**
  * @brief: 初始化IIC
	*/
void IIC_Init(void)
{					     
 	RCC->APB2ENR|=1<<3;			//先使能外设IO PORTB时钟 							 
	GPIOB->CRH&=0XFFFF00FF;	//PB10,PB11 推挽输出
	GPIOB->CRH|=0X00003300;	   
	GPIOB->ODR|=3<<10;			//PB10,PB11 输出高电平
}
/**
  * @brief: IIC起始信号
  */
void IIC_Start(void)
{
	SDA_OUT();		//SDA线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;		//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;		//钳住I2C总线，准备发送或接收数据 
}	  
/**
  * @brief: IIC停止信号
  */
void IIC_Stop(void)
{
	SDA_OUT();	//sda线输出
	IIC_SCL=0;
	IIC_SDA=0;	//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;	//发送I2C总线结束信号
	delay_us(4);							   	
}
/**
  * @brief: 等待应答信号到来
  * @retval: 1 - 超时，未接收到应答信号
  *          0 - 接收到应答信号
  */
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();	//SDA设置为输入  
	IIC_SDA=1;delay_us(1);	   
	IIC_SCL=1;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//时钟输出0 	   
	return 0;  
}

/**
  * @brief: 产生ACK应答
	*/
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}

/**
  * @brief: 不产生ACK应答
  */
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}					 				     
/**
  * @brief: IIC发送一个字节
  * @param: txd - 发送的数据
  * @retval: none
  * @note:
  */
void IIC_Send_Byte(u8 txd)
{                        
	u8 t;   
	SDA_OUT(); 	    
	IIC_SCL=0;//拉低时钟开始数据传输
	for(t=0;t<8;t++)
	{              
		IIC_SDA=(txd&0x80)>>7;
		txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
	}	 
}
/**
  * @brief: 读1个字节
  * @param: ack - 是否需要发送ACK
  * @arg:   1 - 发送ACK
  * @arg:   0 - 发送nACK
  * @retval: 读取到的数据
  * @note:
  */
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}

