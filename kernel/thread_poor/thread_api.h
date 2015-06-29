
#ifndef _THREAD_API_H__
#define _THREAD_API_H__

#include "system_type.h"

#include "memory_poor\\memory_api.h"

typedef long  thread_handle;
typedef void* thread_parameter;
typedef int (*thread_address) (thread_parameter in_thread_parameter);

typedef enum {
    THREAD_CLOSE,
    THREAD_RUN,
    THREAD_SUSPEND,
} thread_stat;

typedef struct {
    thread_handle  handle;
    thread_address address;
    thread_stat    stat;
//    memory_page    heap;
    bool           is_kernel_thread;
} thread_struct;

#define KERNEL_THREAD_MAX 6

struct {
    thread_struct list[KERNEL_THREAD_MAX];
    long          list_count;
} kernel_thread_statistics;

thread_stat thread_create (const thread_address create_address,const thread_parameter create_parameter,const thread_stat create_flag,thread_struct thread_data);
thread_stat thread_suspend(thread_struct thread_data);
thread_stat thread_resume (thread_struct thread_data);
thread_stat thread_close  (thread_struct thread_data);

bool        thread_heap_create (thread_struct thread_data);
bool        thread_heap_distory(thread_struct thread_data);

#endif
