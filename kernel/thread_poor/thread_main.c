
#include "system_type.h"
#include "kernel_config.h"

#include "power_api.h"
#include "led_api.h"
#include "lcd_api.h"
#include "key_api.h"
#include "rtc_device//rtc_device_api.h"
#include "delay.h"

#ifdef SYSTEM_DESKTOP
#include "graphics_window.h"
#endif

#include "thread_main.h"
#include "kernel_notice.h"

bool kernel_power=true;

void kernel_exit(void) {
    kernel_power=false;
    power_shutdown();
}

int thread_main(void) {
    led_work_on();
    while (kernel_power) {
#ifdef SYSTEM_DESKTOP
        if (!lcd_display_state()) {
            if (key_stat(KEY_DOWN)) {
                reset_wait_time();
                refresh_window(get_window_front());
            }
            continue;
        }
        if (lcd_is_touch()) {
            delay_ms(50);
            hardware_notice(hardware_notice_lcdtouch_interrupt,lcd_touch_x(),lcd_touch_y());
            reset_wait_time();
            while (lcd_is_touch());
        }
#endif
    }
    return THREAD_MAIN_ERROR;
}
