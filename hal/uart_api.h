
#ifndef _UART_API_H__
#define _UART_API_H__

#include "system_type.h"

#define UART_INTERRUPT_ENABLE

#define DEFAULT_BOUDRATE 9600

#define MAX_UART_BUFFER_LENGTH 256

void uart_init(void);
void uart_send(const char* send_data);
unsigned char uart_receive_bit(void);

void uart_interrupt_on(void);
void uart_interrupt_exit(void);


#endif
