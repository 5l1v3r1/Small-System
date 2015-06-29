
#include <stdlib.h>
#include <string.h>

#include "lcd_api.h"
#include "malloc.h"

#include "graphics_bitmap.h"
#include "graphics_font.h"
#include "graphics_widget.h"
#include "graphics_window_widget.h"

extern window_list_ window_list;

//对于 widget_data_length 的取值,有:

typedef enum {
    data_length_label=256+4,  //  字符串长度和是否为多行输出:1.is_output_multiline 2.output_data
    data_length_progress=8,   //  三个long 型数据长度,分别表示:1.max_value 2.current_value
    data_length_picture=12+4, //  三个long 型数据长度,分别表示:1.height 2.width 3.point_bitmap_buffer 4.click_recall
    data_length_text=256+4,   //  字符串长度和是否为多行输入:1.is_input_multiline 2.input_data
    data_length_list=256,     //  字符串长度
    data_length_combox=256,   //  字符串长度
    data_length_button=16+4,  //  按钮回调事件地址和按钮上面的显示字符:1.click_recall 2.button_caption
} widget_data_length_enum;

widget_handle create_widget(window_handle window_index,point_window_widget widget_setting) {
    unsigned int find_index=NULL;
    char* new_buffer=NULL;
    point_window_widget new_widget_setting=NULL;
    
    if (INVALID_WINDOW_HANDLE!=window_index && NULL!=widget_setting) {
        for (;find_index<MAX_WIDGET_COUNT;++find_index)
            if (INVALID_WIDGET_HANDLE==window_list.current_window_data[window_index].widget_list[find_index].widget_handle)
                break;
        if (MAX_WIDGET_COUNT==find_index)
            return INVALID_WIDGET_HANDLE;
        
        new_widget_setting=&window_list.current_window_data[window_index].widget_list[find_index];
        memcpy(new_widget_setting,widget_setting,sizeof(window_widget));
        switch (widget_setting->widget_type) {
            case widget_label:
                new_buffer=(char*)mymalloc(SRAMIN,data_length_label);
                if (NULL==new_buffer)
                    return INVALID_WIDGET_HANDLE;
                memset(new_buffer,0,data_length_label);
                new_widget_setting->widget_data_length=data_length_label;
                break;
            case widget_progress:
                new_buffer=(char*)mymalloc(SRAMIN,data_length_progress);
                if (NULL==new_buffer)
                    return INVALID_WIDGET_HANDLE;
                memset(new_buffer,0,data_length_progress);
                new_widget_setting->widget_data_length=data_length_progress;
                break;
            case widget_picture:
                new_buffer=(char*)mymalloc(SRAMIN,data_length_picture);
                if (NULL==new_buffer)
                    return INVALID_WIDGET_HANDLE;
                memset(new_buffer,0,data_length_picture);
                new_widget_setting->widget_data_length=data_length_picture;
                break;
            case widget_text:
                new_buffer=(char*)mymalloc(SRAMIN,data_length_text);
                if (NULL==new_buffer)
                    return INVALID_WIDGET_HANDLE;
                memset(new_buffer,0,data_length_text);
                new_widget_setting->widget_data_length=data_length_text;
                break;
            case widget_list:
                new_buffer=(char*)mymalloc(SRAMIN,data_length_list);
                if (NULL==new_buffer)
                    return INVALID_WIDGET_HANDLE;
                memset(new_buffer,0,data_length_list);
                new_widget_setting->widget_data_length=data_length_list;
                break;
            case widget_combox:
                new_buffer=(char*)mymalloc(SRAMIN,data_length_combox);
                if (NULL==new_buffer) 
                    return INVALID_WIDGET_HANDLE;
                memset(new_buffer,0,data_length_combox);
                new_widget_setting->widget_data_length=data_length_combox;
                break;
            case widget_button:
                new_buffer=(char*)mymalloc(SRAMIN,data_length_button);
                if (NULL==new_buffer) 
                    return INVALID_WIDGET_HANDLE;
                memset(new_buffer,0,data_length_button);
                new_widget_setting->widget_data_length=data_length_button;
                break;
            default:
                return INVALID_WIDGET_HANDLE;
        }
        new_widget_setting->parent_window=window_index;
        new_widget_setting->widget_handle=find_index;
        new_widget_setting->widget_reserved=0;
        new_widget_setting->widget_data=new_buffer;
        new_widget_setting->widget_background=WINDOW_DEFUALT_BACKGROUND_COLOR;
        new_widget_setting->widget_foreground=black;
        new_widget_setting->widget_lock=false;
        new_widget_setting->widget_visual=false;
        
        if (widget_progress==new_widget_setting->widget_type) {
            new_widget_setting->widget_foreground=blue;
            new_widget_setting->widget_background=white;
        }
        
        ++window_list.current_window_data[window_index].widget_count;
        return find_index;
    }
    
    return INVALID_WIDGET_HANDLE;
}    

