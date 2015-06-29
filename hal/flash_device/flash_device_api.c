/*******************************************************************************
 * @file    : flash.c
 * @author  : leon
 * @web     : www.ucortex.com
 * @version : V1.0
 * @date    : 2013-05-02
 * @brief   : SPI Flash驱动文件
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
 * 
 *-----------------------------------------------------------------------------
 *                                 更改记录
 * ----------------------------------------------------------------------------
 * 更改时间：2013-05-02    更改人：Leon
 * 版本记录：V1.0
 * 更改内容：新建
 * ----------------------------------------------------------------------------
 *
 ******************************************************************************/

#include "system_config.h" 

#include "flash_device_api.h"
#include "..//delay.h"

long SPI_FLASH_TYPE=W25Q64;//默认就是25Q64

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
	SpeedSet&=0X07;			//限制范围
	SPI2->CR1&=0XFFC7; 
	SPI2->CR1|=SpeedSet<<3;	//设置SPI2速度  
	SPI2->CR1|=1<<6; 		//SPI设备使能	  
} 

u8 SPI2_ReadWriteByte(u8 TxData)
{		
	u16 retry=0;				 
	while((SPI2->SR&1<<1)==0)		//等待发送区空	
	{
		retry++;
		if(retry>=0XFFFE)return 0; 	//超时退出
	}			  
	SPI2->DR=TxData;	 	  		//发送一个byte 
	retry=0;
	while((SPI2->SR&1<<0)==0) 		//等待接收完一个byte  
	{
		retry++;
		if(retry>=0XFFFE)return 0;	//超时退出
	}	  						    
	return SPI2->DR;          		//返回收到的数据				    
}

// 1 Sector = 4Kbytes
// 1 Block  = 16 Sectors
// W25Q64 : 容量为8M字节,共有128个Block,2048个Sector 
													 
/** @brief: SPI2初始化，配置成主机模式
	* @param: none
	* @retval: none
	* @note:
	*   - W25Q64,NRF24L01,ENC28J60共用SPI2
	*   - SPI2_MOSI -> PB15
	*   - SPI2_MISO -> PB14
	*   - SPI2_SCK  -> PB13
	*   - F_CS      -> PB12	(W25Q64 SPI选通)
	*   - NET_CS    -> PG7	(ENC28J60 SPI选通)
	*   - NRF_CS    -> PG15	(NRF24L01 SPI选通)
	*/
void SPI_Flash_Init(void)
{
	RCC->APB2ENR|=1<<3;			//使能PORTB时钟 
	//F_CS: W25Q64
	GPIOB->CRH&=0XFFF0FFFF; 
	GPIOB->CRH|=0X00030000;	//PB12 推挽 	    
	GPIOB->ODR|=1<<12;    	//PB12上拉
	RCC->APB2ENR|=1<<3;  	//PORTB时钟使能
	RCC->APB1ENR|=1<<14;   	//SPI2时钟使能
	//这里只针对SPI口初始化
	GPIOB->CRH&=0X000FFFFF; 
	GPIOB->CRH|=0XBBB00000;	//PB13/14/15复用 	    
	GPIOB->ODR|=0X7<<13;   	//PB13/14/15上拉
	SPI2->CR1|=0<<10;		//全双工模式	
	SPI2->CR1|=1<<9; 		//软件nss管理
	SPI2->CR1|=1<<8;  
	SPI2->CR1|=1<<2; 		//SPI主机
	SPI2->CR1|=0<<11;		//8bit数据格式	
	SPI2->CR1|=1<<1; 		//空闲模式下SCK为1 CPOL=1
	SPI2->CR1|=1<<0; 		//数据采样从第二个时间边沿开始,CPHA=1  
	//对SPI2属于APB1的外设.时钟频率最大为36M.
	SPI2->CR1|=3<<3; 		//Fsck=Fpclk1/256
	SPI2->CR1|=0<<7; 		//MSBfirst   
	SPI2->CR1|=1<<6; 		//SPI设备使能
	SPI2_ReadWriteByte(0xff);//启动传输	
	
	SPI2_SetSpeed(SPI_SPEED_2);				//设置为18M时钟,高速模式
	SPI_FLASH_TYPE=SPI_Flash_ReadID();//读取FLASH ID.
}  

/** @brief: 读取SPI_FLASH的状态寄存器
	* @retval: SPI_FLASH状态寄存器值
	* @note:
  *   - BIT7  6   5   4   3   2   1   0
  *   - SPR   RV  TB BP2 BP1 BP0 WEL BUSY
  *   - SPR:默认0,状态寄存器保护位,配合WP使用
  *   - TB,BP2,BP1,BP0:FLASH区域写保护设置
  *   - WEL:写使能锁定
  *   - BUSY:忙标记位(1,忙;0,空闲)
  *   - 默认:0x00
	*/
