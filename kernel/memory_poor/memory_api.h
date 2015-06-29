
#ifndef _MEMORY_API_H__
#define _MEMORY_API_H__

#include "system_type.h"
#include "kernel_type.h"

void kernel_memory_api_init(void);

typedef enum {
    GET_MEMORY_SIZE,
    GET_MEMORY_AVAILABLE,
    GET_MEMORY_UNAVAILABLE,
    GET_MEMORY_USERATE,
    GET_ALLOC_COUNT,
} memory_statistics_flag;

long memory_statistics(const memory_statistics_flag in_memory_statistics_flag);

#define INVALID_MEMORY_ADDRESS -1

void* memory_alloc(const long memory_length);
bool  memory_free (void* alloc_memory);

typedef enum {
    READ=1,
    READ_WRITE,
} protect_flag;

bool memory_page_set_protect(void* alloc_memory_address,const protect_flag flag);

typedef enum {
    SHARE_NO=0,
    SHARE_READ,
    SHARE_WRITE,
    SHARE_ALL,
} share_flag;

bool memory_page_set_share (void* alloc_memory_address,const share_flag flag);
bool memory_page_set_kernel(void* alloc_memory_address,const bool enable);

void memory_read_int    (const void* read_memory_address,int* out_data);
void memory_read_long   (const void* read_memory_address,long* out_data);
void memory_read_float  (const void* read_memory_address,float* out_data);
void memory_read_double (const void* read_memory_address,double* out_data);
void memory_read_char   (const void* read_memory_address,char* out_data);
void memory_read_string (const void* read_memory_address,char* out_data,const memory_length read_memory_data_length);

bool memory_write_int   (void* write_memory_address,const int write_memory_data);
bool memory_write_long  (void* write_memory_address,const long write_memory_data);
bool memory_write_float (void* write_memory_address,const float write_memory_data);
bool memory_write_double(void* write_memory_address,const double write_memory_data);
bool memory_write_char  (void* write_memory_address,const char write_memory_data);
bool memory_write_string(void* write_memory_address,const char* write_memory_data,const memory_length write_memory_data_length);

void memory_set (void* address,char setchar,long length);
void memory_copy(void* dest_addr,const void* sorc_addr,long length);
bool memory_cmp (const void* addr_1,const void* addr_2,long length);

#endif