void delete_widget(window_handle window_index,widget_handle widget_index) {
    unsigned int find_index=0;
    
    if (INVALID_WINDOW_HANDLE!=window_index && INVALID_WIDGET_HANDLE!=widget_index) {
        if (widget_index>=window_list.current_window_data[window_index].widget_count)
            return;
        for(;find_index<MAX_WIDGET_COUNT;++find_index) {
            if (find_index==widget_index) {
                mymalloc(SRAMIN,(u32)window_list.current_window_data[window_index].widget_list[widget_index].widget_data);
                memset(&window_list.current_window_data[window_index].widget_list[widget_index],0,sizeof(window_widget));
                --window_list.current_window_data[window_index].widget_count;
                refresh_window(window_index);
                return;
            }
        }
    }
}

/*
         ___________________________
        |Title  [Blue Color]     - x|
        |---------------------------|
        |                           |
        |          label..          |
        |                           |
        |___________________________|

*/

#define DISTANCE_TITLE_TO_LEFT  1
#define DISTANCE_TITLE_FILL    20

static void draw_label(point_window_data window_style,point_window_widget widget_style) {
    unsigned long is_output_multiline=0;
    unsigned long column_height=0;
    unsigned long line_width=0;
    unsigned long a_column_line=0;
    unsigned long a_line_alphabet=0;
    char output_string[SCREEN_HORIZONTAL_PIXEL/FONT_WIDTH]={0};
    unsigned long output_index=0;

    if (NULL==widget_style->widget_data) return;

    is_output_multiline=*(unsigned long*)widget_style->widget_data;
    if (is_output_multiline) {
        line_width=((window_style->width-widget_style->x)<=widget_style->widget_width)?window_style->width-widget_style->x:widget_style->widget_width;
        a_line_alphabet=line_width/FONT_WIDTH;
        column_height=((long)(window_style->height-widget_style->y)<=0)?0:(window_style->height-widget_style->y);
        if (!column_height) return;
        column_height=(column_height<widget_style->widget_height)?column_height:widget_style->widget_height;
        a_column_line=column_height/FONT_HEIGHT;
        
        lcd_draw_rectangle(window_style->x+widget_style->x+DISTANCE_TITLE_TO_LEFT,window_style->y+widget_style->y+DISTANCE_TITLE_FILL,
                           window_style->x+widget_style->x+line_width+DISTANCE_TITLE_TO_LEFT,window_style->y+widget_style->y+column_height+DISTANCE_TITLE_FILL,widget_style->widget_background,true);
            
        for (;a_column_line!=0;--a_column_line,++output_index) {
            memcpy(output_string,(const void*)((unsigned long)widget_style->widget_data+output_index*a_line_alphabet),a_line_alphabet);
            graphics_draw_font_string(output_string,window_style->x+widget_style->x,window_style->y+widget_style->y+a_column_line*FONT_HEIGHT+DISTANCE_TITLE_FILL,widget_style->widget_foreground);
        }
    } else {
        line_width=((window_style->width-widget_style->x)<=widget_style->widget_width)?window_style->width-widget_style->x:widget_style->widget_width;
        a_line_alphabet=line_width/FONT_WIDTH;
        memcpy(output_string,(const void*)((unsigned long)widget_style->widget_data+4),a_line_alphabet);
        if ((widget_style->y+FONT_HEIGHT)<window_style->height && strlen(output_string)) {
            lcd_draw_rectangle(window_style->x+widget_style->x+DISTANCE_TITLE_TO_LEFT,window_style->y+widget_style->y+DISTANCE_TITLE_FILL,window_style->x+widget_style->x+line_width+DISTANCE_TITLE_TO_LEFT,window_style->y+widget_style->y+FONT_HEIGHT+DISTANCE_TITLE_FILL,widget_style->widget_background,true);
            graphics_draw_font_string(output_string,window_style->x+widget_style->x+DISTANCE_TITLE_TO_LEFT,window_style->y+widget_style->y+DISTANCE_TITLE_FILL,widget_style->widget_foreground);
        }
    }
    widget_style->widget_visual=true;
}

