
#include "system_type.h"
#include "system_interrupt.h"

#include "led_api.h"
#include "key_api.h"

#include "kernel_notice.h"

void key_interrupt(key_code in_key_code,bool in_key_stat) {
    if (in_key_stat)
        led_work_on();
    else
        led_work_off();
}

void uart_receive_interrupt(const char* uart_receive_data) {
    hardware_notice(hardware_notice_uart_interrupt,(hardware_notice_parameter)uart_receive_data,0);
}
