
#ifndef _THREAD_API_H__
#define _THREAD_API_H__

#include "thread_api.h"

typedef struct {
    long heap_address_header;
    long heap_address_tailer;
    //  ¸÷ÖÖ¼Ä´æÆ÷..
} thread_context;

inline void thread_context_load_next(void);
inline void thread_context_store_this(void);

inline void thread_interrupt_point_run(void);

#endif