void set_label_multiline(window_handle window_index,widget_handle widget_index,bool is_multiline) {
    if (INVALID_WINDOW_HANDLE!=window_index && INVALID_WIDGET_HANDLE!=widget_index) {
        if (is_multiline)
            *(unsigned long*)window_list.current_window_data[window_index].widget_list[widget_index].widget_data=1;
        else
            *(unsigned long*)window_list.current_window_data[window_index].widget_list[widget_index].widget_data=0;
        if (window_list.current_window_data[window_index].widget_list[widget_index].widget_visual)
            draw_label(&window_list.current_window_data[window_index],&window_list.current_window_data[window_index].widget_list[widget_index]);
    }
}

void set_label_caption(window_handle window_index,widget_handle widget_index,const char* string) {
    if (INVALID_WINDOW_HANDLE!=window_index && INVALID_WIDGET_HANDLE!=widget_index) {
        memset((void*)((unsigned long)window_list.current_window_data[window_index].widget_list[widget_index].widget_data+4),0,data_length_label-4);
        window_list.current_window_data[window_index].widget_list[widget_index].widget_data_length=(strlen(string)<=(data_length_label-4))?strlen(string):(data_length_label-4);
        memcpy((void*)((unsigned long)window_list.current_window_data[window_index].widget_list[widget_index].widget_data+4),string,window_list.current_window_data[window_index].widget_list[widget_index].widget_data_length);
        if (window_list.current_window_data[window_index].widget_list[widget_index].widget_visual)
            draw_label(&window_list.current_window_data[window_index],&window_list.current_window_data[window_index].widget_list[widget_index]);
    }
}

/*
         ___________________________
        |Title  [Blue Color]     - x|
        |---------------------------|
        |         ________          |
        |        |        |         |
        |        |  FONT  |         |
        |        |________|         |
        |___________________________|

*/

#define DISTANCE_BUTTON_STRING_TO_SIDE 2
#define COLOR_BUTTON_SIDE              2

static void draw_button(point_window_data window_style,point_window_widget widget_style) {
    unsigned int draw_height=0;
    unsigned int font_x=0;
    unsigned int font_y=0;
    unsigned int font_length=strlen((const char*)(unsigned long)widget_style->widget_data+4)/2;
    
    if (NULL!=window_style && NULL!=widget_style) {
        if (widget_button==widget_style->widget_type) {
            draw_height=widget_style->y+widget_style->widget_height;
            draw_height=(draw_height<(window_style->y+window_style->height))?draw_height:(((long)((window_style->y+window_style->height)-draw_height-FONT_HEIGHT-2*DISTANCE_BUTTON_STRING_TO_SIDE)>=0)?FONT_HEIGHT+2*DISTANCE_BUTTON_STRING_TO_SIDE:0);
            if (draw_height) {
                lcd_calcu_middle_location(window_style->x+widget_style->x,window_style->y+widget_style->y,
                                          window_style->x+widget_style->x+widget_style->widget_width,window_style->y+widget_style->y+widget_style->widget_height,&font_x,&font_y);
                lcd_draw_rectangle(window_style->x+widget_style->x+DISTANCE_TITLE_TO_LEFT,window_style->y+widget_style->y+DISTANCE_TITLE_FILL,
                                   window_style->x+widget_style->x+widget_style->widget_width+DISTANCE_TITLE_TO_LEFT,window_style->y+widget_style->y+widget_style->widget_height+DISTANCE_TITLE_FILL,widget_style->widget_background,true);                
                lcd_draw_rectangle(window_style->x+widget_style->x+DISTANCE_TITLE_TO_LEFT,window_style->y+widget_style->y+DISTANCE_TITLE_FILL,
                                   window_style->x+widget_style->x+widget_style->widget_width+DISTANCE_TITLE_TO_LEFT,window_style->y+widget_style->y+widget_style->widget_height+DISTANCE_TITLE_FILL,COLOR_BUTTON_SIDE,false);                
                font_y-=FONT_HEIGHT/2;
                if (font_length%2)
                    graphics_draw_font_string((const char*)((unsigned long)widget_style->widget_data+4),font_x-font_length*FONT_WIDTH,font_y+DISTANCE_TITLE_FILL,widget_style->widget_foreground);
                else
                    graphics_draw_font_string((const char*)((unsigned long)widget_style->widget_data+4),font_x-(font_length*FONT_WIDTH-FONT_WIDTH/2),font_y+DISTANCE_TITLE_FILL,widget_style->widget_foreground);
                widget_style->widget_visual=true;
            }
        }
    }
}

