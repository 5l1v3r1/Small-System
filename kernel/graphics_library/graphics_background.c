
#include "system_type.h"

#include <string.h>

#include "lcd_api.h"

#include "..//file_poor//file_api.h"

#include "graphics_api.h"
#include "graphics_bitmap.h"
#include "graphics_background.h"

/*

由于内存过小的问题,背景只用黑色填充

color graphics_background[SCREEN_LONGITUDINAL_PIXEL][SCREEN_HORIZONTAL_PIXEL]={DEFAULT_BACKGROUND_COLOR};
unsigned long graphics_background_height=0;
unsigned long graphics_background_width=0;

static void graphics_background_set_color(graphics_location x,graphics_location y,color set_color) {
    if (x<SCREEN_HORIZONTAL_PIXEL && y<SCREEN_LONGITUDINAL_PIXEL)
        graphics_background[y][x]=set_color;
}

static color graphics_background_get_color(graphics_location x,graphics_location y) {
    if (x<SCREEN_HORIZONTAL_PIXEL && y<SCREEN_LONGITUDINAL_PIXEL)
        return graphics_background[y][x];
    return black;
}

void graphics_background_clean(void) {
    graphics_location clean_index_x=0,clean_index_y=0;
    
    for (;clean_index_y<graphics_background_height;++clean_index_y) {
        for (clean_index_x=0;clean_index_x<graphics_background_width;++clean_index_x) {
            graphics_background_set_color(clean_index_x,clean_index_y,DEFAULT_BACKGROUND_COLOR);
            lcd_draw_point(clean_index_x,clean_index_y,DEFAULT_BACKGROUND_COLOR);
        }
    }
    graphics_background_height=0;
    graphics_background_width=0;
}

void graphics_background_init(void) {
    graphics_background_clean();
}

bool graphics_background_load(const char* file_path) {
    unsigned long bitmap_height=0;
    unsigned long bitmap_width=0;
    unsigned long read_index_x=0;
    unsigned long read_index_y=0;
    
    graphics_get_bitmap(test_bitmap,NULL,&bitmap_height,&bitmap_width);
    graphics_draw_bitmap(test_bitmap,0,0);
    
    for (;read_index_y<bitmap_height;++read_index_y)
        for (read_index_x=0;read_index_x<bitmap_width;++read_index_x)
            graphics_background_set_color(read_index_x,read_index_y,lcd_read_point(read_index_x,read_index_y));
    
    graphics_background_height=bitmap_height;
    graphics_background_width=bitmap_width;
    return true;
}

void graphics_draw_background(void) {
    unsigned long draw_index_x=0;
    unsigned long draw_index_y=0;
    
    for (;draw_index_y<graphics_background_height;++draw_index_y) {
        for (draw_index_x=0;draw_index_x<graphics_background_width;++draw_index_x) {
            lcd_draw_point(draw_index_x,draw_index_y,graphics_background[draw_index_y][draw_index_x]);
        }
    }
}*/

void graphics_background_clean(void) {
    graphics_location clean_index_x=0,clean_index_y=0;
    
    for (;clean_index_y<SCREEN_LONGITUDINAL_PIXEL;++clean_index_y) {
        for (clean_index_x=0;clean_index_x<SCREEN_HORIZONTAL_PIXEL;++clean_index_x) {
            lcd_draw_point(clean_index_x,clean_index_y,DEFAULT_BACKGROUND_COLOR);
        }
    }
}

void graphics_background_init(void) {
    graphics_background_clean();
}

bool graphics_background_load(const char* file_path) {
    graphics_background_clean();
    return true;
}

void graphics_draw_background(void) {
    graphics_background_clean();
}
