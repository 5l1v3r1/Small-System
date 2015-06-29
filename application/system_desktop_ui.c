
#include "system_type.h"

#include <string.h>

#include "malloc.h"
#include "thread_main.h"
#include "file_api.h"
#include "graphics_bitmap.h"
#include "graphics_window.h"
#include "graphics_widget.h"

#include "system_desktop.h"
#include "system_desktop_ui.h"
#include "system_desktop_application_execute.h"
#include "application_local//system_desktop_application_local_data.h"

#include "telephone_ui.h"
#include "alarm_ui.h"

#define APP_NAME_LENGHT                  12

#define APP_ICON_HEIGHT                 100
#define APP_ICON_WIDTH                  100
#define APP_ICON_TO_LEFT                  8
#define APP_ICON_TO_TOP_MESSAGE_BAR      10
#define APP_ICON_TO_APP_ICON             12
#define APP_ICON_TO_APP_NAME             10
#define APP_ICNO_LINE_NUMBER              2
#define APP_NAME_HEIGHT                  16
#define APP_NAME_WIDTH    APP_NAME_LENGHT*8

#define APP_PATH     "0:System_Desktop_App"
#define APP_PATH_LENGTH                  21

#define APP_ICON_EXTENSION           ".bmp"

#define SYSTEM_DESKTOP_SHOW_APP_COUNT     4

window_handle system_desktop_handle=INVALID_WINDOW_HANDLE;

struct {
    unsigned long app_count;
    widget_handle app_icon_handle[SYSTEM_DESKTOP_SHOW_APP_COUNT];
    widget_handle app_name_handle[SYSTEM_DESKTOP_SHOW_APP_COUNT];
    char          app_name_list[SYSTEM_DESKTOP_SHOW_APP_COUNT][APP_PATH_LENGTH+APP_NAME_LENGHT+1];
    bool          app_local[SYSTEM_DESKTOP_SHOW_APP_COUNT];
} app_list;

void system_desktop_title(const char* new_title) {
    if (INVALID_WINDOW_HANDLE!=system_desktop_handle)
        set_window_title(system_desktop_handle,new_title);
}

static void app_click0(void) {
    telephone_init();
}

static void app_click1(void) {
    alarm_init();
}

static void app_click2(void) {
    system_desktop_application_execute_init(APP_PATH,app_list.app_name_list[2]);
}

static void app_click3(void) {
    system_desktop_application_execute_init(APP_PATH,app_list.app_name_list[3]);
}

#define APP_LOCAL_COUNT            2
#define APP_LOCAL_0_NAME "telephone"
#define APP_LOCAL_1_NAME "alarm"

extern const char* kernel_logo;

static void system_desktop_fresh_app_local_list(void) {
    app_list.app_count=APP_LOCAL_COUNT;
    set_picture_bitmap(system_desktop_handle,app_list.app_icon_handle[0],telephone_icon);
    set_picture_bitmap(system_desktop_handle,app_list.app_icon_handle[1],alarm_icon);
    set_label_caption(system_desktop_handle,app_list.app_name_handle[0],"telephone");
    set_label_caption(system_desktop_handle,app_list.app_name_handle[1],"alarm");
    memcpy(app_list.app_name_list[0],"telephone",9);
    memcpy(app_list.app_name_list[1],"alarm",9);
    app_list.app_local[0]=true;
    app_list.app_local[1]=true;
}

window_handle system_desktop_get_window_handle(void) {
    return system_desktop_handle;
}

void system_desktop_fresh_app_list(void) {
    unsigned long app_list_clean_index=APP_LOCAL_COUNT;
    unsigned long app_count=0;
    unsigned long app_load_index=0;
    unsigned long app_load_count=0;
    char name_buffer[MAX_FOLDER_NAME_LENGTH+1]={0};
    char app_path[APP_PATH_LENGTH+MAX_FOLDER_NAME_LENGTH+1]={0};
    unsigned long name_length={0};
    unsigned long icon_file_data_length=0;
    void* icon_file_data=NULL;
    FIL* icon_file=NULL;
    unsigned long icon_height=0;
    unsigned long icon_width=0;

    for (;app_list_clean_index<SYSTEM_DESKTOP_SHOW_APP_COUNT;++app_list_clean_index) {
        set_picture_empty(system_desktop_handle,app_list.app_icon_handle[app_list_clean_index]);
        set_label_caption(system_desktop_handle,app_list.app_name_handle[app_list_clean_index],"");
    }
    system_desktop_fresh_app_local_list();
    if (folder_enum_count(APP_PATH,&app_count)) {
        app_count=(app_count>=SYSTEM_DESKTOP_SHOW_APP_COUNT)?SYSTEM_DESKTOP_SHOW_APP_COUNT:app_count;
        app_count=((long)(SYSTEM_DESKTOP_SHOW_APP_COUNT-app_list.app_count)<=0)?0:(((SYSTEM_DESKTOP_SHOW_APP_COUNT-app_list.app_count)<=app_count)?(SYSTEM_DESKTOP_SHOW_APP_COUNT-app_list.app_count):app_count);
        if (app_count) {
            for (;app_load_index<app_count;++app_load_index) {
                folder_enum(APP_PATH,app_load_index,name_buffer,&name_length);
                memcpy(app_path+22,name_buffer,strlen(name_buffer));
                *(char*)((unsigned long)app_path+APP_PATH_LENGTH)='/';
                memcpy(app_path,APP_PATH,APP_PATH_LENGTH);
                *(char*)((unsigned long)app_path+strlen(app_path))='/';
                memcpy((void*)((unsigned long)app_path+strlen(app_path)),name_buffer,strlen(name_buffer));
                memcpy((void*)((unsigned long)app_path+strlen(app_path)),APP_ICON_EXTENSION,strlen(APP_ICON_EXTENSION));
                if (file_open(app_path,icon_file)) {
                    icon_file_data_length=file_length(icon_file);
                    icon_file_data=mymalloc(SRAMEX,icon_file_data_length);
                    if (NULL!=icon_file_data) {
                        file_read(icon_file,icon_file_data,icon_file_data_length);
                        graphics_get_bitmap((bitmap_data)icon_file_data,NULL,&icon_height,&icon_width);
                        if (APP_ICON_HEIGHT==icon_height && APP_ICON_WIDTH==icon_width) {
                            set_picture_bitmap(system_desktop_handle,app_list.app_icon_handle[app_load_count],(bitmap_data)icon_file_data);
                            app_list.app_local[app_load_count]=false;
                            ++app_list.app_count;
                            memcpy(app_list.app_name_list[app_load_count],name_buffer,strlen(name_buffer));
                            set_label_caption(system_desktop_handle,app_list.app_name_handle[app_load_count],name_buffer);
                            ++app_load_count;
                        }
                        myfree(SRAMEX,icon_file_data);
                        icon_file_data=NULL;
                        icon_file_data_length=0;
                    }
                }
                memset(name_buffer,0,MAX_FOLDER_NAME_LENGTH+1);
                memset(app_path,0,APP_PATH_LENGTH+MAX_FOLDER_NAME_LENGTH+1);
                name_length=0;
            }
        }// else
//            messagebox(system_desktop_handle,"Ex_App List is empty","No Ex_App",messagebox_tips);
    } else {
        folder_create(APP_PATH);
//        messagebox(system_desktop_handle,"Ex_App List is empty","No Ex_App",messagebox_tips);
    }
}

