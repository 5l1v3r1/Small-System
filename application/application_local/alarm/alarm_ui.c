
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "beep_api.h"
#include "time_api.h"

#include "file_api.h"

#include "graphics_window.h"
#include "graphics_widget.h"

#include "alarm_ui.h"

#define MAX_ALARM_COUNT 2

#define INVALID_ALARM_LIST_INDEX 0xFF
#define INVALID_WEEK_NUMBER 0xFF

static struct {
    unsigned char alarm_count;
    time alarm_list[MAX_ALARM_COUNT];
} alarm_list;

typedef enum {
    no_show=0,
    show_create,
    show_setting,
} alarm_setting_show_state;

void alarm_exit(void);
static void alarm_setting_window_init(alarm_setting_show_state show_state,unsigned char alarm_list_index);
void alarm_notice(point_time now_time);

#define ALARM_LABEL_BACKGROUND_COLOR WINDOW_DEFUALT_BACKGROUND_COLOR
#define ALARM_LABEL_FOREGROUND_COLOR black
#define ALARM_LABEL_SELECT_BACKGROUND_COLOR black
#define ALARM_LABEL_SELECT_FOREGROUND_COLOR WINDOW_DEFUALT_BACKGROUND_COLOR

#define ALARM_LABEL_X 10
#define ALARM_LABEL_Y 15
#define ALARM_LABEL_WIDTH 150
#define ALARM_LABEL_HEIGHT 16

#define SELECT_BUTTON_WIDTH  55
#define SELECT_BUTTON_HEIGHT 36

#define ALARM_DETAIL_LABEL_WIDTH  215
#define ALARM_DETAIL_LABEL_HEIGHT 16

#define CONTROL_BUTTON_WIDTH  95
#define CONTROL_BUTTON_HEIGHT 35

#define DISTANCE_ALARM_LABEL_TO_SELECT_BUTTON   20
#define DISTANCE_SELECT_BUTTON_TO_SELECT_BUTTON 15
#define DISTANCE_SELECT_BUTTON_TO_ALARM_DETAIL_LABEL 40
#define DISTANCE_ALARM_DETAIL_LABEL_TO_CONTROL_BUTTON 25
#define DISTANCE_CONTROL_BUTTON_TO_CONTROL_BUTTON 40

static window_handle alarm_main_window=INVALID_WINDOW_HANDLE;
static widget_handle alarm_label_handle_list[MAX_ALARM_COUNT]={INVALID_WIDGET_HANDLE,INVALID_WIDGET_HANDLE};
static widget_handle alarm_list_detail_label=INVALID_WIDGET_HANDLE;
static unsigned char alarm_select_list_line=0;

static void alarm_select_caption_refresh(void) {
    char output_caption[16]={0};
    sprintf(output_caption,"Select Alarm:%1d",alarm_select_list_line);
    set_label_caption(alarm_main_window,alarm_list_detail_label,output_caption);
}

static void alarm_select_up_button(void) {
    if (!alarm_list.alarm_count) return;
    if (1==alarm_list.alarm_count) {
        alarm_select_list_line=0;
    } else {
        if (alarm_select_list_line-1>=0)
            --alarm_select_list_line;
        else
            alarm_select_list_line=alarm_list.alarm_count;
    }
    alarm_select_caption_refresh();
}

static void alarm_select_down_button(void) {
    if (!alarm_list.alarm_count) return;
    if (1==alarm_list.alarm_count) {
        alarm_select_list_line=0;
    } else {
        if (alarm_select_list_line+1<alarm_list.alarm_count)
            ++alarm_select_list_line;
        else
            alarm_select_list_line=0;
    }
    alarm_select_caption_refresh();
}

static void alarm_refresh_alarm_list(void) {
    unsigned char refresh_alarm_list_index=0;
    char label_caption[24]={0};
    for (;refresh_alarm_list_index<alarm_list.alarm_count;++refresh_alarm_list_index) {
        sprintf(label_caption,"%02d:%02d:%02d %1d",alarm_list.alarm_list[refresh_alarm_list_index].hour,alarm_list.alarm_list[refresh_alarm_list_index].min,alarm_list.alarm_list[refresh_alarm_list_index].second,alarm_list.alarm_list[refresh_alarm_list_index].week+1);
        set_label_caption(alarm_main_window,alarm_label_handle_list[refresh_alarm_list_index],label_caption);
        memset(label_caption,0,24);
    }
    if (MAX_ALARM_COUNT==refresh_alarm_list_index) return;
    sprintf(label_caption,"                        ");
    for (;refresh_alarm_list_index<MAX_ALARM_COUNT;++refresh_alarm_list_index)
        set_label_caption(alarm_main_window,alarm_label_handle_list[refresh_alarm_list_index],label_caption);
}

