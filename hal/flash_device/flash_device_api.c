/*******************************************************************************
 * @file    : flash.c
 * @author  : leon
 * @web     : www.ucortex.com
 * @version : V1.0
 * @date    : 2013-05-02
 * @brief   : SPI Flash�����ļ�
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
 * 
 *-----------------------------------------------------------------------------
 *                                 ���ļ�¼
 * ----------------------------------------------------------------------------
 * ����ʱ�䣺2013-05-02    �����ˣ�Leon
 * �汾��¼��V1.0
 * �������ݣ��½�
 * ----------------------------------------------------------------------------
 *
 ******************************************************************************/

#include "system_config.h" 

#include "flash_device_api.h"
#include "..//delay.h"

long SPI_FLASH_TYPE=W25Q64;//Ĭ�Ͼ���25Q64

#define SPI_SPEED_2         0
#define SPI_SPEED_4	        1
#define SPI_SPEED_8         2
#define SPI_SPEED_16        3
#define SPI_SPEED_32        4
#define SPI_SPEED_64        5
#define SPI_SPEED_128       6
#define SPI_SPEED_256       7

#define	SPI_FLASH_CS	PBout(12)	//W25Q64

#define SPI_FLASH_SELECT()      {SPI_FLASH_CS=0;}
#define SPI_FLASH_DESELECT()	{SPI_FLASH_CS=1;}

void SPI2_SetSpeed(u8 SpeedSet)
{
	SpeedSet&=0X07;			//���Ʒ�Χ
	SPI2->CR1&=0XFFC7; 
	SPI2->CR1|=SpeedSet<<3;	//����SPI2�ٶ�  
	SPI2->CR1|=1<<6; 		//SPI�豸ʹ��	  
} 

u8 SPI2_ReadWriteByte(u8 TxData)
{		
	u16 retry=0;				 
	while((SPI2->SR&1<<1)==0)		//�ȴ���������	
	{
		retry++;
		if(retry>=0XFFFE)return 0; 	//��ʱ�˳�
	}			  
	SPI2->DR=TxData;	 	  		//����һ��byte 
	retry=0;
	while((SPI2->SR&1<<0)==0) 		//�ȴ�������һ��byte  
	{
		retry++;
		if(retry>=0XFFFE)return 0;	//��ʱ�˳�
	}	  						    
	return SPI2->DR;          		//�����յ�������				    
}

// 1 Sector = 4Kbytes
// 1 Block  = 16 Sectors
// W25Q64 : ����Ϊ8M�ֽ�,����128��Block,2048��Sector 
													 
/** @brief: SPI2��ʼ�������ó�����ģʽ
	* @param: none
	* @retval: none
	* @note:
	*   - W25Q64,NRF24L01,ENC28J60����SPI2
	*   - SPI2_MOSI -> PB15
	*   - SPI2_MISO -> PB14
	*   - SPI2_SCK  -> PB13
	*   - F_CS      -> PB12	(W25Q64 SPIѡͨ)
	*   - NET_CS    -> PG7	(ENC28J60 SPIѡͨ)
	*   - NRF_CS    -> PG15	(NRF24L01 SPIѡͨ)
	*/
void SPI_Flash_Init(void)
{
	RCC->APB2ENR|=1<<3;			//ʹ��PORTBʱ�� 
	//F_CS: W25Q64
	GPIOB->CRH&=0XFFF0FFFF; 
	GPIOB->CRH|=0X00030000;	//PB12 ���� 	    
	GPIOB->ODR|=1<<12;    	//PB12����
	RCC->APB2ENR|=1<<3;  	//PORTBʱ��ʹ��
	RCC->APB1ENR|=1<<14;   	//SPI2ʱ��ʹ��
	//����ֻ���SPI�ڳ�ʼ��
	GPIOB->CRH&=0X000FFFFF; 
	GPIOB->CRH|=0XBBB00000;	//PB13/14/15���� 	    
	GPIOB->ODR|=0X7<<13;   	//PB13/14/15����
	SPI2->CR1|=0<<10;		//ȫ˫��ģʽ	
	SPI2->CR1|=1<<9; 		//���nss����
	SPI2->CR1|=1<<8;  
	SPI2->CR1|=1<<2; 		//SPI����
	SPI2->CR1|=0<<11;		//8bit���ݸ�ʽ	
	SPI2->CR1|=1<<1; 		//����ģʽ��SCKΪ1 CPOL=1
	SPI2->CR1|=1<<0; 		//���ݲ����ӵڶ���ʱ����ؿ�ʼ,CPHA=1  
	//��SPI2����APB1������.ʱ��Ƶ�����Ϊ36M.
	SPI2->CR1|=3<<3; 		//Fsck=Fpclk1/256
	SPI2->CR1|=0<<7; 		//MSBfirst   
	SPI2->CR1|=1<<6; 		//SPI�豸ʹ��
	SPI2_ReadWriteByte(0xff);//��������	
	
	SPI2_SetSpeed(SPI_SPEED_2);				//����Ϊ18Mʱ��,����ģʽ
	SPI_FLASH_TYPE=SPI_Flash_ReadID();//��ȡFLASH ID.
}  

