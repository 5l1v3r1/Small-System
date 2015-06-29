
#include "system_config.h"
#include "system_type.h"

#include "sram_api.h"

//  SRAM ADDRESS DEFINE
#define SRAM_ADDRESS_START        (long)0x68000000
#define SRAM_ADDRESS_END          (long)0x6BFFFFFF

#define SRAM_SIZE                 (long)0x100000

#define SRAM_PAGE_KERNEL_LENGTH   (long)0xE0000
#define SRAM_ADDRESS_KERNEL_START SRAM_ADDRESS_START
#define SRAM_ADDRESS_KERNEL_END   SRAM_ADDRESS_START+SRAM_PAGE_KERNEL_LENGTH

#define SRAM_PAGE_USER_LENGTH     SRAM_ADDRESS_END-SRAM_ADDRESS_START-SRAM_PAGE_KERNEL_LENGTH
#define SRAM_ADDRESS_USER_START   SRAM_ADDRESS_KERNEL_END+1
#define SRAM_ADDRESS_USER_END     SRAM_ADDRESS_END

//  SRAM BANK DEFINE
//#define SRAM_BANK_1
//#define SRAM_BANK_2
#define SRAM_BANK_3
//#define SRAM_BANK_4

static struct {
#ifdef SRAM_BANK_1
    long sram_bank_1_address_start;
    long sram_bank_1_address_end;
    long sram_bank_1_length;
#endif
#ifdef SRAM_BANK_2
    long sram_bank_2_address_start;
    long sram_bank_2_address_end;
    long sram_bank_2_length;
#endif
#ifdef SRAM_BANK_3
    long sram_bank_3_address_start;
    long sram_bank_3_address_end;
    long sram_bank_3_length;
#endif
#ifdef SRAM_BANK_4
    long sram_bank_4_address_start;
    long sram_bank_4_address_end;
    long sram_bank_4_length;
#endif
    long sram_address_start;
    long sram_address_end;
    long sram_length;
} sram_hardware_information;

//  SRAM BANK INIT CODE DEFINE
#ifdef SRAM_BANK_1
static void sram_port_init_1(void);
static void sram_fsmc_init_1(void);
#endif

#ifdef SRAM_BANK_2
static void sram_port_init_2(void);
static void sram_fsmc_init_2(void);
#endif

#ifdef SRAM_BANK_3
static void sram_port_init_3(void) {
    GPIO_InitTypeDef port;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOG,ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);
    
    port.GPIO_Mode  =GPIO_Mode_AF_PP;
    port.GPIO_Pin   =GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
    port.GPIO_Speed =GPIO_Speed_50MHz;
    GPIO_Init(GPIOD,&port);
    
    port.GPIO_Pin   =GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
    GPIO_Init(GPIOE,&port);
    port.GPIO_Pin   =GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
    GPIO_Init(GPIOF,&port);
    port.GPIO_Pin   =GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_10;
    GPIO_Init(GPIOG,&port);
}

static void sram_fsmc_init_3(void) {
    FSMC_NORSRAMInitTypeDef sram;
    FSMC_NORSRAMTimingInitTypeDef timing;
    
    timing.FSMC_AddressSetupTime      = 0;
    timing.FSMC_AddressHoldTime       = 0;
    timing.FSMC_DataSetupTime         = 3;
    timing.FSMC_BusTurnAroundDuration = 0;
    timing.FSMC_CLKDivision           = 0;
    timing.FSMC_DataLatency           = 0;
    timing.FSMC_AccessMode            = FSMC_AccessMode_A;
    
    sram.FSMC_Bank                    = FSMC_Bank1_NORSRAM3;
    sram.FSMC_DataAddressMux          = FSMC_DataAddressMux_Disable;
    sram.FSMC_MemoryType              = FSMC_MemoryType_SRAM;
    sram.FSMC_MemoryDataWidth         = FSMC_MemoryDataWidth_16b;
    sram.FSMC_BurstAccessMode         = FSMC_BurstAccessMode_Disable;
    sram.FSMC_WaitSignalPolarity      = FSMC_WaitSignalPolarity_Low;
    sram.FSMC_WrapMode                = FSMC_WrapMode_Disable;
    sram.FSMC_WaitSignalActive        = FSMC_WaitSignalActive_BeforeWaitState;
    sram.FSMC_WriteOperation          = FSMC_WriteOperation_Enable;
    sram.FSMC_WaitSignal              = FSMC_WaitSignal_Disable;
    sram.FSMC_ExtendedMode            = FSMC_ExtendedMode_Disable;
    sram.FSMC_AsynchronousWait        = FSMC_AsynchronousWait_Disable;
    sram.FSMC_WriteBurst              = FSMC_WriteBurst_Disable;
    sram.FSMC_ReadWriteTimingStruct   = &timing;
    sram.FSMC_WriteTimingStruct       = &timing;
    
    FSMC_NORSRAMInit(&sram);
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3,ENABLE);
}
#endif

#ifdef SRAM_BANK_4
static void sram_port_init_4(void);
static void sram_fsmc_init_4(void);
#endif

//  SRAM MAIN INIT FUNCTION
void sram_init(void) {
#ifdef SRAM_BANK_1
    sram_port_init_1();
    sram_fsmc_init_1();
#endif
#ifdef SRAM_BANK_2
    sram_port_init_2();
    sram_fsmc_init_2();
#endif
#ifdef SRAM_BANK_3
    sram_port_init_3();
    sram_fsmc_init_3();
    sram_hardware_information.sram_bank_3_address_start=SRAM_ADDRESS_START;
    sram_hardware_information.sram_bank_3_address_end=SRAM_ADDRESS_END;
    sram_hardware_information.sram_bank_3_length=SRAM_ADDRESS_END-SRAM_ADDRESS_START;
#endif
#ifdef SRAM_BANK_4
    sram_port_init_4();
    sram_fsmc_init_4();
#endif
    sram_hardware_information.sram_address_start=SRAM_ADDRESS_START;
    sram_hardware_information.sram_address_end  =SRAM_ADDRESS_END;
    sram_hardware_information.sram_length       =SRAM_SIZE;
}

static bool is_valid_address(const void* address) {
    return (0<=(long)address && (long)address<=SRAM_ADDRESS_KERNEL_END-SRAM_ADDRESS_KERNEL_START)?true:false;
}

void sram_write(void* address,void* data,long data_length) {
    long write_index=0;
    if (!is_valid_address(address)) return;
    for (;write_index<data_length;++write_index)
        *(char*)(SRAM_ADDRESS_START+(long)address+write_index)=*(char*)((long)data+write_index);
}

void sram_read (const void* address,void* data,long data_length) {
    long read_index=0;
    if (!is_valid_address(address)) return;
    for (;read_index<data_length;++read_index)
        *(char*)((long)data+read_index)=*(char*)(SRAM_ADDRESS_START+(long)address+read_index);
}

long sram_get_size(void) {
    return sram_hardware_information.sram_length;
}
