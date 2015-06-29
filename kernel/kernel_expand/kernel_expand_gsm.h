
#ifndef _KERNEL_EXPAND_GSM_H__
#define _KERNEL_EXPAND_GSM_H__

#include "system_type.h"

void gsm_reset(void);
void gsm_init(void);

void gsm_notice(const char* uart_receive);

bool gsm_call(const char* telephone_number);
void gsm_handup(void);
void gsm_answer(void);

#endif
