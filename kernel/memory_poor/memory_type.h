
#ifndef _MEMORY_TYPE_H__
#define _MEMORY_TYPE_H__

#include "memory_api.h"

typedef struct {
    memory_address point_last;
    memory_address alloc_address;
    long length;
    char alloc_header_flag;  //  flag is 'F'
    protect_flag data_protect;
    share_flag data_share;
    char data_kernel;        //  0 not is kernel
} memory_alloc_header,*point_memory_alloc_header;
typedef struct {
    memory_address point_next;
} memory_alloc_ender,*point_memory_alloc_ender;

#endif
