
#include "stm32f10x.h"
#include "stm32f10x_nvic.h"

#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK Mini STM32������
//ϵͳ�жϷ������û�		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/10
//�汾��V1.4
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//********************************************************************************  
void NVIC_Configuration(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�

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
