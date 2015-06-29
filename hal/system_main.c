
#include "system_type.h"
#include "system_config.h"
#include "kernel_config.h"

#include "led_api.h"
#include "beep_api.h"
#include "lcd_api.h"
#include "key_api.h"
#include "uart_api.h"
#include "sram_api.h"
#include "time_api.h"
#include "flash_inside_api.h"
#include "flash_outside_api.h"
#include "delay.h"
#include "sys.h"

static void system_init(void) {
    delay_init();
    led_init();
    beep_init();
    key_init();
    uart_init();
    lcd_init();
    sram_init();
    time_init();
    flash_inside_init();
    flash_outside_init();
    
    led_power_on();
    lcd_on();
}

static void system_exit(void) {
    led_power_off();
}

int main(void) {
    extern int kernel_main(bool system_main_init_stat);  //  只能在main() 内使用
    
    system_init();
    kernel_main(true);
    system_exit();
}
