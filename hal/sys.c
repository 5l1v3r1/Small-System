
#include "stm32f10x.h"
#include "stm32f10x_nvic.h"

#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK Mini STM32开发板
//系统中断分组设置化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/10
//版本：V1.4
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
//********************************************************************************  
void NVIC_Configuration(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级

}
/*
 * ????NVIC??
 * PriGroup: ????(0~7)
 */
void NVIC_PrioGrpCfg(u8 NVIC_Group)
{
  u32 temp1,temp2;

  temp1 = NVIC_Group&0x07;//??????3?
  temp1 <<= 8;
  temp2 = SCB->AIRCR;     //???????
  temp2 &= 0x0000f8ff;    //????????
  temp2 |= temp1;         //????????
  temp2 |= 0x05fa0000;    //?????0x05fa
  SCB->AIRCR = temp2;     //???????
}
void NVIC_Init_(u8 NVIC_GroupPrio,u8 NVIC_SubPrio,u8 NVIC_Channel,u8 NVIC_Group)  
{
  u8 index, offset;
  u8 *pNvicPri;
  u8 *pIser;
  u8 pri;
  
  if(NVIC_Group <= 3) NVIC_Group = 3;
    
  pNvicPri = (u8*)NVIC->IP; 
  pIser = (u8 *)NVIC->ISER;
  
  NVIC_PrioGrpCfg(NVIC_Group);
  
  offset = NVIC_Group-3;
  pri =  (NVIC_GroupPrio&0xf)<<offset;
  pri |= (NVIC_SubPrio&0x0f);
  pri <<= 4;
  *(pNvicPri+NVIC_Channel) =  pri;
  
  index = NVIC_Channel/8; 
  offset = NVIC_Channel%8;
  *(pIser+index) |= 1<<offset;
}
