
#include <string.h>

#include "beep_api.h"

#include "kernel_expand_gsm.h"
#include "graphics_window.h"
#include "graphics_widget.h"

#include "telephone_ui.h"

#define PHONE_NUMBER_LABEL_X          10
#define PHONE_NUMBER_LABEL_Y           5
#define PHONE_NUMBER_LABEL_HEIGHT     16
#define PHONE_NUMBER_LABEL_WIDET     220

#define WIDGET_DISTANCE_HORIZONTAL    15
#define WIDGET_DISTANCE_LONGITUDINAL  10

#define NUNBER_BUTTON_HEIGHT          51
#define NUNBER_BUTTON_WIDTH           51

#define NUMBER_BUTTON_1_X             PHONE_NUMBER_LABEL_X
#define NUMBER_BUTTON_1_Y             31

#define NUMBER_BUTTON_2_X             (NUMBER_BUTTON_1_X+NUNBER_BUTTON_WIDTH+WIDGET_DISTANCE_HORIZONTAL)
#define NUMBER_BUTTON_2_Y             31

#define NUMBER_BUTTON_3_X             (NUMBER_BUTTON_2_X+NUNBER_BUTTON_WIDTH+WIDGET_DISTANCE_HORIZONTAL)
#define NUMBER_BUTTON_3_Y             31

#define NUMBER_BUTTON_4_X             PHONE_NUMBER_LABEL_X
#define NUMBER_BUTTON_4_Y             92

#define NUMBER_BUTTON_5_X             (NUMBER_BUTTON_4_X+NUNBER_BUTTON_WIDTH+WIDGET_DISTANCE_HORIZONTAL)
#define NUMBER_BUTTON_5_Y             92

#define NUMBER_BUTTON_6_X             (NUMBER_BUTTON_5_X+NUNBER_BUTTON_WIDTH+WIDGET_DISTANCE_HORIZONTAL)
#define NUMBER_BUTTON_6_Y             92

#define NUMBER_BUTTON_7_X             PHONE_NUMBER_LABEL_X
#define NUMBER_BUTTON_7_Y             153

#define NUMBER_BUTTON_8_X             (NUMBER_BUTTON_7_X+NUNBER_BUTTON_WIDTH+WIDGET_DISTANCE_HORIZONTAL)
#define NUMBER_BUTTON_8_Y             153

#define NUMBER_BUTTON_9_X             (NUMBER_BUTTON_8_X+NUNBER_BUTTON_WIDTH+WIDGET_DISTANCE_HORIZONTAL)
#define NUMBER_BUTTON_9_Y             153

#define CALL_BUTTON_X                 PHONE_NUMBER_LABEL_X
#define CALL_BUTTON_Y                 214

#define NUMBER_BUTTON_0_X             (CALL_BUTTON_X+NUNBER_BUTTON_WIDTH+WIDGET_DISTANCE_HORIZONTAL)
#define NUMBER_BUTTON_0_Y             214

#define BACK_BUTTON_X                 (NUMBER_BUTTON_0_X+NUNBER_BUTTON_WIDTH+WIDGET_DISTANCE_HORIZONTAL)
#define BACK_BUTTON_Y                 214

window_handle telephone_window_handle=INVALID_WINDOW_HANDLE;
widget_handle phone_number_laben_handle=INVALID_WIDGET_HANDLE;

#define PHONE_NUMBER_LENGTH           20

char call_phone_number_buffer[PHONE_NUMBER_LENGTH]={0};

static void telephone_exit(void);

