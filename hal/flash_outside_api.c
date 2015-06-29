
#include "stm32f10x_conf.h"

#include "system_config.h"
#include "system_type.h"

#include "flash_outside_api.h"

#include "flash_device//flash_device_api.h"

#define FLASH_SIZE              (8*1024*1024)
/*
#define SPI_FLASH

#ifdef  SPI_FLASH
#define SPI_FLASH_TYPE          W25Q64
#define SPI_NUMBER              SPI2

#if     SPI_FLASH_TYPE==W25Q64
#define W25X_WRITE_ENABLE		0x06
#define W25X_WRITE_DISABLE		0x04
#define W25X_READ_STATUSREG		0x05
#define W25X_WRITE_STATUSREG	0x01
#define W25X_READ_DATA			0x03
#define W25X_FAST_READ_DATA		0x0B
#define W25X_FAST_READ_DUAL		0x3B
#define W25X_PAGE_PROGRAM		0x02
#define W25X_BLOCK_ERASE		0XD8
#define W25X_SECTOR_ERASE		0X20
#define W25X_CHIP_ERASE			0XC7
#define W25X_POWERDOWN			0XB9
#define W25X_RELEASE_POWERDOWN	0XAB
#define W25X_DEVICEID			0XAB
#define W25X_MANUFACT_DEVICEID	0x90
#define W25X_JEDEC_DEVICEID		0x9F
#endif

#define SPI_PORT_CS   GPIOB
#define SPI_PORT_MISO GPIOB
#define SPI_PORT_MOSI GPIOB
#define SPI_PORT_SCK  GPIOB
#define SPI_PIN_CS    GPIO_Pin_12
#define SPI_PIN_MISO  GPIO_Pin_14
#define SPI_PIN_MOSI  GPIO_Pin_15
#define SPI_PIN_SCK   GPIO_Pin_13

static void select_spi(void) {
    GPIO_ResetBits(SPI_PORT_CS,SPI_PIN_CS);
}

static void unselect_spi(void) {
	GPIO_SetBits(SPI_PORT_CS,SPI_PIN_CS);
}

static u8 flash_command(u8 data) {
	while (RESET==SPI_I2S_GetFlagStatus(SPI_NUMBER,SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI_NUMBER,data);
	while (RESET==SPI_I2S_GetFlagStatus(SPI_NUMBER,SPI_I2S_FLAG_RXNE));
	return SPI_I2S_ReceiveData(SPI_NUMBER);	    
}
#endif

void flash_outside_init(void) {
#ifdef SPI_FLASH
  	GPIO_InitTypeDef spi_port;
  	SPI_InitTypeDef  spi_init;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);

	spi_port.GPIO_Pin  =SPI_PIN_CS;
 	spi_port.GPIO_Mode =GPIO_Mode_Out_PP;
	spi_port.GPIO_Speed=GPIO_Speed_50MHz;
 	GPIO_Init(SPI_PORT_CS,&spi_port);
 	GPIO_SetBits(SPI_PORT_CS,SPI_PIN_CS);
 
	spi_port.GPIO_Pin  =SPI_PIN_SCK|SPI_PIN_MISO|SPI_PIN_MOSI;
	spi_port.GPIO_Mode =GPIO_Mode_AF_PP;
	spi_port.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(SPI_PORT_MISO,&spi_port);

 	GPIO_SetBits(SPI_PORT_MISO,SPI_PIN_SCK|SPI_PIN_MISO|SPI_PIN_MOSI);

	spi_init.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi_init.SPI_Mode = SPI_Mode_Master;
	spi_init.SPI_DataSize = SPI_DataSize_8b;
	spi_init.SPI_CPOL = SPI_CPOL_High;
	spi_init.SPI_CPHA = SPI_CPHA_2Edge;
	spi_init.SPI_NSS = SPI_NSS_Soft;
	spi_init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	spi_init.SPI_FirstBit = SPI_FirstBit_MSB;
	spi_init.SPI_CRCPolynomial = 7;
	SPI_Init(SPI_NUMBER,&spi_init);
    
	SPI_Cmd(SPI_NUMBER,ENABLE);
	flash_command(0xff);

 	SPI_NUMBER->CR1&=0xFFC7;
	SPI_NUMBER->CR1|=SPI_BaudRatePrescaler_2;
	SPI_Cmd(SPI_NUMBER,ENABLE);
#endif
}

long flash_get_id(void) {
#ifdef SPI_FLASH
	u16 id=0;
    select_spi();
	flash_command(W25X_MANUFACT_DEVICEID);
	flash_command(0x00);
	flash_command(0x00);
	flash_command(0x00);
	id|=flash_command(0xFF)<<8;
	id|=flash_command(0xFF);
	unselect_spi();
	return (long)id;
#endif
}

long flash_get_size(void) {
    return FLASH_SIZE;
}

#ifdef SPI_FLASH
static void spi_flash_write_enable(void) {
	select_spi();
    flash_command(W25X_WRITE_ENABLE);
	unselect_spi();
}
static bool spi_flash_is_busy(void)    {  
	u8 busy_bit=0;
	select_spi();
	flash_command(W25X_READ_STATUSREG);
	busy_bit=flash_command(0xFF);
	unselect_spi();
	return (0x01==(busy_bit&0x01))?true:false;
}
#endif

void flash_write(void* address,void* data,long length) {
#ifdef SPI_FLASH
 	u16 write_index=0;
    spi_flash_write_enable();
	select_spi();
    flash_command(W25X_PAGE_PROGRAM);
    flash_command((u8)(long)address>>16);
    flash_command((u8)(long)address>>8);   
    flash_command((u8)(long)address);
    for(;write_index<length;++write_index)
        flash_command(*(char*)((long)data+write_index));
	unselect_spi();
	while (spi_flash_is_busy());
#endif
}
void flash_read(void* address,void* data,long length) {
#ifdef SPI_FLASH
 	u16 read_index=0;
	select_spi();
    flash_command(W25X_READ_DATA);
    flash_command((u8)(long)address>>16);
    flash_command((u8)(long)address>>8);
    flash_command((u8)(long)address);
    for(;read_index<length;++read_index)
        *(char*)((long)data+read_index)=flash_command(0xFF);
	unselect_spi();
#endif
}
*/

void flash_outside_init(void) {
    SPI_Flash_Init();
}

void flash_write(void* address,const void* data,long length) {
    SPI_Flash_Write((u8*)data,(u32)address,length);
}
void flash_read (void* address,void* data,long length) {
    SPI_Flash_Read((u8*)data,(u32)address,length);
}

long flash_get_id(void) {
    return (long)SPI_Flash_ReadID();
}
long flash_get_size(void) {
    return FLASH_SIZE;
}

void flash_erase_all(void) {
    SPI_Flash_Erase_Chip();
}
void flash_erase_page(long page_index) {
    SPI_Flash_Erase_Sector(page_index*FLASH_PAGE_SIZE);
}
