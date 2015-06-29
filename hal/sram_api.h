
#ifndef _SRAM_API_H__
#define _SRAM_API_H__

#include "system_type.h"

#define SRAM_SPACE_FLAG 0xFF

void sram_init (void);
void sram_write(void* address,void* data,long data_length);
void sram_read (const void* address,void* data,long data_length);

long sram_get_size(void);

#endif
