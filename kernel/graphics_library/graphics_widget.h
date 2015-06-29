
#ifndef _GRAPHICE_WIDGET_H__
#define _GRAPHICE_WIDGET_H__

#include "lcd_color.h"

#include "graphics_bitmap.h"
#include "graphics_window.h"

#define INVALID_WIDGET_HANDLE 0xFFFFFFFF

#define MAX_LENGTH_LABEL             256

typedef unsigned int widget_handle;
typedef unsigned int widget_location;
typedef unsigned int widget_size;

typedef enum {
    widget_no=0,
    widget_label,
    widget_progress,
    widget_picture,
    widget_text,
    widget_list,
    widget_combox,
    widget_button,
} widget_type;

typedef struct {
    window_handle parent_window;
    widget_handle widget_handle;
    widget_size widget_height;
    widget_size widget_width;
    widget_location x;
    widget_location y;
    color widget_background;
    color widget_foreground;
    widget_type widget_type;
    bool widget_lock;
    bool widget_visual;
    char widget_reserved;
    unsigned int widget_data_length;
    void* widget_data;
} window_widget,*point_window_widget;

widget_handle create_widget(window_handle window_index,point_window_widget widget_setting);
void delete_widget(window_handle window_index,widget_handle widget_index);
void draw_widget(window_handle window_index,widget_handle widget_index);

void set_label_multiline(window_handle window_index,widget_handle widget_index,bool is_multiline);
void set_label_caption(window_handle window_index,widget_handle widget_index,const char* string);

void set_progress_max_value(window_handle window_index,widget_handle widget_index,unsigned long max_value);
void set_progress_value(window_handle window_index,widget_handle widget_index,unsigned long current_value);

typedef void (*button_click_recall)(void);

void set_button_click_recall(window_handle window_index,widget_handle widget_index,button_click_recall recall_address);
void set_button_caption(window_handle window_index,widget_handle widget_index,const char* string);

void set_picture_click_recall(window_handle window_index,widget_handle widget_index,button_click_recall recall_address);
void set_picture_bitmap(window_handle window_index,widget_handle widget_index,bitmap_data bitmap);
void set_picture_empty(window_handle window_index,widget_handle widget_index);

bool draw_line(window_handle handle,widget_location x1,widget_location y1,widget_location x2,widget_location y2,color new_color);
bool draw_circle(window_handle handle,widget_location x,widget_location y,widget_size r,color new_color);

void set_widget_background_color(window_handle window_index,widget_handle widget_index,color new_color);
void set_widget_foreground_color(window_handle window_index,widget_handle widget_index,color new_color);

#endif