u8 SPI_Flash_ReadSR(void)   
{  
	u8 byte=0;   
	SPI_FLASH_SELECT();											//使能器件   
	SPI2_ReadWriteByte(W25X_ReadStatusReg);	//发送读取状态寄存器命令    
	byte=SPI2_ReadWriteByte(0Xff);					//读取一个字节  
	SPI_FLASH_DESELECT();										//取消片选     
	return byte;   
}

/** @brief: 写SPI_FLASH状态寄存器
	* @param: sr - 写入的值
	* @retval: none
	* @note: 只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!
	*/
void SPI_FLASH_Write_SR(u8 sr)   
{   
	SPI_FLASH_SELECT();                            //使能器件   
	SPI2_ReadWriteByte(W25X_WriteStatusReg);   //发送写取状态寄存器命令    
	SPI2_ReadWriteByte(sr);               //写入一个字节  
	SPI_FLASH_DESELECT();                            //取消片选     	      
}   

/** 
  * @brief: SPI_FLASH写使能，将WEL置位
	*/
void SPI_FLASH_Write_Enable(void)   
{
	SPI_FLASH_SELECT();										//使能器件   
	SPI2_ReadWriteByte(W25X_WriteEnable);	//发送写使能  
	SPI_FLASH_DESELECT();									//取消片选     	      
} 

/** @brief: SPI_FLASH写禁止，将WEL清零
	* @param: 
	* @retval: 
	* @note:
	*/
void SPI_FLASH_Write_Disable(void)   
{  
	SPI_FLASH_SELECT();                            //使能器件   
    SPI2_ReadWriteByte(W25X_WriteDisable);     //发送写禁止指令    
	SPI_FLASH_DESELECT();                            //取消片选     	      
} 		
/**
  * @brief: 读取芯片ID
  * @retval: 返回值如下:				   
  *   0XEF13 - 表示芯片型号为W25Q80  
  *   0XEF14 - 表示芯片型号为W25Q16    
  *   0XEF15 - 表示芯片型号为W25Q32  
  *   0XEF16 - 表示芯片型号为W25Q64 
  */
u16 SPI_Flash_ReadID(void)
{
	u16 Temp = 0;	  
	SPI_FLASH_SELECT();				    
	SPI2_ReadWriteByte(0x90);//发送读取ID命令	    
	SPI2_ReadWriteByte(0x00); 	    
	SPI2_ReadWriteByte(0x00); 	    
	SPI2_ReadWriteByte(0x00); 	 			   
	Temp|=SPI2_ReadWriteByte(0xFF)<<8;  
	Temp|=SPI2_ReadWriteByte(0xFF);	 
	SPI_FLASH_DESELECT();				    
	return Temp;
}   		    

/** @brief: 读取SPI FLASH，在指定地址开始读取指定长度的数据
	* @param: pBuffer - 数据存储区
  * @param: ReadAddr - 开始读取的地址(24bit)
  * @param: NumByteToRead - 要读取的字节数(最大65535)
	* @retval: none
	* @note:
	*/
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
 	u16 i;   										    
	SPI_FLASH_SELECT();                            //使能器件   
    SPI2_ReadWriteByte(W25X_ReadData);         //发送读取命令   
    SPI2_ReadWriteByte((u8)((ReadAddr)>>16));  //发送24bit地址    
    SPI2_ReadWriteByte((u8)((ReadAddr)>>8));   
    SPI2_ReadWriteByte((u8)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=SPI2_ReadWriteByte(0XFF);   //循环读数  
    }
	SPI_FLASH_DESELECT();  				    	      
}  

/** @brief: SPI在一页(0~65535)内写入少于256个字节的数据
	* @param: pBuffer - 数据存储区
  * @param: WriteAddr - 开始写入的地址(24bit)
  * @param: NumByteToWrite - 要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
	* @retval: 
	* @note:
	*/
void SPI_Flash_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;  
    SPI_FLASH_Write_Enable();                 	 //SET WEL 
	SPI_FLASH_SELECT();														//使能器件   
    SPI2_ReadWriteByte(W25X_PageProgram);				//发送写页命令   
    SPI2_ReadWriteByte((u8)((WriteAddr)>>16));	//发送24bit地址    
    SPI2_ReadWriteByte((u8)((WriteAddr)>>8));   
    SPI2_ReadWriteByte((u8)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)SPI2_ReadWriteByte(pBuffer[i]);//循环写数  
	SPI_FLASH_DESELECT();		//取消片选 
	SPI_Flash_Wait_Busy();	//等待写入结束
} 

