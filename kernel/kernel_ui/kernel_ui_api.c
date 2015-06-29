
#include <string.h>

#include "led_api.h"
#include "lcd_api.h"

#include "kernel_ui_api.h"
#include "kernel_logo.h"

#include "graphics_library//graphics_api.h"

#define DEFAULT_COLOR_BACKGROUND black
#define MAX_KERNEL_LOAD_RATE       100
#define PROGRESS_HEIGHT             10
#define DISTANCE_PROGRESS_TO_LOGO   40

unsigned int load_rate=0;
window_handle kernel_load_window=INVALID_WINDOW_HANDLE;
widget_handle progress_load_rate_handle=INVALID_WIDGET_HANDLE;

void kernel_load_enter(void) {
    graphics_location draw_system_logo_x=0;
    graphics_location draw_system_logo_y=0;
    unsigned long draw_system_logo_height=0;
    unsigned long draw_system_logo_width=0;
    window_widget picture_kernel_logo;
    widget_handle picture_kernel_logo_handle=INVALID_WIDGET_HANDLE;
    window_widget progress_load_rate;
    
    kernel_load_window=create_window("kernel load","kernel load",false,NULL);
    set_window_background_color(kernel_load_window,DEFAULT_COLOR_BACKGROUND);
    show_window(kernel_load_window);
    lcd_calcu_middle_location(0,0,SCREEN_HORIZONTAL_PIXEL,SCREEN_LONGITUDINAL_PIXEL,&draw_system_logo_x,&draw_system_logo_y);
    graphics_get_bitmap(kernel_logo,NULL,&draw_system_logo_height,&draw_system_logo_width);
    draw_system_logo_x-=draw_system_logo_width/2;
    draw_system_logo_y-=draw_system_logo_height/2;
    picture_kernel_logo.widget_type=widget_picture;
    picture_kernel_logo.x=draw_system_logo_x;
    picture_kernel_logo.y=draw_system_logo_y;
    picture_kernel_logo_handle=create_widget(kernel_load_window,&picture_kernel_logo);
    set_picture_bitmap(kernel_load_window,picture_kernel_logo_handle,kernel_logo);
    draw_widget(kernel_load_window,picture_kernel_logo_handle);
    progress_load_rate.widget_type=widget_progress;
    progress_load_rate.widget_height=PROGRESS_HEIGHT;
    progress_load_rate.widget_width=draw_system_logo_width;
    progress_load_rate.x=draw_system_logo_x;
    progress_load_rate.y=draw_system_logo_y+draw_system_logo_height+DISTANCE_PROGRESS_TO_LOGO;
    progress_load_rate_handle=create_widget(kernel_load_window,&progress_load_rate);
    set_progress_max_value(kernel_load_window,progress_load_rate_handle,MAX_KERNEL_LOAD_RATE);
    set_progress_value(kernel_load_window,progress_load_rate_handle,0);
    draw_widget(kernel_load_window,progress_load_rate_handle);
}

void kernel_load_exit(void) {
    if (INVALID_WINDOW_HANDLE!=kernel_load_window) {
        close_window(kernel_load_window);
        kernel_load_window=INVALID_WINDOW_HANDLE;
        progress_load_rate_handle=INVALID_WIDGET_HANDLE;
        load_rate=100;
    }
}

void kernel_set_load_rate(unsigned int rate) {
    if (rate>load_rate) {
        set_progress_value(kernel_load_window,progress_load_rate_handle,rate);
        if (MAX_KERNEL_LOAD_RATE==rate) {
            close_window(kernel_load_window);
            return;
        }
        load_rate=rate;
    }
}

void kernel_err_report(const char* err_report_title,const char* err_report_detail,const char* error_fix_advise) {
/*
     _______________________________
    |                               |
    | REPORT TITLE(Line3-4)         |
    |                               |
    | REPORT DETAIL(Line6-8)        |
    |                               |
    | ERROR  FIX  ADVISE(Line10-12) |
    |                               |
    |_______________________________|
    
*/
#define REPORT_TITLE_LINE      3
#define REPORT_DETAIL_LINE     6
#define ERROR_FIX_ADVISE_LINE 10
    
    long err_report_data_length=strlen(err_report_title);
    char err_report_data_line[MAX_COLUMN_LONGITUDINAL];
    int  error_report_print_control=0;
    int  print_crlf_index=1;
    
    lcd_background(blue);
    lcd_print_clean();
    
    for (;print_crlf_index<REPORT_TITLE_LINE;++print_crlf_index)
        lcd_print_crlf();
    for (;err_report_data_length>0;err_report_data_length-=(MAX_COLUMN_LONGITUDINAL-1),++print_crlf_index,++error_report_print_control) {
        memcpy(err_report_data_line,(const void*)((long)err_report_title+error_report_print_control*(MAX_COLUMN_LONGITUDINAL-1)),MAX_COLUMN_LONGITUDINAL-1);
        lcd_print_line(err_report_data_line);
        memset(err_report_data_line,0,MAX_COLUMN_LONGITUDINAL);
    }
    
    for (;print_crlf_index<REPORT_DETAIL_LINE;++print_crlf_index)
        lcd_print_crlf();
    for (err_report_data_length=strlen(err_report_detail),error_report_print_control=0;err_report_data_length>0;err_report_data_length-=(MAX_COLUMN_LONGITUDINAL-1),++print_crlf_index,++error_report_print_control) {
        memcpy(err_report_data_line,(const void*)((long)err_report_detail+error_report_print_control*(MAX_COLUMN_LONGITUDINAL-1)),MAX_COLUMN_LONGITUDINAL-1);
        lcd_print_line(err_report_data_line);
        memset(err_report_data_line,0,MAX_COLUMN_LONGITUDINAL);
    }
    
    for (;print_crlf_index<ERROR_FIX_ADVISE_LINE;++print_crlf_index)
        lcd_print_crlf();
    for (err_report_data_length=strlen(error_fix_advise),error_report_print_control=0;err_report_data_length>0;err_report_data_length-=(MAX_COLUMN_LONGITUDINAL-1),++print_crlf_index,++error_report_print_control) {
        memcpy(err_report_data_line,(const void*)((long)error_fix_advise+error_report_print_control*(MAX_COLUMN_LONGITUDINAL-1)),MAX_COLUMN_LONGITUDINAL-1);
        lcd_print_line(err_report_data_line);
        memset(err_report_data_line,0,MAX_COLUMN_LONGITUDINAL);
    }
}
