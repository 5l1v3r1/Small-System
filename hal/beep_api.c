
#include "system_config.h"

#include "beep_api.h"

void beep_on(void) {
    GPIO_SetBits(GPIOB,GPIO_Pin_8);
}

void beep_off(void) {
    GPIO_ResetBits(GPIOB,GPIO_Pin_8);
}

void beep_init(void) {
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    beep_off();
}
