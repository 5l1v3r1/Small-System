
#ifndef _GRAPHICS_WINDOW_H__
#define _GRAPHICS_WINDOW_H__

#include "system_type.h"

#define WINDOW_TITLE_LENGTH 26
typedef const char window_title[WINDOW_TITLE_LENGTH];
#define WINDOW_NAME_LENGTH  16
typedef const char window_name[WINDOW_NAME_LENGTH];
typedef unsigned int window_handle;
typedef color window_background_color;
typedef unsigned long window_height;
typedef unsigned long window_width;
typedef enum {
    notice_click=0,
    notice_close,
} notice_flag;
typedef void (*window_notice_recall)(window_handle recall_window,notice_flag in_notice_flag,long parameter1,long parameter2);

#define INVALID_WINDOW_HANDLE 0xFFFFFFFF

#define WINDOW_DEFUALT_BACKGROUND_COLOR 0xCE79

window_handle create_window(window_name name,window_title title,bool is_show_title,window_notice_recall notice_event);
bool show_window(window_handle handle);
void refresh_window(window_handle handle);
void close_window(window_handle handle);
void set_window_show_title(window_handle handle,bool is_show);
void set_window_title(window_handle handle,const char* new_title);
void set_window_background_color(window_handle handle,color new_color);

window_handle get_window_front(void);
void set_window_front(window_handle in_window_handle);

void notice_window(window_handle handle,notice_flag in_notice_flag,long parameter1,long parameter2);

typedef enum {
    messagebox_tips=0,
    messagebox_question,
    messagebox_warning,
} messagebox_style;

typedef enum {
    no_click=0,
    click_ok,
    click_cancel,
    click_ignore,
    click_exit,
} messagebox_return;

messagebox_return messagebox(window_height call_messagebox_window,const char* box_caption,const char* box_title,messagebox_style show_style);

void window_test(void);

#endif