/** @brief: 无检验写SPI FLASH 
	* @param: pBuffer - 数据存储区
  * @param: WriteAddr - 开始写入的地址(24bit)
  * @param: NumByteToWrite:要写入的字节数(最大65535)
	* @retval: none
	* @note:
  *   - 必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
  *   - 具有自动换页功能 
  *   - 在指定地址开始写入指定长度的数据,但是要确保地址不越界!
	*/
void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
	while(1)
	{	   
		SPI_Flash_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite; 	  //不够256个字节了
		}
	};	    
} 

/** @brief: 写SPI FLASH，在指定地址开始写入指定长度的数据
	* @param: pBuffer - 数据存储区
  * @param: WriteAddr - 开始写入的地址(24bit)	
  * @param: NumByteToWrite:要写入的字节数(最大65535)
	* @retval: none
	* @note:
  *   - 该函数带擦除操作!
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
 	secpos=WriteAddr/4096;//扇区地址  
	secoff=WriteAddr%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);	//测试用
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;	//不大于4096个字节
	while(1) 
	{	
		SPI_Flash_Read(SPI_FLASH_BUF,secpos*4096,4096);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;//需要擦除  	  
		}
		if(i<secremain)										//需要擦除
		{
			SPI_Flash_Erase_Sector(secpos);	//擦除这个扇区
			for(i=0;i<secremain;i++)				//复制
			{
				SPI_FLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,secpos*4096,4096);//写入整个扇区  

		}
		else
			SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumByteToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		   	pBuffer+=secremain;  //指针偏移
			WriteAddr+=secremain;//写地址偏移	   
		   	NumByteToWrite-=secremain;				//字节数递减
			if(NumByteToWrite>4096)secremain=4096;	//下一个扇区还是写不完
			else secremain=NumByteToWrite;			//下一个扇区可以写完了
		}	 
	};	 
}

/** @brief: 擦除整个芯片
	* @note:等待时间超长...
	*/
void SPI_Flash_Erase_Chip(void)   
{                                   
    SPI_FLASH_Write_Enable();						//SET WEL 
    SPI_Flash_Wait_Busy();   
  	SPI_FLASH_SELECT();									//使能器件   
    SPI2_ReadWriteByte(W25X_ChipErase);	//发送片擦除命令  
	SPI_FLASH_DESELECT();									//取消片选     	      
	SPI_Flash_Wait_Busy();								//等待芯片擦除结束
}   

/** @brief: 擦除一个扇区
	* @param: Dst_Addr - 扇区地址,根据实际容量设置
	* @retval: 
	* @note: 擦除一个扇区的最少时间:150ms
	*/
void SPI_Flash_Erase_Sector(u32 Dst_Addr)   
{  
	//监视falsh擦除情况,测试用   
// 	printf("fe:%x\r\n",Dst_Addr);	  
 	Dst_Addr*=4096;
	SPI_FLASH_Write_Enable();									//SET WEL 	 
	SPI_Flash_Wait_Busy();   
	SPI_FLASH_SELECT();												//使能器件   
	SPI2_ReadWriteByte(W25X_SectorErase);			//发送扇区擦除指令 
	SPI2_ReadWriteByte((u8)((Dst_Addr)>>16));	//发送24bit地址    
	SPI2_ReadWriteByte((u8)((Dst_Addr)>>8));   
	SPI2_ReadWriteByte((u8)Dst_Addr);  
	SPI_FLASH_DESELECT();											//取消片选     	      
	SPI_Flash_Wait_Busy();										//等待擦除完成
}

/**
  * @brief: 等待空闲
	*/
void SPI_Flash_Wait_Busy(void)   
{   
	while((SPI_Flash_ReadSR()&0x01)==0x01);	// 等待BUSY位清空
}  

/**
  * @brief: 进入掉电模式
	*/
void SPI_Flash_PowerDown(void)   
{ 
	SPI_FLASH_SELECT();									//使能器件   
	SPI2_ReadWriteByte(W25X_PowerDown);	//发送掉电命令  
	SPI_FLASH_DESELECT();								//取消片选     	      
	delay_us(3);												//等待TPD  
}   

/**
  * @brief: 唤醒
	*/
void SPI_Flash_WAKEUP(void)   
{  
	SPI_FLASH_SELECT();													//使能器件   
	SPI2_ReadWriteByte(W25X_ReleasePowerDown);	//  send W25X_PowerDown command 0xAB    
	SPI_FLASH_DESELECT();												//取消片选     	      
	delay_us(3);																//等待TRES1
}   
