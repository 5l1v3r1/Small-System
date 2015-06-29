
#include "system_config.h"
#include "system_type.h"
#include "system_interrupt.h"

#include <stdio.h>
#include <string.h>

#include "delay.h"
#include "lcd_api.h"
#include "uart_api.h"

#include "kernel_notice.h"
#include "kernel_expand_gsm.h"

void uart_interrupt_on(void) {
    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
}

void uart_interrupt_exit(void) {
    USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);
}

unsigned char uart_receive_bit(void) {
	if(USART_GetITStatus(USART1, USART_IT_RXNE)!=RESET)
        return USART_ReceiveData(USART1);
    return 0xFF;
}

void uart_init(void) {
    USART_InitTypeDef uart;
    GPIO_InitTypeDef  uart_pin;
#ifdef UART_INTERRUPT_ENABLE
	NVIC_InitTypeDef  uart_nvic;
#endif
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
    
    uart_pin.GPIO_Pin=GPIO_Pin_9;
    uart_pin.GPIO_Speed=GPIO_Speed_50MHz;
    uart_pin.GPIO_Mode=GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA,&uart_pin);
    uart_pin.GPIO_Pin = GPIO_Pin_10;
    uart_pin.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA,&uart_pin);
    
    uart.USART_BaudRate=DEFAULT_BOUDRATE;
    uart.USART_WordLength=USART_WordLength_8b;
    uart.USART_StopBits=USART_StopBits_1;
    uart.USART_Parity=USART_Parity_No;
    uart.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
    uart.USART_Mode=USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1,&uart);
    
#ifdef UART_INTERRUPT_ENABLE
    uart_nvic.NVIC_IRQChannel=USART1_IRQn;
	uart_nvic.NVIC_IRQChannelPreemptionPriority=3;
	uart_nvic.NVIC_IRQChannelSubPriority=3;
	uart_nvic.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&uart_nvic);
    
    uart_interrupt_on();
#endif
    USART_Cmd(USART1,ENABLE);
}

void uart_send(const char* send_data) {
    unsigned long data_length=strlen(send_data);
    unsigned long send_index=0;
    
    for (;send_index<data_length;++send_index) {
        while (USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
        USART_SendData(USART1,*(char*)(send_data+send_index));
    }
    while (USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
    USART_SendData(USART1,0x0D);
}

#define MAX_UART_BUFFER_LENGTH 256

#define MAX_TELEPHONE_NUMBER_LENGTH 20
#define OFFSET_PHONE_NUMBER 18

char uart_recv_buffer[MAX_UART_BUFFER_LENGTH]={0};
unsigned long uart_recv_buffer_length=0;

void USART1_IRQHandler(void) {
    unsigned find_flag=OFFSET_PHONE_NUMBER+1;
    char telephone_number[OFFSET_PHONE_NUMBER]={0};
    char output_notice[OFFSET_PHONE_NUMBER+6]={0};

	if(USART_GetITStatus(USART1, USART_IT_RXNE)!=RESET) {
        uart_recv_buffer[uart_recv_buffer_length]=USART_ReceiveData(USART1);
        if (0xFF==(unsigned char)uart_recv_buffer[uart_recv_buffer_length]) {
            uart_recv_buffer[uart_recv_buffer_length]=0;
            return;
        }
        ++uart_recv_buffer_length;
        if (uart_recv_buffer_length>=2) {
            if (uart_recv_buffer[uart_recv_buffer_length-2]==0x0D && 
                uart_recv_buffer[uart_recv_buffer_length-1]==0x0A) {
                uart_recv_buffer[uart_recv_buffer_length-1]=0;
                uart_recv_buffer[uart_recv_buffer_length-2]=0;
                uart_recv_buffer_length-=2;
                if (!uart_recv_buffer_length) return;
                if (uart_recv_buffer[0]=='A' && uart_recv_buffer[1]=='T') {
                    memset(uart_recv_buffer,0,uart_recv_buffer_length);
                    uart_recv_buffer_length=0;
                    delay_us(200);
                    return;
                }
                if (uart_recv_buffer[0]=='+') {
                    for (;find_flag<19+MAX_TELEPHONE_NUMBER_LENGTH;++find_flag)
                        if ('"'==*(char*)(uart_recv_buffer+find_flag))
                            break;
                    memcpy(telephone_number,uart_recv_buffer+OFFSET_PHONE_NUMBER,find_flag-OFFSET_PHONE_NUMBER-1);
                    sprintf(output_notice,"RING:%s",telephone_number);
                    hardware_notice(hardware_notice_uart_interrupt,(hardware_notice_parameter)output_notice,0);
                    //gsm_notice(output_notice);
                    uart_recv_buffer_length=0;
                    delay_us(200);
                    return;
                }
//                lcd_print_line(uart_recv_buffer);
                hardware_notice(hardware_notice_uart_interrupt,(hardware_notice_parameter)uart_recv_buffer,0);
                memset(uart_recv_buffer,0,uart_recv_buffer_length);
                uart_recv_buffer_length=0;
            }
        }
    }
}