void set_button_click_recall(window_handle window_index,widget_handle widget_index,button_click_recall recall_address) {
    if (INVALID_WINDOW_HANDLE!=window_index && INVALID_WIDGET_HANDLE!=widget_index)
        memcpy(window_list.current_window_data[window_index].widget_list[widget_index].widget_data,&recall_address,sizeof(button_click_recall));
}

void set_button_caption(window_handle window_index,widget_handle widget_index,const char* string) {
    if (INVALID_WINDOW_HANDLE!=window_index && INVALID_WIDGET_HANDLE!=widget_index) {
        window_list.current_window_data[window_index].widget_list[widget_index].widget_data_length=(strlen(string)>(data_length_button-4))?data_length_button-4:strlen(string);
        memcpy((void*)((unsigned long)window_list.current_window_data[window_index].widget_list[widget_index].widget_data+4),string,window_list.current_window_data[window_index].widget_list[widget_index].widget_data_length);
        if (window_list.current_window_data[window_index].widget_list[widget_index].widget_visual)
            draw_button(&window_list.current_window_data[window_index],&window_list.current_window_data[window_index].widget_list[widget_index]);
    }
}

/*
         ___________________________
        |Title  [Blue Color]     - x|
        |---------------------------|
        |                           |
        |         ________          |
        |        |________|         |
        |                           |
        |___________________________|

         ___________________________
        |Title  [Blue Color]     - x|
        |---------------------------|
        |                           |
        |         ________          |
        |        |000|____|         |
        |                           |
        |___________________________|

*/

#define PROGRESS_LENGTH_SIDE    1
#define PROGRESS_COLOR_SIDE white

#define PROGRESS_MAX_VALUE    100

static void draw_progress(point_window_data window_style,point_window_widget widget_style) {
    unsigned long max_value=0;
    unsigned long current_value=0;
    double one_percentage_conver_pixel=0.0;
    
    if (NULL!=window_style && NULL!=widget_style) {
        lcd_draw_rectangle(window_style->x+widget_style->x-PROGRESS_LENGTH_SIDE,window_style->y+widget_style->y-PROGRESS_LENGTH_SIDE,
                           window_style->x+widget_style->x+widget_style->widget_width+PROGRESS_LENGTH_SIDE,window_style->y+widget_style->y+widget_style->widget_height+PROGRESS_LENGTH_SIDE,PROGRESS_COLOR_SIDE,true);
        lcd_draw_rectangle(window_style->x+widget_style->x,window_style->y+widget_style->y,
                           window_style->x+widget_style->x+widget_style->widget_width,window_style->y+widget_style->y+widget_style->widget_height,widget_style->widget_background,true);
        memcpy(&max_value,widget_style->widget_data,4);
        memcpy(&current_value,(const void*)((unsigned long)widget_style->widget_data+4),4);
        if (max_value<=PROGRESS_MAX_VALUE && current_value<=PROGRESS_MAX_VALUE && current_value<=max_value) {
            one_percentage_conver_pixel=widget_style->widget_width*1.0/100;
            lcd_draw_rectangle(window_style->x+widget_style->x,window_style->y+widget_style->y,
                               window_style->x+widget_style->x+(one_percentage_conver_pixel*((100*current_value)/max_value)),window_style->y+widget_style->y+widget_style->widget_height,widget_style->widget_foreground,true);
            widget_style->widget_visual=true;
        }
    }
}