static void number_button_1_click(void) {
    if (strlen(call_phone_number_buffer)<=PHONE_NUMBER_LENGTH) {
        strcat(call_phone_number_buffer,"1");
        set_label_caption(telephone_window_handle,phone_number_laben_handle,call_phone_number_buffer);
    }
}
static void number_button_2_click(void) {
    if (strlen(call_phone_number_buffer)<=PHONE_NUMBER_LENGTH) {
        strcat(call_phone_number_buffer,"2");
        set_label_caption(telephone_window_handle,phone_number_laben_handle,call_phone_number_buffer);
    }
}
static void number_button_3_click(void) {
    if (strlen(call_phone_number_buffer)<=PHONE_NUMBER_LENGTH) {
        strcat(call_phone_number_buffer,"3");
        set_label_caption(telephone_window_handle,phone_number_laben_handle,call_phone_number_buffer);
    }
}
static void number_button_4_click(void) {
    if (strlen(call_phone_number_buffer)<=PHONE_NUMBER_LENGTH) {
        strcat(call_phone_number_buffer,"4");
        set_label_caption(telephone_window_handle,phone_number_laben_handle,call_phone_number_buffer);
    }
}
static void number_button_5_click(void) {
    if (strlen(call_phone_number_buffer)<=PHONE_NUMBER_LENGTH) {
        strcat(call_phone_number_buffer,"5");
        set_label_caption(telephone_window_handle,phone_number_laben_handle,call_phone_number_buffer);
    }
}
static void number_button_6_click(void) {
    if (strlen(call_phone_number_buffer)<=PHONE_NUMBER_LENGTH) {
        strcat(call_phone_number_buffer,"6");
        set_label_caption(telephone_window_handle,phone_number_laben_handle,call_phone_number_buffer);
    }
}
static void number_button_7_click(void) {
    if (strlen(call_phone_number_buffer)<=PHONE_NUMBER_LENGTH) {
        strcat(call_phone_number_buffer,"7");
        set_label_caption(telephone_window_handle,phone_number_laben_handle,call_phone_number_buffer);
    }
}
static void number_button_8_click(void) {
    if (strlen(call_phone_number_buffer)<=PHONE_NUMBER_LENGTH) {
        strcat(call_phone_number_buffer,"8");
        set_label_caption(telephone_window_handle,phone_number_laben_handle,call_phone_number_buffer);
    }
}
static void number_button_9_click(void) {
    if (strlen(call_phone_number_buffer)<=PHONE_NUMBER_LENGTH) {
        strcat(call_phone_number_buffer,"9");
        set_label_caption(telephone_window_handle,phone_number_laben_handle,call_phone_number_buffer);
    }
}
static void number_button_0_click(void) {
    if (strlen(call_phone_number_buffer)<=PHONE_NUMBER_LENGTH) {
        strcat(call_phone_number_buffer,"0");
        set_label_caption(telephone_window_handle,phone_number_laben_handle,call_phone_number_buffer);
    }
}
static void call_button_click(void) {
    if ('\0'!=*call_phone_number_buffer) {
        gsm_call(call_phone_number_buffer);
        telephone_dial_init(call_phone_number_buffer,dial_call);
        memset(call_phone_number_buffer,0,PHONE_NUMBER_LENGTH);
        set_label_caption(telephone_window_handle,phone_number_laben_handle,"");
    }
}
static void back_button_click(void) {
    if (strlen(call_phone_number_buffer)) {
        call_phone_number_buffer[strlen(call_phone_number_buffer)-1]='\0';
        set_label_caption(telephone_window_handle,phone_number_laben_handle,call_phone_number_buffer);
    }
}

static void telephone_main_window_notice_recall(window_handle recall_window,notice_flag in_notice_flag,long parameter1,long parameter2) {
    switch (in_notice_flag) {
        case notice_click:
            break;
        case notice_close:
            telephone_window_handle=INVALID_WINDOW_HANDLE;
            phone_number_laben_handle=INVALID_WIDGET_HANDLE;
            telephone_exit();
    }
}

