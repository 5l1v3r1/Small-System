
#ifndef _LCD_API_H__
#define _LCD_API_H__

#include "system_type.h"

#include "lcd_color.h"

#define MAX_LINE_HORIZONTAL     15  //  ºáÏò
#define MAX_COLUMN_HORIZONTAL   40
#define MAX_LINE_LONGITUDINAL   20  //  ×ÝÏò
#define MAX_COLUMN_LONGITUDINAL 30

#define MAX_LINE   MAX_LINE_LONGITUDINAL
#define MAX_COLUMN MAX_COLUMN_HORIZONTAL

#define SCREEN_HORIZONTAL_PIXEL   240
#define SCREEN_LONGITUDINAL_PIXEL 320

#define DEFAULT_BACKGROUND_COLOR black
#define DEFAULT_FONT_COLOR       white

typedef enum {
    screen_direction=0,
} setting_flag;

void lcd_init(void);

void lcd_on(void);
void lcd_off(void);

bool lcd_display_state(void);

void lcd_setting(setting_flag in_setting_flag,int in_setting_data);

void lcd_background(color color_code);
void lcd_font(color color_code);

color lcd_read_point(int x,int y);

void lcd_draw_point(int x,int y,color color_code);
void lcd_draw_circle(int x,int y,int radius,color color_code);
void lcd_draw_line(int x_left_upper,int y1_left_upper,int x_right_lower,int y_right_lower,color color_code);
void lcd_draw_rectangle(int x_left_upper,int y_left_upper,int x_right_lower,int y_right_lower,color color_code,bool is_fill);

void lcd_cursor_set(int x,int y);
void lcd_print_line(const char* string);
void lcd_print(const char* string);
void lcd_print_crlf(void);
void lcd_print_clean(void);

typedef unsigned int lcd_location;

bool lcd_is_touch(void);
lcd_location lcd_touch_x(void);
lcd_location lcd_touch_y(void);

void lcd_calcu_middle_location(unsigned int x_left_upper,unsigned int y_left_upper,unsigned int x_right_lower,unsigned int y_right_lower,unsigned int* out_x,unsigned int* out_y);

#endif