static void alarm_setting_button(void) {
    if (click_ok==messagebox(alarm_main_window,"Do you want to add a new alarm?","Alarm",messagebox_question)) {
        if (MAX_ALARM_COUNT==alarm_list.alarm_count) {
            messagebox(alarm_main_window,"Alarm has full!","Alarm",messagebox_tips);
            return;
        }
        alarm_setting_window_init(show_create,0);
    } else
        alarm_setting_window_init(show_setting,alarm_select_list_line);
}

static void alarm_delete_button(void) {
    if (click_ok==messagebox(alarm_main_window,"Do you want to cancel it?","Alarm",messagebox_question)) {
        if (!alarm_list.alarm_count) return;
        if (1==alarm_list.alarm_count) {
            time_alarm_delete(&alarm_list.alarm_list[0]);
            memset(&alarm_list.alarm_list[0],0,sizeof(time));
            alarm_select_list_line=0;
            alarm_list.alarm_count=0;
        } else {
            time_alarm_delete(&alarm_list.alarm_list[alarm_select_list_line]);
            memset(&alarm_list.alarm_list[alarm_select_list_line],0,sizeof(time));
            alarm_select_list_line=0;
            --alarm_list.alarm_count;
        }
        alarm_refresh_alarm_list();
    }
}

static void alarm_main_window_notice_recall(window_handle recall_window,notice_flag in_notice_flag,long parameter1,long parameter2) {
    switch (in_notice_flag) {
        case notice_click:
            alarm_refresh_alarm_list();
            alarm_select_caption_refresh();
            break;
        case notice_close:
            alarm_exit();
    }
}

void alarm_init(void) {
    window_handle alarm_window=INVALID_WINDOW_HANDLE;
    window_widget alarm_label;
    window_widget alarm_button;
    widget_handle alarm_button_handle=INVALID_WIDGET_HANDLE;
    unsigned char alarm_label_init_index=0;
    
    alarm_window=create_window("alarm","alarm",true,&alarm_main_window_notice_recall);
    if (INVALID_WINDOW_HANDLE!=alarm_window) {
        show_window(alarm_window);
        alarm_button.widget_type=widget_button;
        alarm_button.x=ALARM_LABEL_X+ALARM_LABEL_WIDTH+DISTANCE_ALARM_LABEL_TO_SELECT_BUTTON;
        alarm_button.y=ALARM_LABEL_Y;
        alarm_button.widget_height=SELECT_BUTTON_HEIGHT;
        alarm_button.widget_width=SELECT_BUTTON_WIDTH;
        alarm_button_handle=create_widget(alarm_window,&alarm_button);
        set_button_caption(alarm_window,alarm_button_handle,"Up");
        set_button_click_recall(alarm_window,alarm_button_handle,&alarm_select_up_button);
        alarm_button.y+=SELECT_BUTTON_HEIGHT+DISTANCE_SELECT_BUTTON_TO_SELECT_BUTTON;
        alarm_button_handle=create_widget(alarm_window,&alarm_button);
        set_button_caption(alarm_window,alarm_button_handle,"Down");
        set_button_click_recall(alarm_window,alarm_button_handle,&alarm_select_down_button);
        alarm_button.x=ALARM_LABEL_X;
        alarm_button.y+=DISTANCE_SELECT_BUTTON_TO_ALARM_DETAIL_LABEL+ALARM_DETAIL_LABEL_HEIGHT+DISTANCE_ALARM_DETAIL_LABEL_TO_CONTROL_BUTTON;
        alarm_button.widget_height=CONTROL_BUTTON_HEIGHT;
        alarm_button.widget_width=CONTROL_BUTTON_WIDTH;
        alarm_button_handle=create_widget(alarm_window,&alarm_button);
        set_button_caption(alarm_window,alarm_button_handle,"Setting");
        set_button_click_recall(alarm_window,alarm_button_handle,&alarm_setting_button);
        alarm_button.x+=CONTROL_BUTTON_WIDTH+DISTANCE_CONTROL_BUTTON_TO_CONTROL_BUTTON;
        alarm_button_handle=create_widget(alarm_window,&alarm_button);
        set_button_caption(alarm_window,alarm_button_handle,"Delete");
        set_button_click_recall(alarm_window,alarm_button_handle,&alarm_delete_button);
        
        alarm_label.widget_type=widget_label;
        alarm_label.x=ALARM_LABEL_X;
        alarm_label.y=alarm_button.y-DISTANCE_ALARM_DETAIL_LABEL_TO_CONTROL_BUTTON-ALARM_DETAIL_LABEL_HEIGHT;
        alarm_label.widget_width=ALARM_DETAIL_LABEL_WIDTH;
        alarm_label.widget_height=ALARM_DETAIL_LABEL_HEIGHT;
        alarm_list_detail_label=create_widget(alarm_window,&alarm_label);
        set_label_caption(alarm_window,alarm_list_detail_label,"");
        alarm_label.widget_width=ALARM_LABEL_WIDTH;
        alarm_label.widget_height=ALARM_LABEL_HEIGHT;
        for (;alarm_label_init_index<MAX_ALARM_COUNT;++alarm_label_init_index) {
            alarm_label.y=ALARM_LABEL_Y+alarm_label_init_index*ALARM_LABEL_HEIGHT;
            alarm_label_handle_list[alarm_label_init_index]=create_widget(alarm_window,&alarm_label);
            set_label_caption(alarm_window,alarm_label_handle_list[alarm_label_init_index],"");
        }
        refresh_window(alarm_window);
        alarm_main_window=alarm_window;
    }
}

