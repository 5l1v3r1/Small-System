
#include "system_type.h"

#include "uart_api.h"

#include "kernel_notice.h"

#include "graphics_library//graphics_window.h"
#include "kernel_expand//kernel_expand_gsm.h"

void hardware_notice(hardware_notice_type notice_type,hardware_notice_parameter parameter1,hardware_notice_parameter parameter2) {
    switch (notice_type) {
        case hardware_notice_lcdtouch_interrupt:
            notice_window(get_window_front(),notice_click,parameter1,parameter2);
            break;
        case hardware_notice_key_interrupt:
            break;
        case hardware_notice_uart_interrupt:
            gsm_notice((const char*)parameter1);
            break;
        default:
            break;
    }
}

#define MAX_BIND_EVENT_COUNT 4

typedef struct {
    kernel_notice_type  notice_type;
    kernel_notice_event notice_event[MAX_BIND_EVENT_COUNT];
} kernel_notice_struct;

#define MAX_NOTICE_COUNT 8

static struct {
    unsigned long event_count;
    kernel_notice_struct notice_event[MAX_NOTICE_COUNT];
} kernel_notice_data;

kernel_notice_handle create_notice(kernel_notice_type notice_type,kernel_notice_event notice_event) {
    unsigned long find_notice_index=0;
    unsigned long find_event_index=0;
    
    for (;find_notice_index<MAX_NOTICE_COUNT;++find_notice_index) {
        if (kernel_notice_data.notice_event[find_notice_index].notice_type==notice_type ||
            kernel_notice_data.notice_event[find_notice_index].notice_type==kernel_notice_no) {
            for (;find_event_index<MAX_BIND_EVENT_COUNT;++find_event_index) {
                if (kernel_notice_data.notice_event[find_notice_index].notice_event[find_event_index]==NULL) {
                    kernel_notice_data.notice_event[find_notice_index].notice_event[find_event_index]=notice_event;
                    return find_event_index;
                }
            }
        }
    }
    return INVALID_EVENT_HANDLE;
}

void delete_notice(kernel_notice_type notice_type,kernel_notice_handle notice_handle) {
    unsigned long find_notice_index=0;
    unsigned long find_event_index=0;
    
    for (;find_notice_index<MAX_NOTICE_COUNT;++find_notice_index) {
        if (kernel_notice_data.notice_event[find_notice_index].notice_type==notice_type) {
            kernel_notice_data.notice_event[find_notice_index].notice_event[notice_handle]=NULL;
            for (;find_event_index<MAX_BIND_EVENT_COUNT;++find_event_index)
                if (kernel_notice_data.notice_event[find_notice_index].notice_event[find_event_index]!=NULL)
                    return;
            kernel_notice_data.notice_event[find_notice_index].notice_type=kernel_notice_no;
            return ;
        }
    }
}

void kernel_notice(kernel_notice_type notice_type,kernel_notice_parameter parameter1,kernel_notice_parameter parameter2) {
    unsigned long find_notice_index=0;
    unsigned long find_event_index=0;
    
    for (;find_notice_index<MAX_NOTICE_COUNT;++find_notice_index) {
        if (kernel_notice_data.notice_event[find_notice_index].notice_type==notice_type) {
            for (;find_event_index<MAX_BIND_EVENT_COUNT;++find_event_index)
                if (kernel_notice_data.notice_event[find_notice_index].notice_event[find_event_index]!=NULL)
                    kernel_notice_data.notice_event[find_notice_index].notice_event[find_event_index](parameter1,parameter2);
        }
    }
}

void kernel_notice_init(void) {
    unsigned long notice_clean_index=0;
    unsigned long event_clean_index=0;
    kernel_notice_data.event_count=0;
    
    for (;notice_clean_index<MAX_NOTICE_COUNT;++notice_clean_index) {
        for (event_clean_index=0;event_clean_index<MAX_BIND_EVENT_COUNT;++event_clean_index) {
            kernel_notice_data.notice_event[notice_clean_index].notice_type=kernel_notice_no;
            kernel_notice_data.notice_event[notice_clean_index].notice_event[event_clean_index]=NULL;
        }
    }
}
