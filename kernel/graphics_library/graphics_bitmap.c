
#include "system_type.h"

#include <string.h>

#include "lcd_api.h"

#include "graphics_api.h"
#include "graphics_bitmap.h"

/*
static color graphics_rgb_555_to_565(color conver_color) {
    char c1=(char)conver_color>>8,c2=(char)conver_color;
    char c3,c4;
    color conver_new_color;
    
    c3=c1<<1;
    c3|=c2>>7;

    c4=c2<<1;
    c4&=0xc0;
    c4|=c2&0x1f;
    if(c2&0x20) c4+=0x20;
    
    conver_new_color=(c3<<8)+c4;
    return conver_new_color;
}*/

static color graphics_24bit_to_16bit(color conver_color) {
    char red=(conver_color>>19)& 0x1F;
    char green=(conver_color>>10)& 0x3F;
    char blue=(conver_color>>3)& 0x1F;
    
    return ((red<<11)+(green<<5)+blue);
}

static void graphics_draw_bitmap_2(bitmap_data bitmap_data,graphics_location draw_point_x,graphics_location draw_point_y) {
    
    //  WARNING! 这个函数还有错误..
    
    unsigned long draw_index=0;
    unsigned long draw_control_x=0;
    unsigned long draw_control_y=*(unsigned long*)(bitmap_data+18);
    unsigned long picture_width=*(unsigned long*)(bitmap_data+22);
    unsigned long image_scan_point=(unsigned long)bitmap_data+*(unsigned long*)(bitmap_data+10);
    
    while (draw_control_y>0) {
        lcd_draw_point(draw_control_x+draw_point_x,draw_control_y+draw_point_y,((*(color*)(image_scan_point+draw_index) & 0x80)?white:black));
        ++draw_control_x;
        if (draw_control_x==picture_width) {
            draw_control_x=0;
            --draw_point_y;
            if (draw_point_y==0)
                return;
        }
        lcd_draw_point(draw_control_x+draw_point_x,draw_control_y+draw_point_y,((*(color*)(image_scan_point+draw_index) & 0x40)?white:black));
        ++draw_control_x;
        if (draw_control_x==picture_width) {
            draw_control_x=0;
            --draw_point_y;
            if (draw_point_y==0)
                return;
        }
        lcd_draw_point(draw_control_x+draw_point_x,draw_control_y+draw_point_y,((*(color*)(image_scan_point+draw_index) & 0x20)?white:black));
        ++draw_control_x;
        if (draw_control_x==picture_width) {
            draw_control_x=0;
            --draw_point_y;
            if (draw_point_y==0)
                return;
        }
        lcd_draw_point(draw_control_x+draw_point_x,draw_control_y+draw_point_y,((*(color*)(image_scan_point+draw_index) & 0x10)?white:black));
        ++draw_control_x;
        if (draw_control_x==picture_width) {
            draw_control_x=0;
            --draw_point_y;
            if (draw_point_y==0)
                return;
        }
        lcd_draw_point(draw_control_x+draw_point_x,draw_control_y+draw_point_y,((*(color*)(image_scan_point+draw_index) & 0x8)?white:black));
        ++draw_control_x;
        if (draw_control_x==picture_width) {
            draw_control_x=0;
            --draw_point_y;
            if (draw_point_y==0)
                return;
        }
        lcd_draw_point(draw_control_x+draw_point_x,draw_control_y+draw_point_y,((*(color*)(image_scan_point+draw_index) & 0x4)?white:black));
        ++draw_control_x;
        if (draw_control_x==picture_width) {
            draw_control_x=0;
            --draw_point_y;
            if (draw_point_y==0)
                return;
        }
        lcd_draw_point(draw_control_x+draw_point_x,draw_control_y+draw_point_y,((*(color*)(image_scan_point+draw_index) & 0x2)?white:black));
        ++draw_control_x;
        if (draw_control_x==picture_width) {
            draw_control_x=0;
            --draw_point_y;
            if (draw_point_y==0)
                return;
        }
        lcd_draw_point(draw_control_x+draw_point_x,draw_control_y+draw_point_y,((*(color*)(image_scan_point+draw_index) & 0x1)?white:black));
        ++draw_control_x;
        if (draw_control_x==picture_width) {
            draw_control_x=0;
            --draw_point_y;
            if (draw_point_y==0)
                return;
        }
        
        ++draw_index;
    }
}
static void graphics_draw_bitmap_4(bitmap_data bitmap_data,graphics_location draw_point_x,graphics_location draw_point_y) {
    unsigned long draw_index=0;
    unsigned long draw_control_x=0;
    unsigned long draw_control_y=*(unsigned long*)(bitmap_data+18)+draw_point_y;
    unsigned long picture_width=*(unsigned long*)(bitmap_data+22);
    unsigned long image_scan_point=(unsigned long)bitmap_data+*(unsigned long*)(bitmap_data+10);
    unsigned long scan_point=0;
    color output_color=0;
    rgb_quad quad_list[16]={0};
    
    memcpy(&quad_list,bitmap_data+BITMAP_HEADER_LENGTH+BITMAP_INFORMATION_LENGTH,RBG_QUAD_LENGTH*16);
    
    for (;draw_control_y>0;--draw_control_y,draw_index+=2) {
        for (draw_control_x=0;draw_control_x<picture_width;draw_control_x+=2,++draw_index) {
            scan_point=(*(char*)(image_scan_point+draw_index) & 0xF0)>>4;
            output_color=((quad_list[scan_point].red & 0x1F)<<11)+((quad_list[scan_point].green & 0x3F)<<5)+(quad_list[scan_point].blue & 0x1F);
            lcd_draw_point(draw_control_x+draw_point_x,draw_control_y+draw_point_y,output_color);

            scan_point=*(char*)(image_scan_point+draw_index) & 0xF;
            output_color=((quad_list[scan_point].red & 0x1F)<<11)+((quad_list[scan_point].green & 0x3F)<<5)+(quad_list[scan_point].blue & 0x1F);
            lcd_draw_point(draw_control_x+draw_point_x+1,draw_control_y+draw_point_y,output_color);
        }
    }
}

