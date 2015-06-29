/*******************************************************************************
 * @file    : 24cxx.c
 * @author  : leon
 * @web     : www.ucortex.com
 * @version : V1.0
 * @date    : 2013-05-02
 * @brief   : AT24Cxx�ײ�����
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
#include "24cxx.h" 
#include "delay.h" 										 

/**
  * @brief: ��AT24CXXָ����ַ����һ������
  * @param: ReadAddr - ������ַ
  * @retval: ����������
	* @note:
	*/
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);				//����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);	//���͸ߵ�ַ	    
	}
	else
		IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));	//����������ַ0XA0,д���� 	   
	IIC_Wait_Ack(); 
	IIC_Send_Byte(ReadAddr%256);	//���͵͵�ַ
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);					//�������ģʽ			   
	IIC_Wait_Ack();	 
	temp=IIC_Read_Byte(0);		   
	IIC_Stop();										//����һ��ֹͣ����	    
	return temp;
}

/**
  * @brief: ��AT24CXXָ����ַд��һ������
  * @param: WriteAddr   - д�����ݵ�Ŀ�ĵ�ַ    
  * @param: DataToWrite - Ҫд�������
  * @retval: none
  * @note:
  */
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
	IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);				//����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ	  
	}
	else
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));	//����������ַ0XA0,д���� 	 
	IIC_Wait_Ack();	   
	IIC_Send_Byte(WriteAddr%256);	//���͵͵�ַ
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);		//�����ֽ�							   
	IIC_Wait_Ack();  		    	   
	IIC_Stop();										//����һ��ֹͣ���� 
	delay_ms(10);	 
}

/**
  * @brief: ��AT24CXX�����ָ����ַ��ʼд�볤��ΪLen������,�ú�������
  *         д��16bit����32bit������.
  * @param: WriteAddr  - ��ʼд��ĵ�ַ  
  * @param: DataToWrite- ���������׵�ַ
  * @param: Len        -Ҫд�����ݵĳ���2,4
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
  * @brief: ��AT24CXX�����ָ����ַ��ʼ��������ΪLen������
  *         �ú������ڶ���16bit����32bit������.
  * @param: ReadAddr - ��ʼ�����ĵ�ַ
  * @param: Len      - Ҫ�������ݵĳ���2,4
  * @retval: ����������
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
  * @brief: ���AT24CXX�Ƿ�����
  * @retval: 0 - ���ɹ�  !0 - ���ʧ��
  * @note:
  *   - ��������24XX�����һ����ַ(255)���洢��־��.
  *     ���������24Cϵ��,�����ַҪ�޸�
  */
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//����ÿ�ο�����дAT24CXX			   
	if(temp==0X55)return 0;		   
	else//�ų���һ�γ�ʼ�������
	{
		AT24CXX_WriteOneByte(255,0X55);
		temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

/**
  * @brief: ��AT24CXX�����ָ����ַ��ʼ����ָ������������
  * @param: ReadAddr - ��ʼ�����ĵ�ַ ��24c02Ϊ0~255
  * @param: pBuffer  - ���������׵�ַ
  * @param: NumToRead- Ҫ�������ݵĸ���
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
  * @brief: ��AT24CXX�����ָ����ַ��ʼд��ָ������������
  * @param: WriteAddr - ��ʼд��ĵ�ַ ��24c02Ϊ0~255
  * @param: pBuffer   - ���������׵�ַ
  * @param:NumToWrite - Ҫд�����ݵĸ���
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








