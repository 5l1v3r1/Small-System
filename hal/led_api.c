
#include "system_config.h"
#include "system_type.h"

#include "led_api.h"

#define LED_ON_ELECT_LOW  0
#define LED_ON_ELECT_HIGE 1
#define LED_ON_ELECT      LED_ON_ELECT_LOW  //  设置LED 是以高电平还是低电平的方式发光(1 为高电平)

#define LED_ON         1
#define LED_OFF        0
#define LED_INIT_STAT  LED_OFF  //  设置LED 初始化后的状态

#define LED_POWER
#ifdef  LED_POWER
#define LED_PORT_POWER GPIOB
#define LED_PIN_POWER  GPIO_Pin_5
#endif
#define LED_WORK
#ifdef  LED_WORK
#define LED_PORT_WORK  GPIOE
#define LED_PIN_WORK   GPIO_Pin_5
#endif

#ifdef  LED_POWER
void led_power_on(void) {
#if LED_ON_ELECT==LED_ON_ELECT_LOW
    GPIO_ResetBits(LED_PORT_POWER,LED_PIN_POWER);
#else
    GPIO_SetBits(LED_PORT_POWER,LED_PIN_POWER);
#endif
}

void led_power_off(void) {
#if LED_ON_ELECT==LED_ON_ELECT_LOW
    GPIO_SetBits(LED_PORT_POWER,LED_PIN_POWER);
#else
    GPIO_ResetBits(LED_PORT_POWER,LED_PIN_POWER);
#endif
}
#endif

#ifdef  LED_WORK
void led_work_on(void) {
#if LED_ON_ELECT==LED_ON_ELECT_LOW
    GPIO_ResetBits(LED_PORT_WORK,LED_PIN_WORK);
#else
    GPIO_SetBits(LED_PORT_WORK,LED_PIN_WORK);
#endif
}

void led_work_off(void) {
#if LED_ON_ELECT==LED_ON_ELECT_LOW
    GPIO_SetBits(LED_PORT_WORK,LED_PIN_WORK);
#else
    GPIO_ResetBits(LED_PORT_WORK,LED_PIN_WORK);
#endif
}
#endif

void led_init(void) {
#ifdef LED_POWER
    GPIO_InitTypeDef led_power;
#endif
#ifdef LED_WORK
    GPIO_InitTypeDef led_work;
#endif
    
#ifdef LED_POWER
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    
    led_power.GPIO_Pin   =LED_PIN_POWER;
    led_power.GPIO_Speed =GPIO_Speed_2MHz;
    led_power.GPIO_Mode  =GPIO_Mode_Out_OD;
    GPIO_Init(LED_PORT_POWER,&led_power);
#if LED_INIT_STAT==LED_ON
    led_power_on();
#else
    led_power_off();
#endif
#endif
    
#ifdef LED_WORK
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
    
    led_work.GPIO_Pin   =LED_PIN_WORK;
    led_work.GPIO_Mode  =GPIO_Mode_Out_OD;
    led_work.GPIO_Speed =GPIO_Speed_2MHz;
    GPIO_Init(LED_PORT_WORK,&led_work);
#if LED_INIT_STAT==LED_ON
    led_work_on();
#else
    led_work_off();
#endif
#endif
}