static void graphics_draw_bitmap_8(bitmap_data bitmap_data,graphics_location draw_point_x,graphics_location draw_point_y) {
    unsigned long draw_index=0;
    unsigned long draw_control_x=0;
    unsigned long draw_control_y=*(unsigned long*)(bitmap_data+18);
    unsigned long picture_width=*(unsigned long*)(bitmap_data+22);
    unsigned long image_scan_point=(unsigned long)bitmap_data+*(unsigned long*)(bitmap_data+10);
    
    for (;draw_control_y>0;--draw_control_y) {
        for (draw_control_x=0;draw_control_x<picture_width;++draw_control_x,++draw_index) {
            lcd_draw_point(draw_control_x+draw_point_x,draw_control_y+draw_point_y,*(color*)(image_scan_point+draw_index));
        }
    }
}

static void graphics_draw_bitmap_16(bitmap_data bitmap_data,graphics_location draw_point_x,graphics_location draw_point_y) {
}

static void graphics_draw_bitmap_24(bitmap_data bitmap_data,graphics_location draw_point_x,graphics_location draw_point_y) {
    unsigned long draw_index=0;
    unsigned long draw_control_x=0;
    unsigned long draw_control_y=*(unsigned long*)(bitmap_data+18);
    unsigned long picture_width=*(unsigned long*)(bitmap_data+22);
    unsigned long image_scan_point=(unsigned long)bitmap_data+*(unsigned long*)(bitmap_data+10);
    
    for (;draw_control_y>0;--draw_control_y) {
        for (draw_control_x=0;draw_control_x<picture_width;++draw_control_x,draw_index+=3) {
            lcd_draw_point(draw_control_x+draw_point_x,draw_control_y+draw_point_y,graphics_24bit_to_16bit(*(color*)(image_scan_point+draw_index)));
        }
    }
}

void graphics_draw_bitmap(bitmap_data bitmap_data,graphics_location draw_point_x,graphics_location draw_point_y) {
    unsigned long bitmap_bit=*(unsigned long*)(bitmap_data+28);
    
    switch (bitmap_bit) {
        case 1:
            graphics_draw_bitmap_2(bitmap_data,draw_point_x,draw_point_y);
            break;
        case 4:
            graphics_draw_bitmap_4(bitmap_data,draw_point_x,draw_point_y);
            break;
        case 8:
            graphics_draw_bitmap_8(bitmap_data,draw_point_x,draw_point_y);
            break;
        case 16:
            graphics_draw_bitmap_16(bitmap_data,draw_point_x,draw_point_y);
            break;
        case 24:
            graphics_draw_bitmap_24(bitmap_data,draw_point_x,draw_point_y);
    }
}

void graphics_get_bitmap(bitmap_data bitmap_data,unsigned long* bitmap_size,unsigned long* bitmap_height,unsigned long* bitmap_width) {
    if (NULL!=bitmap_size)
        *bitmap_size=*(unsigned long*)(bitmap_data+2);
    if (NULL!=bitmap_height)
        *bitmap_height=*(unsigned long*)(bitmap_data+18);
    if (NULL!=bitmap_width)
        *bitmap_width=*(unsigned long*)(bitmap_data+22);
}
