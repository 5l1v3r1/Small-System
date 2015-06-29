
#include "system_config.h"
#include "system_type.h"
#include "system_interrupt.h"

#include "key_api.h"

#define KEY_PORT_UP     GPIOA
#define KEY_PORT_DOWN   GPIOE
#define KEY_PORT_LEFT   GPIOE
#define KEY_PORT_RIGHT  GPIOE

#define KEY_PIN_UP      GPIO_Pin_0
#define KEY_PIN_DOWN    GPIO_Pin_3
#define KEY_PIN_LEFT    GPIO_Pin_4
#define KEY_PIN_RIGHT   GPIO_Pin_2

#define KEY_COUNT       4

bool key_stat(key_code in_key_code) {
    switch (in_key_code) {
        case KEY_NO:
            return false;
        case KEY_UP:
            return GPIO_ReadInputDataBit(KEY_PORT_UP,KEY_PIN_UP)?false:true;
        case KEY_DOWN:
            return GPIO_ReadInputDataBit(KEY_PORT_DOWN,KEY_PIN_DOWN)?false:true;
        case KEY_LEFT:
            return GPIO_ReadInputDataBit(KEY_PORT_LEFT,KEY_PIN_LEFT)?false:true;
        case KEY_RIGHT:
            return GPIO_ReadInputDataBit(KEY_PORT_RIGHT,KEY_PIN_RIGHT)?false:true;
    }
    return false;
}
/*
void key_interrupt_init(void) {
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource2);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line2;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);
    
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource3);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line3;
  	EXTI_Init(&EXTI_InitStructure);

  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource4);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
  	EXTI_Init(&EXTI_InitStructure);

 	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
 	EXTI_InitStructure.EXTI_Line=EXTI_Line0;
  	EXTI_Init(&EXTI_InitStructure);


  	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure); 

    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);


  	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
}
*/
void key_init(void) {
    GPIO_InitTypeDef key;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);

    key.GPIO_Mode=GPIO_Mode_IPD;
    key.GPIO_Pin=KEY_PIN_UP;
    GPIO_Init(KEY_PORT_UP,&key);
    
    key.GPIO_Mode=GPIO_Mode_IPU;
    key.GPIO_Pin=KEY_PIN_DOWN|KEY_PIN_LEFT|KEY_PIN_RIGHT;
    GPIO_Init(KEY_PORT_DOWN,&key);
    
//    key_interrupt_init();
}
/*
void EXTI0_IRQHandler(void) {
	delay_ms(10);
    key_interrupt(KEY_UP,true);
    while (key_stat(KEY_UP))
        delay_ms(10);
    key_interrupt(KEY_UP,false);
	EXTI_ClearITPendingBit(EXTI_Line0);
}
void EXTI2_IRQHandler(void) {
	delay_ms(10);
    key_interrupt(KEY_RIGHT,true);
    while (key_stat(KEY_RIGHT))
        delay_ms(10);
    key_interrupt(KEY_RIGHT,false);
	EXTI_ClearITPendingBit(EXTI_Line2);
}
void EXTI3_IRQHandler(void) {
	delay_ms(10);
    key_interrupt(KEY_DOWN,true);
    while (key_stat(KEY_DOWN))
        delay_ms(10);
    key_interrupt(KEY_DOWN,false);
	EXTI_ClearITPendingBit(EXTI_Line3);
}
void EXTI4_IRQHandler(void) {
	delay_ms(10);
    key_interrupt(KEY_LEFT,true);
    while (key_stat(KEY_LEFT))
        delay_ms(10);
    key_interrupt(KEY_LEFT,false);
	EXTI_ClearITPendingBit(EXTI_Line4);
}
*/
