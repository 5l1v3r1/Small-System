
#ifndef _TIME_API_H__
#define _TIME_API_H__

#include "system_type.h"

typedef struct {
    unsigned short year;
    unsigned char month;
    unsigned char day;
    unsigned char hour;
    unsigned char min;
    unsigned char second;
    unsigned char week;
} time,*point_time;

void time_init(void);
void time_exit(void);
void time_get(point_time now_time);
void time_set(point_time new_time);

typedef void (*time_interrupt_recall)(point_time interrupt_time);

bool time_alarm_add(point_time interrupt_time,time_interrupt_recall interrupt_recall);
void time_alarm_delete(point_time interrupt_time);

#endif
