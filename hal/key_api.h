
#ifndef _KEY_API_H__
#define _KEY_API_H__

#include "system_type.h"

typedef enum {
    KEY_NO=0,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
} key_code;

void key_init(void);

bool key_stat(key_code in_key_code);

#endif
