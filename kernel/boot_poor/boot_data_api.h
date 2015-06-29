
#ifndef _BOOT_DATA_API_H__
#define _BOOT_DATA_API_H__

void boot_data_init(void);

void boot_data_read(void* address,void* data,long length);
void boot_data_write(void* address,void* data,long length);

int boot_data_size(void);
int boot_data_buffer_size(void);

#endif