/** @brief: ��ȡSPI_FLASH��״̬�Ĵ���
	* @retval: SPI_FLASH״̬�Ĵ���ֵ
	* @note:
  *   - BIT7  6   5   4   3   2   1   0
  *   - SPR   RV  TB BP2 BP1 BP0 WEL BUSY
  *   - SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
  *   - TB,BP2,BP1,BP0:FLASH����д��������
  *   - WEL:дʹ������
  *   - BUSY:æ���λ(1,æ;0,����)
  *   - Ĭ��:0x00
	*/
u8 SPI_Flash_ReadSR(void)   
{  
	u8 byte=0;   
	SPI_FLASH_SELECT();											//ʹ������   
	SPI2_ReadWriteByte(W25X_ReadStatusReg);	//���Ͷ�ȡ״̬�Ĵ�������    
	byte=SPI2_ReadWriteByte(0Xff);					//��ȡһ���ֽ�  
	SPI_FLASH_DESELECT();										//ȡ��Ƭѡ     
	return byte;   
}

/** @brief: дSPI_FLASH״̬�Ĵ���
	* @param: sr - д���ֵ
	* @retval: none
	* @note: ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!
	*/
void SPI_FLASH_Write_SR(u8 sr)   
{   
	SPI_FLASH_SELECT();                            //ʹ������   
	SPI2_ReadWriteByte(W25X_WriteStatusReg);   //����дȡ״̬�Ĵ�������    
	SPI2_ReadWriteByte(sr);               //д��һ���ֽ�  
	SPI_FLASH_DESELECT();                            //ȡ��Ƭѡ     	      
}   

/** 
  * @brief: SPI_FLASHдʹ�ܣ���WEL��λ
	*/
void SPI_FLASH_Write_Enable(void)   
{
	SPI_FLASH_SELECT();										//ʹ������   
	SPI2_ReadWriteByte(W25X_WriteEnable);	//����дʹ��  
	SPI_FLASH_DESELECT();									//ȡ��Ƭѡ     	      
} 

/** @brief: SPI_FLASHд��ֹ����WEL����
	* @param: 
	* @retval: 
	* @note:
	*/
void SPI_FLASH_Write_Disable(void)   
{  
	SPI_FLASH_SELECT();                            //ʹ������   
    SPI2_ReadWriteByte(W25X_WriteDisable);     //����д��ָֹ��    
	SPI_FLASH_DESELECT();                            //ȡ��Ƭѡ     	      
} 		
/**
  * @brief: ��ȡоƬID
  * @retval: ����ֵ����:				   
  *   0XEF13 - ��ʾоƬ�ͺ�ΪW25Q80  
  *   0XEF14 - ��ʾоƬ�ͺ�ΪW25Q16    
  *   0XEF15 - ��ʾоƬ�ͺ�ΪW25Q32  
  *   0XEF16 - ��ʾоƬ�ͺ�ΪW25Q64 
  */
u16 SPI_Flash_ReadID(void)
{
	u16 Temp = 0;	  
	SPI_FLASH_SELECT();				    
	SPI2_ReadWriteByte(0x90);//���Ͷ�ȡID����	    
	SPI2_ReadWriteByte(0x00); 	    
	SPI2_ReadWriteByte(0x00); 	    
	SPI2_ReadWriteByte(0x00); 	 			   
	Temp|=SPI2_ReadWriteByte(0xFF)<<8;  
	Temp|=SPI2_ReadWriteByte(0xFF);	 
	SPI_FLASH_DESELECT();				    
	return Temp;
}   		    

/** @brief: ��ȡSPI FLASH����ָ����ַ��ʼ��ȡָ�����ȵ�����
	* @param: pBuffer - ���ݴ洢��
  * @param: ReadAddr - ��ʼ��ȡ�ĵ�ַ(24bit)
  * @param: NumByteToRead - Ҫ��ȡ���ֽ���(���65535)
	* @retval: none
	* @note:
	*/
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
 	u16 i;   										    
	SPI_FLASH_SELECT();                            //ʹ������   
    SPI2_ReadWriteByte(W25X_ReadData);         //���Ͷ�ȡ����   
    SPI2_ReadWriteByte((u8)((ReadAddr)>>16));  //����24bit��ַ    
    SPI2_ReadWriteByte((u8)((ReadAddr)>>8));   
    SPI2_ReadWriteByte((u8)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=SPI2_ReadWriteByte(0XFF);   //ѭ������  
    }
	SPI_FLASH_DESELECT();  				    	      
}  

/** @brief: SPI��һҳ(0~65535)��д������256���ֽڵ�����
	* @param: pBuffer - ���ݴ洢��
  * @param: WriteAddr - ��ʼд��ĵ�ַ(24bit)
  * @param: NumByteToWrite - Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
	* @retval: 
	* @note:
	*/