void alarm_exit(void) {
}

//  setting window

#define ALARM_SELECT_WEEK_LABEL_X 30
#define ALARM_SELECT_WEEK_LABEL_Y 10
#define ALARM_SELECT_WEEK_LABEL_WIDTH 180
#define ALARM_SELECT_WEEK_LABEL_HEIGHT 16

#define DISTANCE_SELECT_WEEK_LABEL_TO_SELECT_WEEK_BUTTON 10
#define DISTANCE_SELECT_WEEK_BUTTON_TO_SELECT_WEEK_BUTTON 5
#define DISTANCE_SELECT_WEEK_BUTTON_TO_HOUR_LABEL        60
#define DISTANCE_SELECT_WEEK_BUTTON_TO_ADD_BUTTON        30
#define DISTANCE_ADD_BUTTON_TO_SUB_BUTTON                15
#define DISTANCE_HOUR_LABEL_TO_ADD_BUTTON                30
#define DISTANCE_OK_BUTTON_TO_CANCEL_BUTTON              40

#define ALARM_SELECT_WEEK_BUTTON_Y (ALARM_SELECT_WEEK_LABEL_Y+ALARM_SELECT_WEEK_LABEL_HEIGHT+DISTANCE_SELECT_WEEK_LABEL_TO_SELECT_WEEK_BUTTON)
#define ALARM_SELECT_WEEK_BUTTON_WIDTH  20
#define ALARM_SELECT_WEEK_BUTTON_HEIGHT 20

#define ALARM_HOUR_LABEL_X 10
#define ALARM_HOUR_LABEL_Y (ALARM_SELECT_WEEK_BUTTON_Y+ALARM_SELECT_WEEK_BUTTON_HEIGHT+DISTANCE_SELECT_WEEK_BUTTON_TO_HOUR_LABEL)
#define ALARM_HOUR_LABEL_WIDTH  24
#define ALARM_HOUR_LABEL_HEIGHT 16
#define ALARM_HOUR_ADD_BUTTON_X (ALARM_HOUR_LABEL_X+ALARM_HOUR_LABEL_WIDTH+DISTANCE_HOUR_LABEL_TO_ADD_BUTTON)
#define ALARM_HOUR_ADD_BUTTON_Y (ALARM_SELECT_WEEK_BUTTON_Y+ALARM_SELECT_WEEK_BUTTON_HEIGHT+DISTANCE_SELECT_WEEK_BUTTON_TO_ADD_BUTTON)
#define ALARM_HOUR_ADD_BUTTON_WIDTH  45
#define ALARM_HOUR_ADD_BUTTON_HEIGHT 30 

