
#ifndef _FLASH_INSIDE_API_H__
#define _FLASH_INSIDE_API_H__

//  WARNING! 由于FLASH_ProgramHalfWord 函数的写有些问题,于是现在就这样
//  在系统首次安装时,数据写死在FLASH 中,以后只提供读而不提供写..
//  可以全部擦除再写,只不过性能很低
//  2014/7/4

void flash_inside_init (void);
void flash_inside_write(void* address,void* data,long length);
void flash_inside_read (void* address,void* data,long length);
void flash_inside_erase(void* address,long length);
void flash_inside_erase_all(void);

long flasg_inside_get_size(void);

#endif
