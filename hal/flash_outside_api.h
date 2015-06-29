
#ifndef _FLASH_OUTSIDE_API_H__
#define _FLASH_OUTSIDE_API_H__

#define FLASH_PAGE_SIZE 4*1024

void flash_outside_init(void);

void flash_write(void* address,const void* data,long length);
void flash_read (void* address,void* data,long length);

long flash_get_id(void);
long flash_get_size(void);

void flash_erase_all(void);
void flash_erase_page(long page_index);

#endif