#define ALARM_MIN_LABEL_X 130
#define ALARM_MIN_LABEL_Y ALARM_HOUR_LABEL_Y
#define ALARM_MIN_LABEL_WIDTH  24
#define ALARM_MIN_LABEL_HEIGHT 16
#define ALARM_MIN_ADD_BUTTON_X (ALARM_MIN_LABEL_X+ALARM_MIN_LABEL_WIDTH+DISTANCE_HOUR_LABEL_TO_ADD_BUTTON)
#define ALARM_MIN_ADD_BUTTON_Y (ALARM_SELECT_WEEK_BUTTON_Y+ALARM_SELECT_WEEK_BUTTON_HEIGHT+DISTANCE_SELECT_WEEK_BUTTON_TO_ADD_BUTTON)
#define ALARM_MIN_ADD_BUTTON_WIDTH  45
#define ALARM_MIN_ADD_BUTTON_HEIGHT 30 

#define ALARM_OK_BUTTON_X 10
#define ALARM_OK_BUTTON_Y 240
#define ALARM_OK_BUTTON_WIDTH  90
#define ALARM_OK_BUTTON_HEIGHT 35
#define ALARM_CANCEL_BUTTON_X (ALARM_OK_BUTTON_X+ALARM_OK_BUTTON_WIDTH+DISTANCE_OK_BUTTON_TO_CANCEL_BUTTON)
#define ALARM_CANCEL_BUTTON_Y ALARM_OK_BUTTON_Y

#define ALARM_SELECT_WEEK_BUTTON_COUNT 7

static window_handle alarm_setting_window_handle=INVALID_WINDOW_HANDLE;
static unsigned char select_week=INVALID_WEEK_NUMBER;
static widget_handle select_button_handle[ALARM_SELECT_WEEK_BUTTON_COUNT]={INVALID_WIDGET_HANDLE};
static widget_handle setting_hour_handle=INVALID_WIDGET_HANDLE;
static widget_handle setting_min_handle=INVALID_WIDGET_HANDLE;
static unsigned char setting_hour=0;
static unsigned char setting_min=0;
static unsigned char setting_alarm_list_index=INVALID_ALARM_LIST_INDEX;
static alarm_setting_show_state alarm_setting_state=no_show;

static void alarm_setting_window_notice_recall(window_handle recall_window,notice_flag in_notice_flag,long parameter1,long parameter2) {
    switch (in_notice_flag) {
        case notice_click:
            break;
        case notice_close:
            alarm_setting_window_handle=INVALID_WINDOW_HANDLE;
            select_week=INVALID_WEEK_NUMBER;
            setting_alarm_list_index=INVALID_ALARM_LIST_INDEX;
            alarm_setting_state=no_show;
            setting_hour_handle=INVALID_WIDGET_HANDLE;
            setting_min_handle=INVALID_WIDGET_HANDLE;
            setting_hour=0;
            setting_min=0;
            memset(&select_button_handle,INVALID_WIDGET_HANDLE,sizeof(select_button_handle));
    }
}

static void alarm_select_button_all_unselect(void) {
    unsigned char select_button_index=0;
    
    select_week=INVALID_WEEK_NUMBER;
    for (;select_button_index<ALARM_SELECT_WEEK_BUTTON_COUNT;++select_button_index)
        set_button_caption(alarm_setting_window_handle,select_button_handle[select_button_index],"X");
}

static void alarm_select_button_1_click(void) {
    alarm_select_button_all_unselect();
    select_week=0;
    set_button_caption(alarm_setting_window_handle,select_button_handle[0],"V");
}

static void alarm_select_button_2_click(void) {
    alarm_select_button_all_unselect();
    select_week=1;
    set_button_caption(alarm_setting_window_handle,select_button_handle[1],"V");
}

static void alarm_select_button_3_click(void) {
    alarm_select_button_all_unselect();
    select_week=2;
    set_button_caption(alarm_setting_window_handle,select_button_handle[2],"V");
}

