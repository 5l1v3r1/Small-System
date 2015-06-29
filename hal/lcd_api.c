
#include <string.h>

#include "system_type.h"

#include "lcd_api.h"

#include "lcd_device//lcd_device_api.h"
#include "lcd_device//lcd_device_touch.h"

static void lcd_touch_change_directon(bool is_longitudinal) {
    if (is_longitudinal)
        tp_dev.touchtype=0;
    else
        tp_dev.touchtype=1;
}

void lcd_init(void) {
    LCD_Init();
    TP_Init();
    lcd_background(black);
    lcd_font(white);
    lcd_setting(screen_direction,0);
    lcd_touch_change_directon(true);
    lcd_on();
}

static bool lcd_diplay_state=true;

void lcd_on(void) {
//    LCD_DisplayOn();
    lcd_diplay_state=true;
}
void lcd_off(void) {
//    LCD_DisplayOff();
    lcd_draw_rectangle(0,0,SCREEN_HORIZONTAL_PIXEL,SCREEN_LONGITUDINAL_PIXEL,BLACK,true);
    lcd_diplay_state=false;
}

bool lcd_display_state(void) {
    return lcd_diplay_state;
}

static bool screen_direction_flag=false;

void lcd_setting(setting_flag in_setting_flag,int in_setting_data) {
    if (!lcd_diplay_state) return;
    switch(in_setting_flag) {
        case screen_direction:
            if (in_setting_data) {
                LCD_Display_Dir(1);
                lcd_touch_change_directon(false);
                screen_direction_flag=true;
            } else {
                LCD_Display_Dir(0);
                lcd_touch_change_directon(true);
                screen_direction_flag=false;
            }
            break;
    }        
}

static color background_color_code=black;

void lcd_background(color color_code) {
    background_color_code=color_code;
	BACK_COLOR=color_code;
    LCD_Clear(color_code);
}
static color font_color_code =white;
void lcd_font(color color_code) {
	POINT_COLOR=color_code;
}

color lcd_read_point(int x,int y) {
    return LCD_ReadPoint(x,y);
}

void lcd_draw_point(int x,int y,color color_code) {
    if (!lcd_diplay_state) return;
    LCD_Fast_DrawPoint(x,y,color_code);
}
void lcd_draw_circle(int x,int y,int radius,color color_code) {
    color old_color=font_color_code;
    
    if (!lcd_diplay_state) return;
    lcd_font(color_code);
    Draw_Circle(x,y,radius);
    lcd_font(old_color);
}
void lcd_draw_line(int x_left_upper,int y_left_upper,int x_right_lower,int y_right_lower,color color_code) {
    color old_color=font_color_code;
    
    if (!lcd_diplay_state) return;
    lcd_font(color_code);
    LCD_DrawLine(x_left_upper,y_left_upper,x_right_lower,y_right_lower);
    lcd_font(old_color);
}
void lcd_draw_rectangle(int x_left_upper,int y_left_upper,int x_right_lower,int y_right_lower,color color_code,bool is_fill) {
    color old_color=font_color_code;
    
    if (!lcd_diplay_state) return;
    lcd_font(color_code);
    if (is_fill)
        LCD_Fill(x_left_upper,y_left_upper,x_right_lower,y_right_lower,color_code);
    else
        LCD_DrawRectangle(x_left_upper,y_left_upper,x_right_lower,y_right_lower);
    lcd_font(old_color);
}
void lcd_cursor_set(int x,int y) {
    if (!lcd_diplay_state) return;
    LCD_SetCursor(x,y);
}

#define MAX_LINE_HORIZONTAL_PIXEL   345
#define MAX_LINE_LONGITUDINAL_PIXEL 240
#define FONT_PIXEL 16

static char print_buffer[MAX_LINE][MAX_COLUMN+1]={' '};
static int print_current_line=0;

#define LINE_SPACE "                                      "

void lcd_print_clean(void) {
    if (!lcd_diplay_state) return;
    LCD_Clear(background_color_code);
    memset(print_buffer,0,sizeof(print_buffer));
    print_current_line=0;
}