void SPI_Flash_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;  
    SPI_FLASH_Write_Enable();                 	 //SET WEL 
	SPI_FLASH_SELECT();														//ʹ������   
    SPI2_ReadWriteByte(W25X_PageProgram);				//����дҳ����   
    SPI2_ReadWriteByte((u8)((WriteAddr)>>16));	//����24bit��ַ    
    SPI2_ReadWriteByte((u8)((WriteAddr)>>8));   
    SPI2_ReadWriteByte((u8)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)SPI2_ReadWriteByte(pBuffer[i]);//ѭ��д��  
	SPI_FLASH_DESELECT();		//ȡ��Ƭѡ 
	SPI_Flash_Wait_Busy();	//�ȴ�д�����
} 

/** @brief: �޼���дSPI FLASH 
	* @param: pBuffer - ���ݴ洢��
  * @param: WriteAddr - ��ʼд��ĵ�ַ(24bit)
  * @param: NumByteToWrite:Ҫд����ֽ���(���65535)
	* @retval: none
	* @note:
  *   - ����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
  *   - �����Զ���ҳ���� 
  *   - ��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
	*/
void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		SPI_Flash_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	};	    
} 

/** @brief: дSPI FLASH����ָ����ַ��ʼд��ָ�����ȵ�����
	* @param: pBuffer - ���ݴ洢��
  * @param: WriteAddr - ��ʼд��ĵ�ַ(24bit)	
  * @param: NumByteToWrite:Ҫд����ֽ���(���65535)
	* @retval: none
	* @note:
  *   - �ú�������������!
	*/
u8 SPI_FLASH_BUFFER[4096];		 
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    
	u8 * SPI_FLASH_BUF;	  

	SPI_FLASH_BUF=SPI_FLASH_BUFFER;	     
 	secpos=WriteAddr/4096;//������ַ  
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);	//������
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;	//������4096���ֽ�
	while(1) 
	{	
		SPI_Flash_Read(SPI_FLASH_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;//��Ҫ����  	  
		}
		if(i<secremain)										//��Ҫ����
		{
			SPI_Flash_Erase_Sector(secpos);	//�����������
			for(i=0;i<secremain;i++)				//����
			{
				SPI_FLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,secpos*4096,4096);//д����������  

		}
		else
			SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumByteToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		   	pBuffer+=secremain;  //ָ��ƫ��
			WriteAddr+=secremain;//д��ַƫ��	   
		   	NumByteToWrite-=secremain;				//�ֽ����ݼ�
			if(NumByteToWrite>4096)secremain=4096;	//��һ����������д����
			else secremain=NumByteToWrite;			//��һ����������д����
		}	 
	};	 
}

/** @brief: ��������оƬ
	* @note:�ȴ�ʱ�䳬��...
	*/
void SPI_Flash_Erase_Chip(void)   
{                                   
    SPI_FLASH_Write_Enable();						//SET WEL 
    SPI_Flash_Wait_Busy();   
  	SPI_FLASH_SELECT();									//ʹ������   
    SPI2_ReadWriteByte(W25X_ChipErase);	//����Ƭ��������  
	SPI_FLASH_DESELECT();									//ȡ��Ƭѡ     	      
	SPI_Flash_Wait_Busy();								//�ȴ�оƬ��������
}   

/** @brief: ����һ������
	* @param: Dst_Addr - ������ַ,����ʵ����������
	* @retval: 
	* @note: ����һ������������ʱ��:150ms
	*/
void SPI_Flash_Erase_Sector(u32 Dst_Addr)   
{  
	//����falsh�������,������   
// 	printf("fe:%x\r\n",Dst_Addr);	  
 	Dst_Addr*=4096;
	SPI_FLASH_Write_Enable();									//SET WEL 	 
	SPI_Flash_Wait_Busy();   
	SPI_FLASH_SELECT();												//ʹ������   
	SPI2_ReadWriteByte(W25X_SectorErase);			//������������ָ�� 
	SPI2_ReadWriteByte((u8)((Dst_Addr)>>16));	//����24bit��ַ    
	SPI2_ReadWriteByte((u8)((Dst_Addr)>>8));   
	SPI2_ReadWriteByte((u8)Dst_Addr);  
	SPI_FLASH_DESELECT();											//ȡ��Ƭѡ     	      
	SPI_Flash_Wait_Busy();										//�ȴ��������
}

/**
  * @brief: �ȴ�����
	*/
void SPI_Flash_Wait_Busy(void)   
{   
	while((SPI_Flash_ReadSR()&0x01)==0x01);	// �ȴ�BUSYλ���
}  

/**
  * @brief: �������ģʽ
	*/
void SPI_Flash_PowerDown(void)   
{ 
	SPI_FLASH_SELECT();									//ʹ������   
	SPI2_ReadWriteByte(W25X_PowerDown);	//���͵�������  
	SPI_FLASH_DESELECT();								//ȡ��Ƭѡ     	      
	delay_us(3);												//�ȴ�TPD  
}   

/**
  * @brief: ����
	*/
void SPI_Flash_WAKEUP(void)   
{  
	SPI_FLASH_SELECT();													//ʹ������   
	SPI2_ReadWriteByte(W25X_ReleasePowerDown);	//  send W25X_PowerDown command 0xAB    
	SPI_FLASH_DESELECT();												//ȡ��Ƭѡ     	      
	delay_us(3);																//�ȴ�TRES1
}   
