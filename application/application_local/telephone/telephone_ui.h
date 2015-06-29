
#ifndef _TELEPHONE_UI_H__
#define _TELEPHONE_UI_H__

#include "system_type.h"

bool telephone_init(void);

typedef enum {
    no_dial=0,
    dial_call,
    dial_ring,
    dial_hang,
} telephone_dial_state;

void telephone_dial_init(const char* phone_number,telephone_dial_state show_state);

#endif