static void alarm_select_button_4_click(void) {
    alarm_select_button_all_unselect();
    select_week=3;
    set_button_caption(alarm_setting_window_handle,select_button_handle[3],"V");
}

static void alarm_select_button_5_click(void) {
    alarm_select_button_all_unselect();
    select_week=4;
    set_button_caption(alarm_setting_window_handle,select_button_handle[4],"V");
}

static void alarm_select_button_6_click(void) {
    alarm_select_button_all_unselect();
    select_week=5;
    set_button_caption(alarm_setting_window_handle,select_button_handle[5],"V");
}

static void alarm_select_button_7_click(void) {
    alarm_select_button_all_unselect();
    select_week=6;
    set_button_caption(alarm_setting_window_handle,select_button_handle[6],"V");
}

static void alarm_hour_add_button_click(void) {
    char output_caption[4]={0};
    setting_hour=(setting_hour==23)?0:++setting_hour;
    sprintf(output_caption,"%02d",setting_hour);
    set_label_caption(alarm_setting_window_handle,setting_hour_handle,output_caption);
}

static void alarm_hour_sub_button_click(void) {
    char output_caption[4]={0};
    setting_hour=(setting_hour==0)?23:--setting_hour;
    sprintf(output_caption,"%02d",setting_hour);
    set_label_caption(alarm_setting_window_handle,setting_hour_handle,output_caption);
}

static void alarm_min_add_button_click(void) {
    char output_caption[4]={0};
    setting_min=(setting_min==59)?0:++setting_min;
    sprintf(output_caption,"%02d",setting_min);
    set_label_caption(alarm_setting_window_handle,setting_min_handle,output_caption);
}

static void alarm_min_sub_button_click(void) {
    char output_caption[4]={0};
    setting_min=(setting_min==0)?59:--setting_min;
    sprintf(output_caption,"%02d",setting_min);
    set_label_caption(alarm_setting_window_handle,setting_min_handle,output_caption);
}

static void alarm_ok_button_click(void) {
    if (show_create==alarm_setting_state) {
        alarm_list.alarm_list[alarm_list.alarm_count].week=select_week;
        alarm_list.alarm_list[alarm_list.alarm_count].hour=setting_hour;
        alarm_list.alarm_list[alarm_list.alarm_count].min=setting_min;
        alarm_list.alarm_list[alarm_list.alarm_count].second=0;
        ++alarm_list.alarm_count;
    } else {
        alarm_list.alarm_list[setting_alarm_list_index].week=select_week;
        alarm_list.alarm_list[setting_alarm_list_index].hour=setting_hour;
        alarm_list.alarm_list[setting_alarm_list_index].min=setting_min;
        alarm_list.alarm_list[setting_alarm_list_index].second=0;
    }
    time_alarm_add(&alarm_list.alarm_list[setting_alarm_list_index],&alarm_notice);
    close_window(alarm_setting_window_handle);
}

static void alarm_cancel_button_click(void) {
    close_window(alarm_setting_window_handle);
}

