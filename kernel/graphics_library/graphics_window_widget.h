
#ifndef _GRAPHICS_WINDOW_WIDGET_H__
#define _GRAPHICS_WINDOW_WIDGET_H__

#include "graphics_api.h"
#include "graphics_window.h"
#include "graphics_widget.h"

#define MAX_WINDOW_COUNT             5
#define MAX_WINDOW_AND_MESSAGE_COUNT 6
//  为信息框留一个空间,搜寻空余窗口是以MAX_WINDOW_COUNT 来列举的..
#define MAX_WIDGET_COUNT             16

typedef struct {
    window_handle handle;
    window_name name;
    window_title title;
    window_background_color background_color;
    graphics_location x;
    graphics_location y;
    window_height height;
    window_width width;
    bool show_stat;
    bool show_title;
    window_notice_recall recall_event;
    unsigned int widget_count;
    window_widget widget_list[MAX_WIDGET_COUNT];
} window_data,*point_window_data;

typedef struct {
    unsigned int current_window_count;
    window_data  current_window_data[MAX_WINDOW_AND_MESSAGE_COUNT];
} window_list_;

#endif
