
#include <string.h>

#include "system_type.h"

#include "flash_inside_api.h"

#include "boot_data_api.h"

/*

   BOOT DATA AT FLASH STRUCTION:
   
   | LENGTH | ADDITION  |
   | 2-BIT  | 0-256 BIT |
   
*/

int  boot_data_length=0;
char boot_data[sizeof(int)]={0};

void boot_data_init(void) {
    flash_inside_read(0,&boot_data_length,sizeof(int));
    flash_inside_read((void*)2,boot_data,boot_data_length);
}

void boot_data_read(void* address,void* data,long length){
    memcpy(data,(const void*)((unsigned long)boot_data+(long)address),length);
}
void boot_data_write(void* address,void* data,long length) {
    //  WARNING! This is a null funtion !..
    //  2014/7/4
}

int boot_data_size(void) {
    return sizeof(int);
}
int boot_data_buffer_size(void) {
    return boot_data_length;
}
