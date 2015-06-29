
#ifndef _SYSTEM_DESKTOP_UI_H__
#define _SYSTEM_DESKTOP_UI_H__

#include "graphics_window.h"

void system_desktop_ui_init(window_handle init_window_handle);
void system_desktop_ui_notice(window_handle recall_window,notice_flag in_notice_flag,long parameter1,long parameter2);

void system_desktop_title(const char* new_title);
void system_desktop_fresh_app_list(void);

window_handle system_desktop_get_window_handle(void);

#endif