void set_progress_max_value(window_handle window_index,widget_handle widget_index,unsigned long max_value) {
    unsigned int current_value=0;
    
    if (INVALID_WINDOW_HANDLE!=window_index && INVALID_WIDGET_HANDLE!=widget_index && max_value<=PROGRESS_MAX_VALUE) {
        memcpy(window_list.current_window_data[window_index].widget_list[widget_index].widget_data,&max_value,sizeof(unsigned long));
        memcpy((void*)((unsigned long)window_list.current_window_data[window_index].widget_list[widget_index].widget_data+4),&current_value,sizeof(unsigned long));
        if (window_list.current_window_data[window_index].widget_list[widget_index].widget_visual)
            draw_progress(&window_list.current_window_data[window_index],&window_list.current_window_data[window_index].widget_list[widget_index]);
    }
}

void set_progress_value(window_handle window_index,widget_handle widget_index,unsigned long current_value) {
    unsigned long max_value=0;
    
    if (INVALID_WINDOW_HANDLE!=window_index && INVALID_WIDGET_HANDLE!=widget_index && current_value<=PROGRESS_MAX_VALUE) {
        memcpy(&max_value,window_list.current_window_data[window_index].widget_list[widget_index].widget_data,sizeof(unsigned long));
        if (current_value<=max_value) {
            memcpy((void*)((unsigned long)window_list.current_window_data[window_index].widget_list[widget_index].widget_data+4),&current_value,sizeof(unsigned long));
            if (window_list.current_window_data[window_index].widget_list[widget_index].widget_visual)
                draw_progress(&window_list.current_window_data[window_index],&window_list.current_window_data[window_index].widget_list[widget_index]);
        }
    }
}

/*
         ___________________________
        |Title  [Blue Color]     - x|
        |---------------------------|
        |                           |
        |         ________          |
        |        |        |         |
        |        | -    - |         |
        |        |    |   |         |
        |        |   ---  |         |
        |        |________|         |
        |                           |
        |                           |
        |___________________________|

*/

static void draw_picture(point_window_data window_style,point_window_widget widget_style) {
    bitmap_data point_bitmap_data=NULL;
    
    if (NULL!=window_style && NULL!=widget_style) {
        memcpy(&point_bitmap_data,(const void*)((unsigned long)widget_style->widget_data+8),sizeof(bitmap_data));
        if (NULL!=point_bitmap_data) {
            if ((widget_style->x+widget_style->widget_width)<=window_style->width &&
                (widget_style->y+widget_style->widget_height)<=window_style->height) {
                 graphics_draw_bitmap(point_bitmap_data,window_style->x+widget_style->x,window_style->y+widget_style->y+DISTANCE_TITLE_FILL);
                     widget_style->widget_visual=true;
            }
        }
    }
}

void set_picture_bitmap(window_handle window_index,widget_handle widget_index,bitmap_data bitmap) {
    unsigned long height=0;
    unsigned long width=0;
    
    if (INVALID_WINDOW_HANDLE!=window_index && INVALID_WIDGET_HANDLE!=widget_index && NULL!=bitmap) {
        graphics_get_bitmap(bitmap,NULL,&height,&width);
        memcpy(window_list.current_window_data[window_index].widget_list[widget_index].widget_data,&height,sizeof(unsigned long));
        memcpy((void*)((unsigned long)window_list.current_window_data[window_index].widget_list[widget_index].widget_data+4),&width,sizeof(unsigned long));
        memcpy((void*)((unsigned long)window_list.current_window_data[window_index].widget_list[widget_index].widget_data+8),&bitmap,sizeof(bitmap_data));
        window_list.current_window_data[window_index].widget_list[widget_index].widget_width=width;
        window_list.current_window_data[window_index].widget_list[widget_index].widget_height=height;
        if (window_list.current_window_data[window_index].widget_list[widget_index].widget_visual)
            draw_picture(&window_list.current_window_data[window_index],&window_list.current_window_data[window_index].widget_list[widget_index]);
    }
}