void lcd_print_line(const char* string) {
    char fill_string[MAX_COLUMN];
    int print_index=0;
    
    if (!lcd_diplay_state) return;
    if (!((screen_direction_flag && strlen(string)<MAX_LINE_HORIZONTAL) || (!screen_direction_flag && strlen(string)<MAX_COLUMN_LONGITUDINAL)))
        return;
    memset(fill_string,0,sizeof(fill_string));
    memcpy(fill_string,string,strlen(string));
    
    if (screen_direction_flag) {
        if (print_current_line<MAX_LINE_HORIZONTAL) {
            memcpy(print_buffer[print_current_line],fill_string,strlen(fill_string));
            LCD_ShowString(0,print_current_line*FONT_PIXEL,MAX_LINE_HORIZONTAL_PIXEL,FONT_PIXEL,FONT_PIXEL,string);
            ++print_current_line;
        } else {
            memcpy(print_buffer,print_buffer[1],MAX_COLUMN*(MAX_LINE_HORIZONTAL-1));
            memset(print_buffer[MAX_LINE_HORIZONTAL-1],' ',MAX_COLUMN);
            memcpy(print_buffer[MAX_LINE_HORIZONTAL-1],fill_string,strlen(fill_string)-1);
            for (;print_index<MAX_LINE_HORIZONTAL;++print_index)
                LCD_ShowString(0,print_index*FONT_PIXEL,MAX_LINE_HORIZONTAL_PIXEL,FONT_PIXEL,FONT_PIXEL,print_buffer[print_index]);
        }
    } else {
        if (print_current_line<MAX_LINE_LONGITUDINAL) {
            memcpy(print_buffer[print_current_line],fill_string,strlen(fill_string));
            LCD_ShowString(0,print_current_line*FONT_PIXEL,MAX_LINE_LONGITUDINAL_PIXEL,FONT_PIXEL,FONT_PIXEL,string);
            ++print_current_line;
        } else {
            memcpy(print_buffer,print_buffer[1],MAX_COLUMN*(MAX_LINE_LONGITUDINAL-1));
            memset(print_buffer[MAX_LINE_LONGITUDINAL-1],0,MAX_COLUMN);
            memcpy(print_buffer[MAX_LINE_LONGITUDINAL-1],fill_string,strlen(fill_string));
            for (;print_index<MAX_LINE_LONGITUDINAL;++print_index)
                LCD_ShowString(0,print_index*FONT_PIXEL,MAX_LINE_LONGITUDINAL_PIXEL,FONT_PIXEL,FONT_PIXEL,print_buffer[print_index]);
        }
    }
}

void lcd_print(const char* string) {
    long string_length=strlen(string);
    char string_line[MAX_COLUMN];
    int  print_control=0;
    int  print_crlf_index=1;
    
    if (!lcd_diplay_state) return;
    if (screen_direction_flag) {
        for (;string_length>0;string_length-=(MAX_COLUMN_HORIZONTAL-1),++print_crlf_index,++print_control) {
            memcpy(string_line,(const void*)((long)string+print_control*(MAX_COLUMN_HORIZONTAL-1)),MAX_COLUMN_HORIZONTAL-1);
            lcd_print_line(string_line);
            memset(string_line,0,MAX_COLUMN_HORIZONTAL);
        }
    } else {
        for (;string_length>0;string_length-=(MAX_COLUMN_LONGITUDINAL-1),++print_crlf_index,++print_control) {
            memcpy(string_line,(const void*)((long)string+print_control*(MAX_COLUMN_LONGITUDINAL-1)),MAX_COLUMN_LONGITUDINAL-1);
            lcd_print_line(string_line);
            memset(string_line,0,MAX_COLUMN_LONGITUDINAL);
        }
    }
}

void lcd_print_crlf(void) {
    if (!lcd_diplay_state) return;
    lcd_print_line("");
}

bool lcd_is_touch(void) {
    tp_dev.scan(0);
    return (tp_dev.sta & TP_PRES_DOWN)?true:false;
}

lcd_location lcd_touch_x(void) {
    tp_dev.scan(0);
    return tp_dev.x;
}

lcd_location lcd_touch_y(void) {
    tp_dev.scan(0);
    return tp_dev.y;
}

void lcd_calcu_middle_location(unsigned int x_left_upper,unsigned int y_left_upper,unsigned int x_right_lower,unsigned int y_right_lower,unsigned int* out_x,unsigned int* out_y) {
    *out_x=(x_right_lower-x_left_upper)/2+x_left_upper;
    *out_y=(y_right_lower-y_left_upper)/2+y_left_upper;
}