bool telephone_init(void) {
    window_widget phone_number_label;
    window_widget number_button_0;
    window_widget number_button_1;
    window_widget number_button_2;
    window_widget number_button_3;
    window_widget number_button_4;
    window_widget number_button_5;
    window_widget number_button_6;
    window_widget number_button_7;
    window_widget number_button_8;
    window_widget number_button_9;
    window_widget call_button;
    window_widget back_button;
    widget_handle number_button_handle=INVALID_WIDGET_HANDLE;
    
    telephone_window_handle=create_window("telephone","telephone",true,&telephone_main_window_notice_recall);
    show_window(telephone_window_handle);
    
    if (INVALID_WINDOW_HANDLE!=telephone_window_handle) {
        phone_number_label.widget_type=widget_label;
        phone_number_label.x=PHONE_NUMBER_LABEL_X;
        phone_number_label.y=PHONE_NUMBER_LABEL_Y;
        phone_number_label.widget_height=PHONE_NUMBER_LABEL_HEIGHT;
        phone_number_label.widget_width=PHONE_NUMBER_LABEL_WIDET;
        number_button_1.widget_type=widget_button;
        number_button_1.x=NUMBER_BUTTON_1_X;
        number_button_1.y=NUMBER_BUTTON_1_Y;
        number_button_1.widget_height=NUNBER_BUTTON_HEIGHT;
        number_button_1.widget_width=NUNBER_BUTTON_WIDTH;
        number_button_2.widget_type=widget_button;
        number_button_2.x=NUMBER_BUTTON_2_X;
        number_button_2.y=NUMBER_BUTTON_2_Y;
        number_button_2.widget_height=NUNBER_BUTTON_HEIGHT;
        number_button_2.widget_width=NUNBER_BUTTON_WIDTH;
        number_button_3.widget_type=widget_button;
        number_button_3.x=NUMBER_BUTTON_3_X;
        number_button_3.y=NUMBER_BUTTON_3_Y;
        number_button_3.widget_height=NUNBER_BUTTON_HEIGHT;
        number_button_3.widget_width=NUNBER_BUTTON_WIDTH;
        number_button_4.widget_type=widget_button;
        number_button_4.x=NUMBER_BUTTON_4_X;
        number_button_4.y=NUMBER_BUTTON_4_Y;
        number_button_4.widget_height=NUNBER_BUTTON_HEIGHT;
        number_button_4.widget_width=NUNBER_BUTTON_WIDTH;
        number_button_5.widget_type=widget_button;
        number_button_5.x=NUMBER_BUTTON_5_X;
        number_button_5.y=NUMBER_BUTTON_5_Y;
        number_button_5.widget_height=NUNBER_BUTTON_HEIGHT;
        number_button_5.widget_width=NUNBER_BUTTON_WIDTH;
        number_button_6.widget_type=widget_button;
        number_button_6.x=NUMBER_BUTTON_6_X;
        number_button_6.y=NUMBER_BUTTON_6_Y;
        number_button_6.widget_height=NUNBER_BUTTON_HEIGHT;
        number_button_6.widget_width=NUNBER_BUTTON_WIDTH;
        number_button_7.widget_type=widget_button;
        number_button_7.x=NUMBER_BUTTON_7_X;
        number_button_7.y=NUMBER_BUTTON_7_Y;
        number_button_7.widget_height=NUNBER_BUTTON_HEIGHT;
        number_button_7.widget_width=NUNBER_BUTTON_WIDTH;
        number_button_8.widget_type=widget_button;
        number_button_8.x=NUMBER_BUTTON_8_X;
        number_button_8.y=NUMBER_BUTTON_8_Y;
        number_button_8.widget_height=NUNBER_BUTTON_HEIGHT;
        number_button_8.widget_width=NUNBER_BUTTON_WIDTH;
        number_button_9.widget_type=widget_button;
        number_button_9.x=NUMBER_BUTTON_9_X;
        number_button_9.y=NUMBER_BUTTON_9_Y;
        number_button_9.widget_height=NUNBER_BUTTON_HEIGHT;
        number_button_9.widget_width=NUNBER_BUTTON_WIDTH;
        number_button_0.widget_type=widget_button;
        number_button_0.x=NUMBER_BUTTON_0_X;
        number_button_0.y=NUMBER_BUTTON_0_Y;
        number_button_0.widget_height=NUNBER_BUTTON_HEIGHT;
        number_button_0.widget_width=NUNBER_BUTTON_WIDTH;
        call_button.widget_type=widget_button;
        call_button.x=CALL_BUTTON_X;
        call_button.y=CALL_BUTTON_Y;
        call_button.widget_height=NUNBER_BUTTON_HEIGHT;
        call_button.widget_width=NUNBER_BUTTON_WIDTH;
        back_button.widget_type=widget_button;
        back_button.x=BACK_BUTTON_X;
        back_button.y=BACK_BUTTON_Y;
        back_button.widget_height=NUNBER_BUTTON_HEIGHT;
        back_button.widget_width=NUNBER_BUTTON_WIDTH;
        phone_number_laben_handle=create_widget(telephone_window_handle,&phone_number_label);
        set_label_caption(telephone_window_handle,phone_number_laben_handle," ");
        number_button_handle=create_widget(telephone_window_handle,&number_button_1);
        set_button_caption(telephone_window_handle,number_button_handle,"1");
        set_button_click_recall(telephone_window_handle,number_button_handle,&number_button_1_click);
        number_button_handle=create_widget(telephone_window_handle,&number_button_2);
        set_button_caption(telephone_window_handle,number_button_handle,"2");
        set_button_click_recall(telephone_window_handle,number_button_handle,&number_button_2_click);
        number_button_handle=create_widget(telephone_window_handle,&number_button_3);
        set_button_caption(telephone_window_handle,number_button_handle,"3");
        set_button_click_recall(telephone_window_handle,number_button_handle,&number_button_3_click);
        number_button_handle=create_widget(telephone_window_handle,&number_button_4);
        set_button_caption(telephone_window_handle,number_button_handle,"4");
        set_button_click_recall(telephone_window_handle,number_button_handle,&number_button_4_click);
        number_button_handle=create_widget(telephone_window_handle,&number_button_5);
        set_button_caption(telephone_window_handle,number_button_handle,"5");
        set_button_click_recall(telephone_window_handle,number_button_handle,&number_button_5_click);
        number_button_handle=create_widget(telephone_window_handle,&number_button_6);
        set_button_caption(telephone_window_handle,number_button_handle,"6");
        set_button_click_recall(telephone_window_handle,number_button_handle,&number_button_6_click);
        number_button_handle=create_widget(telephone_window_handle,&number_button_7);
        set_button_caption(telephone_window_handle,number_button_handle,"7");
        set_button_click_recall(telephone_window_handle,number_button_handle,&number_button_7_click);
        number_button_handle=create_widget(telephone_window_handle,&number_button_8);
        set_button_caption(telephone_window_handle,number_button_handle,"8");
        set_button_click_recall(telephone_window_handle,number_button_handle,&number_button_8_click);
        number_button_handle=create_widget(telephone_window_handle,&number_button_9);
        set_button_caption(telephone_window_handle,number_button_handle,"9");
        set_button_click_recall(telephone_window_handle,number_button_handle,&number_button_9_click);
        number_button_handle=create_widget(telephone_window_handle,&number_button_0);
        set_button_caption(telephone_window_handle,number_button_handle,"0");
        set_button_click_recall(telephone_window_handle,number_button_handle,&number_button_0_click);
        number_button_handle=create_widget(telephone_window_handle,&call_button);
        set_button_caption(telephone_window_handle,number_button_handle,"call");
        set_button_click_recall(telephone_window_handle,number_button_handle,&call_button_click);
        number_button_handle=create_widget(telephone_window_handle,&back_button);
        set_button_caption(telephone_window_handle,number_button_handle,"<-");
        set_button_click_recall(telephone_window_handle,number_button_handle,&back_button_click);
        memset(call_phone_number_buffer,0,PHONE_NUMBER_LENGTH);
        refresh_window(telephone_window_handle);
        return true;
    }
    return false;
}

