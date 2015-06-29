
#ifndef _SYSTEM_DESKTOP_APPLICATION_EXECUTE_H__
#define _SYSTEM_DESKTOP_APPLICATION_EXECUTE_H__

#include "system_type.h"

typedef enum {
    no_notice=0,
} notice_type;

bool system_desktop_application_execute_init(const char* app_root_path,const char* app_name);

void system_desktop_application_execute_widget_notice(notice_type in_notice_type);

#endif
