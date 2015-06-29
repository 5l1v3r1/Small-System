/*******************************************************************************
 * @file    : myiic.c
 * @author  : leon
 * @web     : www.ucortex.com
 * @version : V1.0
 * @date    : 2013-05-02
 * @brief   : IICЭ��ײ�����
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
 * SCL - PB10
 * SDA - PB11
 *-----------------------------------------------------------------------------
 *                                 ���ļ�¼
 * ----------------------------------------------------------------------------
 * ����ʱ�䣺2013-05-02    �����ˣ�Leon
 * �汾��¼��V1.0
 * �������ݣ��½�
 * ----------------------------------------------------------------------------
 *
 ******************************************************************************/
#include "iic.h"
#include "delay.h"
 
/**
  * @brief: ��ʼ��IIC
	*/
void IIC_Init(void)
{					     
 	RCC->APB2ENR|=1<<3;			//��ʹ������IO PORTBʱ�� 							 
	GPIOB->CRH&=0XFFFF00FF;	//PB10,PB11 �������
	GPIOB->CRH|=0X00003300;	   
	GPIOB->ODR|=3<<10;			//PB10,PB11 ����ߵ�ƽ
}
/**
  * @brief: IIC��ʼ�ź�
  */
void IIC_Start(void)
{
	SDA_OUT();		//SDA�����
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;		//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;		//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
/**
  * @brief: IICֹͣ�ź�
  */
void IIC_Stop(void)
{
	SDA_OUT();	//sda�����
	IIC_SCL=0;
	IIC_SDA=0;	//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;	//����I2C���߽����ź�
	delay_us(4);							   	
}
/**
  * @brief: �ȴ�Ӧ���źŵ���
  * @retval: 1 - ��ʱ��δ���յ�Ӧ���ź�
  *          0 - ���յ�Ӧ���ź�
  */
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();	//SDA����Ϊ����  
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
	IIC_SCL=0;//ʱ�����0 	   
	return 0;  
}

/**
  * @brief: ����ACKӦ��
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
  * @brief: ������ACKӦ��
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
  * @brief: IIC����һ���ֽ�
  * @param: txd - ���͵�����
  * @retval: none
  * @note:
  */
void IIC_Send_Byte(u8 txd)
{                        
	u8 t;   
	SDA_OUT(); 	    
	IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
	for(t=0;t<8;t++)
	{              
		IIC_SDA=(txd&0x80)>>7;
		txd<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
	}	 
}
/**
  * @brief: ��1���ֽ�
  * @param: ack - �Ƿ���Ҫ����ACK
  * @arg:   1 - ����ACK
  * @arg:   0 - ����nACK
  * @retval: ��ȡ��������
  * @note:
  */
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
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
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}

