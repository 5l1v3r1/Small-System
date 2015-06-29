
#ifndef _SYSTEM_INTERRUPT_H__
#define _SYSTEM_INTERRUPT_H__

#include "key_api.h"

void key_interrupt(key_code in_key_code,bool key_stat);
void uart_receive_interrupt(const char* uart_receive_data_bit);

#endif
