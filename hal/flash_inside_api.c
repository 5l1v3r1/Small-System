
#include <string.h>
#include <stdlib.h>

#include "system_config.h"
#include "system_type.h"

#include "flash_inside_api.h"

#define FLASH_INSIDE_ADDRESS_START 0x08070000
#define FLASH_INSIDE_ADDRESS_END   0x0807FFFF
#define FLASH_INSIDE_SIZE          FLASH_INSIDE_ADDRESS_END-FLASH_INSIDE_ADDRESS_START

#define FLASH_PAGE_LENGTH          2048

#define FLASH_SPACE_FLAG_HALFWORD  (u16)0xFFFF

static bool is_valid_address(void* address) {
    return (0<=(long)address && (long)address<=FLASH_INSIDE_ADDRESS_END-FLASH_INSIDE_ADDRESS_START-2)?true:false;
}

void flash_inside_init(void) {
}
long flash_inside_get_size(void) {
    return FLASH_INSIDE_SIZE;
}
void flash_inside_erase_all(void) {
    FLASH_Unlock();
    FLASH_ErasePage(FLASH_INSIDE_ADDRESS_START);
    FLASH_Lock();
    FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
}
void flash_inside_read(void* address,void* data,long length) {
    if (!is_valid_address(address)) return;
    while (length--)
        *(char*)((long)data+length)=*(char*)(FLASH_INSIDE_ADDRESS_START+(long)address+length);
}
void flash_inside_write(void* address,void* data,long length) {
    char old_data[2]={0};
    int  old_data_int=0; 
    long the_last=length-1;

    if (!is_valid_address(address)) return;
    
    FLASH_Unlock();
    if ((long)address%2!=0) {
        flash_inside_read((void*)((long)address-1),old_data,2);
        old_data[1]=*(char*)data;
        memcpy(&old_data_int,old_data,sizeof(int));
        FLASH_ProgramHalfWord((uint32_t)(FLASH_INSIDE_ADDRESS_START+(long)address-1),old_data_int);
        --length;
    }
    for (;length>1;length-=2)
        FLASH_ProgramHalfWord((uint32_t)(FLASH_INSIDE_ADDRESS_START+(long)address+length-2),*(uint16_t*)((long)data+length-2));
    if (length) {
        flash_inside_read((void*)((long)address+the_last),old_data,2);
        old_data[0]=*(char*)((long)data+the_last);
        memcpy(&old_data_int,old_data,sizeof(int));
        FLASH_ProgramHalfWord((uint32_t)(FLASH_INSIDE_ADDRESS_START+(long)address+the_last),old_data_int);
    }
    FLASH_Lock();
    FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
}