static void telephone_exit(void) {
}

//  telephone main window

window_handle telephone_dial_window_handle=INVALID_WINDOW_HANDLE;
widget_handle ring_button_handle=INVALID_WIDGET_HANDLE;

static void telephone_dial_exit(void);

static void telephone_dial_window_notice_recall(window_handle recall_window,notice_flag in_notice_flag,long parameter1,long parameter2) {
    switch (in_notice_flag) {
        case notice_click:
            break;
        case notice_close:
            telephone_dial_window_handle=INVALID_WINDOW_HANDLE;
            telephone_dial_exit();
    }
}

static void telephone_dial_button_ring_click(void) {
    beep_off();
    gsm_answer();
    delete_widget(telephone_dial_window_handle,ring_button_handle);
    ring_button_handle=INVALID_WIDGET_HANDLE;
}

static void telephone_dial_button_hang_click(void) {
    gsm_handup();
    close_window(telephone_dial_window_handle);
    telephone_dial_window_handle=INVALID_WINDOW_HANDLE;
}

void telephone_dial_init(const char* phone_number,telephone_dial_state show_state) {
    window_widget phone_number_label;
    window_widget ring_button;
    window_widget hang_button;
    widget_handle phone_label_handle=INVALID_WIDGET_HANDLE;
    widget_handle button_handle=INVALID_WIDGET_HANDLE;
    
    switch (show_state) {
        case dial_call:
            telephone_dial_window_handle=create_window("telephone_dial","dial calling",false,*telephone_dial_window_notice_recall);
            break;
        case dial_ring:
            beep_on();
            telephone_dial_window_handle=create_window("telephone_dial","dial ring",false,*telephone_dial_window_notice_recall);
            break;
        case dial_hang:
            beep_off();
            close_window(telephone_dial_window_handle);
        default:
            return;
    }
    if (INVALID_WINDOW_HANDLE!=telephone_dial_window_handle) {
        show_window(telephone_dial_window_handle);
        phone_number_label.widget_type=widget_label;
        phone_number_label.x=PHONE_NUMBER_LABEL_X;
        phone_number_label.y=PHONE_NUMBER_LABEL_Y;
        phone_number_label.widget_height=PHONE_NUMBER_LABEL_HEIGHT;
        phone_number_label.widget_width=PHONE_NUMBER_LABEL_WIDET;
        ring_button.widget_type=widget_button;
        ring_button.x=CALL_BUTTON_X;
        ring_button.y=CALL_BUTTON_Y;
        ring_button.widget_height=NUNBER_BUTTON_HEIGHT;
        ring_button.widget_width=NUNBER_BUTTON_WIDTH;
        hang_button.widget_type=widget_button;
        hang_button.x=BACK_BUTTON_X;
        hang_button.y=BACK_BUTTON_Y;
        hang_button.widget_height=NUNBER_BUTTON_HEIGHT;
        hang_button.widget_width=NUNBER_BUTTON_WIDTH;
        phone_label_handle=create_widget(telephone_dial_window_handle,&phone_number_label);
        set_label_caption(telephone_dial_window_handle,phone_label_handle,phone_number);
        if (dial_ring==show_state) {
            button_handle=create_widget(telephone_dial_window_handle,&ring_button);
            set_button_caption(telephone_dial_window_handle,button_handle,"Ring");
            set_button_click_recall(telephone_dial_window_handle,button_handle,&telephone_dial_button_ring_click);
            ring_button_handle=button_handle;
        }
        button_handle=create_widget(telephone_dial_window_handle,&hang_button);
        set_button_caption(telephone_dial_window_handle,button_handle,"Hang");
        set_button_click_recall(telephone_dial_window_handle,button_handle,&telephone_dial_button_hang_click);
        refresh_window(telephone_dial_window_handle);
    }
}

static void telephone_dial_exit(void) {
}
