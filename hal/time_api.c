
#include "system_type.h"

#include <string.h>

#include "time_api.h"

#include "rtc_device//rtc_device_api.h"

#define MAX_ALARM_EVENT_COUNT 5

struct {
    unsigned long alarm_event_count;
    time alarm_event_list[MAX_ALARM_EVENT_COUNT];
    time_interrupt_recall alarm_event_recall_list[MAX_ALARM_EVENT_COUNT];
} time_alarm;

void time_exit(void) {
    RTC_Exit();
}

void time_init(void) {
    RTC_Init();
    memset(&time_alarm,0,sizeof(time_alarm));
}

void time_get(point_time now_time) {
    if (NULL!=now_time) {
        now_time->year=calendar.w_year;
        now_time->month=calendar.w_month;
        now_time->day=calendar.w_date;
        now_time->hour=calendar.hour;
        now_time->min=calendar.min;
        now_time->second=calendar.sec;
        now_time->week=calendar.week;
    }
}

void time_set(point_time new_time) {
    if (NULL!=new_time)
        RTC_Set(new_time->year,new_time->month,new_time->day,new_time->hour,new_time->min,new_time->second);
}

bool time_alarm_add(point_time interrupt_time,time_interrupt_recall interrupt_recall) {
    if (MAX_ALARM_EVENT_COUNT>time_alarm.alarm_event_count && NULL!=interrupt_recall) {
        if (NULL==interrupt_time->year) {
            time_alarm.alarm_event_list[time_alarm.alarm_event_count].week=interrupt_time->week;
            time_alarm.alarm_event_list[time_alarm.alarm_event_count].hour=interrupt_time->hour;
            time_alarm.alarm_event_list[time_alarm.alarm_event_count].min=interrupt_time->min;
            time_alarm.alarm_event_list[time_alarm.alarm_event_count].second=interrupt_time->second;
        } else {
            time_alarm.alarm_event_list[time_alarm.alarm_event_count].year=interrupt_time->year;
            time_alarm.alarm_event_list[time_alarm.alarm_event_count].month=interrupt_time->month;
            time_alarm.alarm_event_list[time_alarm.alarm_event_count].day=interrupt_time->day;
            time_alarm.alarm_event_list[time_alarm.alarm_event_count].hour=interrupt_time->hour;
            time_alarm.alarm_event_list[time_alarm.alarm_event_count].min=interrupt_time->min;
            time_alarm.alarm_event_list[time_alarm.alarm_event_count].second=interrupt_time->second;
        }
        time_alarm.alarm_event_recall_list[time_alarm.alarm_event_count]=interrupt_recall;
        ++time_alarm.alarm_event_count;
        return true;
    }
    return false;
}

void time_alarm_delete(point_time interrupt_time) {
    unsigned long find_index=0;
    for (;find_index<MAX_ALARM_EVENT_COUNT;++find_index) {
        if (NULL!=time_alarm.alarm_event_list[find_index].year) {
            if (time_alarm.alarm_event_list[find_index].year==interrupt_time->year &&
                time_alarm.alarm_event_list[find_index].month==interrupt_time->month &&
                time_alarm.alarm_event_list[find_index].day==interrupt_time->day &&
                time_alarm.alarm_event_list[find_index].hour==interrupt_time->hour &&
                time_alarm.alarm_event_list[find_index].min==interrupt_time->min &&
                time_alarm.alarm_event_list[find_index].second==interrupt_time->second) {
                memset(&time_alarm.alarm_event_list[find_index],0,sizeof(time));
                time_alarm.alarm_event_recall_list[find_index]=NULL;
                --time_alarm.alarm_event_count;
            }
        } else {
            if (time_alarm.alarm_event_list[find_index].week==interrupt_time->week &&
                time_alarm.alarm_event_list[find_index].hour==interrupt_time->hour &&
                time_alarm.alarm_event_list[find_index].min==interrupt_time->min &&
                time_alarm.alarm_event_list[find_index].second==interrupt_time->second) {
                memset(&time_alarm.alarm_event_list[find_index],0,sizeof(time));
                time_alarm.alarm_event_recall_list[find_index]=NULL;
                --time_alarm.alarm_event_count;
            }
        }
    }
}

void time_notice(point_time now_time) {
    unsigned long find_index=0;
    for (;find_index<MAX_ALARM_EVENT_COUNT;++find_index) {
        if (NULL!=time_alarm.alarm_event_list[find_index].year) {
            if (time_alarm.alarm_event_list[find_index].year==now_time->year &&
                time_alarm.alarm_event_list[find_index].month==now_time->month &&
                time_alarm.alarm_event_list[find_index].day==now_time->day &&
                time_alarm.alarm_event_list[find_index].hour==now_time->hour &&
                time_alarm.alarm_event_list[find_index].min==now_time->min &&
                time_alarm.alarm_event_list[find_index].second==now_time->second) {
                if (NULL!=time_alarm.alarm_event_recall_list[find_index])
                    time_alarm.alarm_event_recall_list[find_index](now_time);
                time_alarm_delete(now_time);
            }
        } else {
            if (time_alarm.alarm_event_list[find_index].week==now_time->week &&
                time_alarm.alarm_event_list[find_index].hour==now_time->hour &&
                time_alarm.alarm_event_list[find_index].min==now_time->min &&
                time_alarm.alarm_event_list[find_index].second==now_time->second) {
                now_time->year=NULL;
                if (NULL!=time_alarm.alarm_event_recall_list[find_index])
                    time_alarm.alarm_event_recall_list[find_index](now_time);
                time_alarm_delete(now_time);
            }
        }
    }
}
