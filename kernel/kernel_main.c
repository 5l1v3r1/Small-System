
#include "system_type.h"
#include "kernel_config.h"

#include "led_api.h"
#include "lcd_api.h"
#include "key_api.h"
#include "delay.h"

#include "kernel_notice.h"
#include "kernel_ui_api.h"
#include "malloc.h"
#include "file_api.h"
#include "thread_main.h"
#include "graphics_api.h"
#include "kernel_expand_gsm.h"

#ifdef  SYSTEM_DESKTOP
#include "system_desktop.h"
#endif

#ifdef  APPLICATION_ALARM
#include "alarm_ui.h"
#endif

#define SYSTEM_ERR     -1
#define SYSTEM_SUCCESS  0

int kernel_main(bool system_main_init_stat) {
    if (system_main_init_stat) {
        graphics_init();
        mem_init(SRAMIN);
        mem_init(SRAMEX);
        kernel_load_enter();

        delay_ms(500);
        kernel_set_load_rate(40);
        
        kernel_notice_init();
        delay_ms(500);
        kernel_set_load_rate(60);
        
        gsm_init();
        delay_ms(500);
        kernel_set_load_rate(80);
        
        file_system_init();
        delay_ms(1000);
        kernel_set_load_rate(100);
        kernel_load_exit();
        
#ifdef SYSTEM_DESKTOP
        system_desktop_init();
#endif
        thread_main();
        
        return SYSTEM_SUCCESS;
    }
    kernel_err_report("Kernel Init Error!","That is an error about hardware init!","Plaese restart the chip or exchange some device on PCB.if you see the report again,you should exchange hardware device..");
    
    while (!key_stat(KEY_UP));
    return SYSTEM_ERR;
}