void system_desktop_ui_init(window_handle init_window_handle) {
    window_widget app_icon[SYSTEM_DESKTOP_SHOW_APP_COUNT];
    window_widget app_name[SYSTEM_DESKTOP_SHOW_APP_COUNT];
    unsigned long app_init_index=0;
    
    system_desktop_handle=init_window_handle;
    memset(&app_icon,0,sizeof(app_icon));
    memset(&app_name,0,sizeof(app_name));
    
    show_window(system_desktop_handle);
    
    for (;app_init_index<SYSTEM_DESKTOP_SHOW_APP_COUNT;app_init_index+=APP_ICNO_LINE_NUMBER) {
        app_icon[app_init_index].widget_type=widget_picture;
        app_icon[app_init_index].x=APP_ICON_TO_LEFT;
        app_icon[app_init_index].y=APP_ICON_TO_TOP_MESSAGE_BAR+(app_init_index/APP_ICNO_LINE_NUMBER)*(APP_ICON_HEIGHT+APP_ICON_TO_APP_NAME*2+APP_NAME_HEIGHT);
        app_list.app_icon_handle[app_init_index]=create_widget(system_desktop_handle,&app_icon[app_init_index]);
        app_name[app_init_index].widget_type=widget_label;
        app_name[app_init_index].x=app_icon[app_init_index].x;
        app_name[app_init_index].y=app_icon[app_init_index].y+APP_ICON_HEIGHT+APP_ICON_TO_APP_NAME;
        app_name[app_init_index].widget_width=APP_NAME_WIDTH;
        app_name[app_init_index].widget_height=APP_NAME_HEIGHT;
        app_list.app_name_handle[app_init_index]=create_widget(system_desktop_handle,&app_name[app_init_index]);
        set_label_caption(system_desktop_handle,app_list.app_name_handle[app_init_index],"");
        app_icon[app_init_index+1].widget_type=widget_picture;
        app_icon[app_init_index+1].x=APP_ICON_TO_LEFT+APP_ICON_WIDTH+APP_ICON_TO_APP_ICON;
        app_icon[app_init_index+1].y=APP_ICON_TO_TOP_MESSAGE_BAR+(app_init_index/APP_ICNO_LINE_NUMBER)*(APP_ICON_HEIGHT+APP_ICON_TO_APP_NAME*2+APP_NAME_HEIGHT);
        app_list.app_icon_handle[app_init_index+1]=create_widget(system_desktop_handle,&app_icon[app_init_index+1]);
        app_name[app_init_index+1].widget_type=widget_label;
        app_name[app_init_index+1].x=app_icon[app_init_index+1].x;
        app_name[app_init_index+1].y=app_icon[app_init_index+1].y+APP_ICON_HEIGHT+APP_ICON_TO_APP_NAME;
        app_name[app_init_index+1].widget_width=APP_NAME_WIDTH;
        app_name[app_init_index+1].widget_height=APP_NAME_HEIGHT;
        app_list.app_name_handle[app_init_index+1]=create_widget(system_desktop_handle,&app_name[app_init_index+1]);
        set_label_caption(system_desktop_handle,app_list.app_name_handle[app_init_index+1],"");
    }
    set_picture_click_recall(system_desktop_handle,app_list.app_icon_handle[0],&app_click0);
    set_picture_click_recall(system_desktop_handle,app_list.app_icon_handle[1],&app_click1);
    set_picture_click_recall(system_desktop_handle,app_list.app_icon_handle[2],&app_click2);
    set_picture_click_recall(system_desktop_handle,app_list.app_icon_handle[3],&app_click3);
    system_desktop_fresh_app_list();
    refresh_window(system_desktop_handle);
}

void system_desktop_ui_notice(window_handle recall_window,notice_flag in_notice_flag,long parameter1,long parameter2) {
    switch (in_notice_flag) {
        case notice_close:
            system_desktop_init();
            break;
        default:
            break;
    }
}
