
#ifndef _KERNEL_NOTICE_H__
#define _KERNEL_NOTICE_H__

typedef enum {
    hardware_notice_no=0,
    hardware_notice_lcdtouch_interrupt,
    hardware_notice_key_interrupt,
    hardware_notice_uart_interrupt,
} hardware_notice_type;

typedef unsigned long hardware_notice_parameter;

void hardware_notice(hardware_notice_type notice_type,hardware_notice_parameter parameter1,hardware_notice_parameter parameter2);

typedef unsigned long kernel_notice_parameter;
typedef void (*kernel_notice_event)(kernel_notice_parameter parameter1,kernel_notice_parameter parameter2);
typedef unsigned long kernel_notice_handle;

#define INVALID_EVENT_HANDLE 0xFFFFFFFF

typedef enum {
    kernel_notice_no=0,
    kernel_notice_expand_gsm_ring,
    kernel_notice_expand_gsm_message_receive,
} kernel_notice_type;

kernel_notice_handle create_notice(kernel_notice_type notice_type,kernel_notice_event notice_event);
void delete_notice(kernel_notice_type notice_type,kernel_notice_handle notice_handle);
void kernel_notice(kernel_notice_type notice_type,kernel_notice_parameter parameter1,kernel_notice_parameter parameter2);
void kernel_notice_init(void);

#endif
