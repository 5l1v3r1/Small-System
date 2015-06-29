
#include <string.h>
#include <stdio.h>

#include "delay.h"
#include "lcd_api.h"
#include "uart_api.h"

#include "kernel_notice.h"
#include "kernel_expand_gsm.h"

#include "telephone_ui.h"

#define MAX_GSM_RECEIVE_COMMAND_LENGTH 64

static void gsm_command(const char* send_command) {
    uart_send(send_command);
}

void gsm_reset(void) {
    gsm_command("ATZ");
}

void gsm_init(void) {
    gsm_reset();
}

#define MAX_TELEPHONE_NUMBER_LENGTH 20

char ring_number_buffer[MAX_TELEPHONE_NUMBER_LENGTH]={0};

static void gsm_get_ring_number(void) {
    gsm_command("AT+CLCC");
}

void gsm_notice(const char* uart_receive) {
    char receive_buffer[10]={0};
    if (!strcmp(uart_receive,"OK")) {
        return;
    }
    if (!strcmp(uart_receive,"ERROR")) {
        return;
    }
    if (!strcmp(uart_receive,"NO CARRIER")) {
        telephone_dial_init("",dial_hang);
        return;
    }
    if (!strcmp(uart_receive,"RING")) {
        gsm_get_ring_number();
        return;
    }
    if (!strcmp(uart_receive,"BUSY")) {
        return;
    }
    memcpy(receive_buffer,uart_receive,5);
    if (!strcmp(receive_buffer,"RING:")) {
        memcpy(ring_number_buffer,uart_receive+5,strlen(uart_receive)-5);
        telephone_dial_init(ring_number_buffer,dial_ring);
        memset(ring_number_buffer,0,MAX_TELEPHONE_NUMBER_LENGTH);
        return;
    }
}

bool gsm_call(const char* telephone_number) {
    char link_string[MAX_TELEPHONE_NUMBER_LENGTH+4]={0};
    if (strlen(telephone_number)<MAX_TELEPHONE_NUMBER_LENGTH) {
        sprintf(link_string,"ATD%s;",telephone_number);
        gsm_command(link_string);
        return true;
    }
    return false;
}

void gsm_handup(void) {
    gsm_command("ATH");
}

void gsm_answer(void) {
    gsm_command("ATA");
}
