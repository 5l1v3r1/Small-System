/*******************************************************************************
 * @file    : flash.h
 * @author  : leon
 * @web     : www.ucortex.com
 * @version : V1.0
 * @date    : 2013-05-02
 * @brief   : SPI Flash驱动头文件
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
#ifndef _FLASH_DEVICE_H__
#define _FLASH_DEVICE_H__

//W25X系列/Q系列芯片列表
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16

extern long SPI_FLASH_TYPE;		//定义我们使用的flash芯片型号
				 
/*----------------------------------------------------------------------------*/
 
//指令表
#define W25X_WriteEnable			0x06 
#define W25X_WriteDisable			0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData					0x03 
#define W25X_FastReadData			0x0B 
#define W25X_FastReadDual			0x3B 
#define W25X_PageProgram			0x02 
#define W25X_BlockErase				0xD8 
#define W25X_SectorErase			0x20 
#define W25X_ChipErase				0xC7 
#define W25X_PowerDown				0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID					0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 

void SPI_Flash_Init(void);
u16  SPI_Flash_ReadID(void);				//读取FLASH ID
u8 SPI_Flash_ReadSR(void);				//读取状态寄存器 
void SPI_FLASH_Write_SR(u8 sr);			//写状态寄存器
void SPI_FLASH_Write_Enable(void);	//写使能 
void SPI_FLASH_Write_Disable(void);	//写保护
void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);		//读取flash
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);//写入flash
void SPI_Flash_Erase_Chip(void);						//整片擦除
void SPI_Flash_Erase_Sector(u32 Dst_Addr);	//扇区擦除
void SPI_Flash_Wait_Busy(void);							//等待空闲
void SPI_Flash_PowerDown(void);							//进入掉电模式
void SPI_Flash_WAKEUP(void);								//唤醒
#endif
