static void alarm_setting_window_init(alarm_setting_show_state show_state,unsigned char alarm_list_index) {
    window_handle alarm_setting_window=INVALID_WINDOW_HANDLE;
    widget_handle alarm_setting_widget_handle=INVALID_WIDGET_HANDLE;
    window_widget alarm_select_button;
    window_widget alarm_week_label;
    char output_caption[4]={0};
    
    alarm_setting_window=create_window("alarm_setting","alarm_setting",true,&alarm_setting_window_notice_recall);
    if (INVALID_WINDOW_HANDLE!=alarm_setting_window) {
        alarm_setting_state=show_state;
        show_window(alarm_setting_window);
        alarm_week_label.widget_type=widget_label;
        alarm_week_label.x=ALARM_SELECT_WEEK_LABEL_X;
        alarm_week_label.y=ALARM_SELECT_WEEK_LABEL_Y;
        alarm_week_label.widget_width=ALARM_SELECT_WEEK_LABEL_WIDTH;
        alarm_week_label.widget_height=ALARM_SELECT_WEEK_LABEL_HEIGHT;
        alarm_setting_widget_handle=create_widget(alarm_setting_window,&alarm_week_label);
        set_label_caption(alarm_setting_window,alarm_setting_widget_handle," 1  2  3  4   5  6  7");
        alarm_week_label.x=ALARM_HOUR_LABEL_X;
        alarm_week_label.y=ALARM_HOUR_LABEL_Y;
        alarm_week_label.widget_width=ALARM_HOUR_LABEL_WIDTH;
        alarm_week_label.widget_height=ALARM_HOUR_LABEL_HEIGHT;
        setting_hour_handle=create_widget(alarm_setting_window,&alarm_week_label);
        set_label_caption(alarm_setting_window,setting_hour_handle,"00");
        alarm_week_label.x=ALARM_MIN_LABEL_X;
        alarm_week_label.y=ALARM_MIN_LABEL_Y;
        alarm_week_label.widget_width=ALARM_MIN_LABEL_WIDTH;
        alarm_week_label.widget_height=ALARM_MIN_LABEL_HEIGHT;
        setting_min_handle=create_widget(alarm_setting_window,&alarm_week_label);
        set_label_caption(alarm_setting_window,setting_min_handle,"00");
        alarm_select_button.widget_type=widget_button;
        alarm_select_button.x=ALARM_SELECT_WEEK_LABEL_X;
        alarm_select_button.y=ALARM_SELECT_WEEK_BUTTON_Y;
        alarm_select_button.widget_width=ALARM_SELECT_WEEK_BUTTON_WIDTH;
        alarm_select_button.widget_height=ALARM_SELECT_WEEK_BUTTON_HEIGHT;
        alarm_setting_widget_handle=create_widget(alarm_setting_window,&alarm_select_button);
        set_button_caption(alarm_setting_window,alarm_setting_widget_handle,"X");
        set_button_click_recall(alarm_setting_window,alarm_setting_widget_handle,&alarm_select_button_1_click);
        select_button_handle[0]=alarm_setting_widget_handle;
        alarm_select_button.x+=ALARM_SELECT_WEEK_BUTTON_WIDTH+DISTANCE_SELECT_WEEK_BUTTON_TO_SELECT_WEEK_BUTTON;
        alarm_setting_widget_handle=create_widget(alarm_setting_window,&alarm_select_button);
        set_button_caption(alarm_setting_window,alarm_setting_widget_handle,"X");
        set_button_click_recall(alarm_setting_window,alarm_setting_widget_handle,&alarm_select_button_2_click);
        select_button_handle[1]=alarm_setting_widget_handle;
        alarm_select_button.x+=ALARM_SELECT_WEEK_BUTTON_WIDTH+DISTANCE_SELECT_WEEK_BUTTON_TO_SELECT_WEEK_BUTTON;
        alarm_setting_widget_handle=create_widget(alarm_setting_window,&alarm_select_button);
        set_button_caption(alarm_setting_window,alarm_setting_widget_handle,"X");
        set_button_click_recall(alarm_setting_window,alarm_setting_widget_handle,&alarm_select_button_3_click);
        select_button_handle[2]=alarm_setting_widget_handle;
        alarm_select_button.x+=ALARM_SELECT_WEEK_BUTTON_WIDTH+DISTANCE_SELECT_WEEK_BUTTON_TO_SELECT_WEEK_BUTTON;
        alarm_setting_widget_handle=create_widget(alarm_setting_window,&alarm_select_button);
        set_button_caption(alarm_setting_window,alarm_setting_widget_handle,"X");
        set_button_click_recall(alarm_setting_window,alarm_setting_widget_handle,&alarm_select_button_4_click);
        select_button_handle[3]=alarm_setting_widget_handle;
        alarm_select_button.x+=ALARM_SELECT_WEEK_BUTTON_WIDTH+DISTANCE_SELECT_WEEK_BUTTON_TO_SELECT_WEEK_BUTTON;
        alarm_setting_widget_handle=create_widget(alarm_setting_window,&alarm_select_button);
        set_button_caption(alarm_setting_window,alarm_setting_widget_handle,"X");
        set_button_click_recall(alarm_setting_window,alarm_setting_widget_handle,&alarm_select_button_5_click);
        select_button_handle[4]=alarm_setting_widget_handle;
        alarm_select_button.x+=ALARM_SELECT_WEEK_BUTTON_WIDTH+DISTANCE_SELECT_WEEK_BUTTON_TO_SELECT_WEEK_BUTTON;
        alarm_setting_widget_handle=create_widget(alarm_setting_window,&alarm_select_button);
        set_button_caption(alarm_setting_window,alarm_setting_widget_handle,"X");
        set_button_click_recall(alarm_setting_window,alarm_setting_widget_handle,&alarm_select_button_6_click);
        select_button_handle[5]=alarm_setting_widget_handle;
        alarm_select_button.x+=ALARM_SELECT_WEEK_BUTTON_WIDTH+DISTANCE_SELECT_WEEK_BUTTON_TO_SELECT_WEEK_BUTTON;
        alarm_setting_widget_handle=create_widget(alarm_setting_window,&alarm_select_button);
        set_button_caption(alarm_setting_window,alarm_setting_widget_handle,"X");
        set_button_click_recall(alarm_setting_window,alarm_setting_widget_handle,&alarm_select_button_7_click);
        select_button_handle[6]=alarm_setting_widget_handle;
        alarm_select_button.x=ALARM_HOUR_ADD_BUTTON_X;
        alarm_select_button.y=ALARM_HOUR_ADD_BUTTON_Y;
        alarm_select_button.widget_width=ALARM_HOUR_ADD_BUTTON_WIDTH;
        alarm_select_button.widget_height=ALARM_HOUR_ADD_BUTTON_HEIGHT;
        alarm_setting_widget_handle=create_widget(alarm_setting_window,&alarm_select_button);
        set_button_caption(alarm_setting_window,alarm_setting_widget_handle,"<");
        set_button_click_recall(alarm_setting_window,alarm_setting_widget_handle,&alarm_hour_add_button_click);
        alarm_select_button.y+=ALARM_HOUR_ADD_BUTTON_HEIGHT+DISTANCE_ADD_BUTTON_TO_SUB_BUTTON;
        alarm_setting_widget_handle=create_widget(alarm_setting_window,&alarm_select_button);
        set_button_caption(alarm_setting_window,alarm_setting_widget_handle,">");
        set_button_click_recall(alarm_setting_window,alarm_setting_widget_handle,&alarm_hour_sub_button_click);
        alarm_select_button.x=ALARM_MIN_ADD_BUTTON_X;
        alarm_select_button.y=ALARM_MIN_ADD_BUTTON_Y;
        alarm_setting_widget_handle=create_widget(alarm_setting_window,&alarm_select_button);
        set_button_caption(alarm_setting_window,alarm_setting_widget_handle,"<");
        set_button_click_recall(alarm_setting_window,alarm_setting_widget_handle,&alarm_min_add_button_click);
        alarm_select_button.y+=ALARM_MIN_ADD_BUTTON_HEIGHT+DISTANCE_ADD_BUTTON_TO_SUB_BUTTON;
        alarm_setting_widget_handle=create_widget(alarm_setting_window,&alarm_select_button);
        set_button_caption(alarm_setting_window,alarm_setting_widget_handle,">");
        set_button_click_recall(alarm_setting_window,alarm_setting_widget_handle,&alarm_min_sub_button_click);
        alarm_select_button.x=ALARM_OK_BUTTON_X;
        alarm_select_button.y=ALARM_OK_BUTTON_Y;
        alarm_select_button.widget_width=ALARM_OK_BUTTON_WIDTH;
        alarm_select_button.widget_height=ALARM_OK_BUTTON_HEIGHT;
        alarm_setting_widget_handle=create_widget(alarm_setting_window,&alarm_select_button);
        set_button_caption(alarm_setting_window,alarm_setting_widget_handle,"OK");
        set_button_click_recall(alarm_setting_window,alarm_setting_widget_handle,&alarm_ok_button_click);
        alarm_select_button.x=ALARM_CANCEL_BUTTON_X;
        alarm_setting_widget_handle=create_widget(alarm_setting_window,&alarm_select_button);
        set_button_caption(alarm_setting_window,alarm_setting_widget_handle,"Cancel");
        set_button_click_recall(alarm_setting_window,alarm_setting_widget_handle,&alarm_cancel_button_click);
        if (show_setting==show_state) {
            setting_alarm_list_index=alarm_list_index;
            switch (alarm_list.alarm_list[alarm_list_index].week) {
                case 0:
                    set_button_caption(alarm_setting_window,select_button_handle[0],"V");
                    break;
                case 1:
                    set_button_caption(alarm_setting_window,select_button_handle[1],"V");
                    break;
                case 2:
                    set_button_caption(alarm_setting_window,select_button_handle[2],"V");
                    break;
                case 3:
                    set_button_caption(alarm_setting_window,select_button_handle[3],"V");
                    break;
                case 4:
                    set_button_caption(alarm_setting_window,select_button_handle[4],"V");
                    break;
                case 5:
                    set_button_caption(alarm_setting_window,select_button_handle[5],"V");
                    break;
                case 6:
                    set_button_caption(alarm_setting_window,select_button_handle[6],"V");
                    break;
            }
            select_week=alarm_list.alarm_list[alarm_list_index].week;
            setting_hour=alarm_list.alarm_list[alarm_list_index].hour;
            setting_min=alarm_list.alarm_list[alarm_list_index].min;
            sprintf(output_caption,"%02d",setting_hour);
            set_label_caption(alarm_setting_window_handle,setting_hour_handle,output_caption);
            sprintf(output_caption,"%02d",setting_min);
            set_label_caption(alarm_setting_window_handle,setting_min_handle,output_caption);
        }
        refresh_window(alarm_setting_window);
        alarm_setting_window_handle=alarm_setting_window;
    }
}

