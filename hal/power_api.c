
#include "system_config.h"
#include "system_type.h"

#include "power_api.h"

bool power_stat=true;

void power_init(void) {
    // init port ..
    
    /*
    
        外部电路还没有设置,原理是这样的:
        利用一个寄存器加AND 运算器.[out 系统电源,in1 供电电源,in2 是否需要供电]
        在上电时给寄存器一边置高,AND 就可以流电流,关机的时候把寄存器置低,AND 就没电流了..
    
    */
    
}

void power_shutdown(void) {
    if (power_stat) {
        power_stat=false;
        
        //  控制外部电流
        
    }
}