void set_picture_empty(window_handle window_index,widget_handle widget_index) {
    unsigned long bitmap_data_address=NULL;
    if (INVALID_WINDOW_HANDLE!=window_index && INVALID_WIDGET_HANDLE!=widget_index) {
        memcpy((void*)((unsigned long)window_list.current_window_data[window_index].widget_list[widget_index].widget_data+8),&bitmap_data_address,sizeof(bitmap_data));
        draw_picture(&window_list.current_window_data[window_index],&window_list.current_window_data[window_index].widget_list[widget_index]);
    }
}

void set_picture_click_recall(window_handle window_index,widget_handle widget_index,button_click_recall recall_address) {
    if (INVALID_WINDOW_HANDLE!=window_index && INVALID_WIDGET_HANDLE!=widget_index && NULL!=recall_address)
        memcpy((void*)((unsigned long)window_list.current_window_data[window_index].widget_list[widget_index].widget_data+12),&recall_address,sizeof(button_click_recall));
}

/*
         ___________________________
        |Title  [Blue Color]     - x|
        |---------------------------|
        |                           |
        |         ________          |
        |        |List1 |^|         |
        |        |List2 |_|         |
        |        |List3 |_|         |
        |        |      |v|         |
        |        |______|_|         |
        |                           |
        |                           |
        |___________________________|

*/

void draw_widget(window_handle window_index,widget_handle widget_index) {
    if (INVALID_WINDOW_HANDLE!=window_index && INVALID_WIDGET_HANDLE!=widget_index) {
        switch (window_list.current_window_data[window_index].widget_list[widget_index].widget_type) {
            case widget_label:
                draw_label(&window_list.current_window_data[window_index],&window_list.current_window_data[window_index].widget_list[widget_index]);
                break;
            case widget_progress:
                draw_progress(&window_list.current_window_data[window_index],&window_list.current_window_data[window_index].widget_list[widget_index]);
                break;
            case widget_picture:
                draw_picture(&window_list.current_window_data[window_index],&window_list.current_window_data[window_index].widget_list[widget_index]);
                break;
            case widget_text:
                break;
            case widget_list:
                break;
            case widget_combox:
                break;
            case widget_button:
                draw_button(&window_list.current_window_data[window_index],&window_list.current_window_data[window_index].widget_list[widget_index]);
                break;
            default:
                break;
        }
    }
}

bool draw_line(window_handle handle,widget_location x1,widget_location y1,widget_location x2,widget_location y2,color new_color) {
    if (handle==get_window_front()) {
        if (x1<=SCREEN_HORIZONTAL_PIXEL &&
            y1<=SCREEN_LONGITUDINAL_PIXEL &&
            x2<=SCREEN_HORIZONTAL_PIXEL &&
            y2<=SCREEN_LONGITUDINAL_PIXEL) {
            lcd_draw_line(x1,y1,x2,y2,new_color);
            return true;
        }
    }
    return false;
}

bool draw_circle(window_handle handle,widget_location x,widget_location y,widget_size r,color new_color) {
    if (handle==get_window_front()) {
        if ((long)(x-r)>=0 &&
            (long)(y-r)>=0 &&
            (x+r)<=SCREEN_HORIZONTAL_PIXEL &&
            (y+r)<=SCREEN_LONGITUDINAL_PIXEL) {
            lcd_draw_circle(x,y,r,new_color);
            return true;
        }
    }
    return false;
}

void set_widget_background_color(window_handle window_index,widget_handle widget_index,color new_color) {
    if (INVALID_WINDOW_HANDLE!=window_index && INVALID_WIDGET_HANDLE!=widget_index) {
        window_list.current_window_data[window_index].widget_list[widget_index].widget_background=new_color;
        draw_widget(window_index,widget_index);
    }
}

void set_widget_foreground_color(window_handle window_index,widget_handle widget_index,color new_color) {
    if (INVALID_WINDOW_HANDLE!=window_index && INVALID_WIDGET_HANDLE!=widget_index) {
        window_list.current_window_data[window_index].widget_list[widget_index].widget_foreground=new_color;
        draw_widget(window_index,widget_index);
    }
}