//  notice window

static void alarm_notice_window_notice_recall(window_handle recall_window,notice_flag in_notice_flag,long parameter1,long parameter2) {
    switch (in_notice_flag) {
        case notice_click:
            break;
        case notice_close:
            break;
    }
}

static window_handle alarm_notice_window_handle=INVALID_WINDOW_HANDLE;

static void alarm_notice_exit(void) {
    beep_off();
    close_window(alarm_notice_window_handle);
    alarm_notice_window_handle=INVALID_WINDOW_HANDLE;
}

#define ALARM_NOTICE_LABEL_X 20
#define ALARM_NOTICE_LABEL_Y 25
#define ALARM_NOTICE_LABEL_WIDTH  36
#define ALARM_NOTICE_LABEL_HEIGHT 16

#define ALARM_NOTICE_CLOSE_BUTTON_X 60
#define ALARM_NOTICE_CLOSE_BUTTON_Y 80
#define ALARM_NOTICE_CLOSE_BUTTON_WIDTH  100
#define ALARM_NOTICE_CLOSE_BUTTON_HEIGHT 50

void alarm_notice(point_time now_time) {
    window_widget alarm_tips_label;
    window_widget alarm_close_button;
    window_handle alarm_window_notice=create_window("alarm_notice","alarm_notice",true,&alarm_notice_window_notice_recall);
    widget_handle alarm_widget_handle=INVALID_WIDGET_HANDLE;
    
    alarm_tips_label.widget_type=widget_label;
    alarm_tips_label.x=ALARM_NOTICE_LABEL_X;
    alarm_tips_label.y=ALARM_NOTICE_LABEL_Y;
    alarm_tips_label.widget_width=ALARM_NOTICE_LABEL_WIDTH;
    alarm_tips_label.widget_height=ALARM_NOTICE_LABEL_HEIGHT;
    alarm_widget_handle=create_widget(alarm_window_notice,&alarm_tips_label);
    set_label_caption(alarm_window_notice,alarm_widget_handle,"Alarm!");
    alarm_close_button.widget_type=widget_button;
    alarm_close_button.x=ALARM_NOTICE_CLOSE_BUTTON_X;
    alarm_close_button.y=ALARM_NOTICE_CLOSE_BUTTON_Y;
    alarm_close_button.widget_width=ALARM_NOTICE_CLOSE_BUTTON_WIDTH;
    alarm_close_button.widget_height=ALARM_NOTICE_CLOSE_BUTTON_HEIGHT;
    alarm_widget_handle=create_widget(alarm_window_notice,&alarm_close_button);
    set_button_caption(alarm_window_notice,alarm_widget_handle,"Close");
    set_button_click_recall(alarm_window_notice,alarm_widget_handle,&alarm_notice_exit);
    beep_on();
    refresh_window(alarm_window_notice);
    alarm_notice_window_handle=alarm_window_notice;
}
