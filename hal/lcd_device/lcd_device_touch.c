/*******************************************************************************
 * @file    : touch.c
 * @author  : leon
 * @web     : www.ucortex.com
 * @version : V1.0
 * @date    : 2013-05-02
 * @brief   : 触摸屏底层驱动
 * ---------------------------------------------------------------------------- 
 *
 *                                重要的声明
 *
 * 本文件所涉及的内容是免费提供，请各位用于研究学习，禁止用于商业目的，学习结束
 * 后，请在24小时之内删除。文件内部分代码引用于网上开源的例程，部分代码经过改写。
 * 对于因为参考本文件内容，导致您的产品直接或间接受到破坏，或涉及到法律问题，作
 * 者不承担任何责任。对于使用过程中发现的问题，如果您能在www.ucortex.com网站反馈
 * 给我们，我们会非常感谢，并将对问题及时研究并作出改善。例程的版本更新，将不做
 * 特别通知，请您自行到www.ucortex.com下载最新版本，谢谢。
 * 对于以上声明，UCORTEX保留进一步解释的权利！
 * ----------------------------------------------------------------------------
 * 说明:
 * T_SCK  -> PB1
 * T_CS   -> PB2
 * T_MISO -> PF8
 * T_MOSI -> PF9
 * T_PEN  -> PF10
 *-----------------------------------------------------------------------------
 *                                 更改记录
 * ----------------------------------------------------------------------------
 * 更改时间：2013-05-02    更改人：Leon
 * 版本记录：V1.0
 * 更改内容：新建
 * ----------------------------------------------------------------------------
 *
 ******************************************************************************/
#include "..//system_config.h"

#include "lcd_device_touch.h" 
#include "lcd_device_api.h"
#include "delay.h"
#include "stdlib.h"
#include "math.h"
#include "24cxx.h"

_m_tp_dev tp_dev=
{
	TP_Init,
	TP_Scan,
	TP_Adjust,
	0,
	0,
 	0,
	0,
	0,
	0,
	0,
	0,	  	 		
	0,
	0,	  	 		
};					
//默认为touchtype=0的数据.
u8 CMD_RDX=0XD0;
u8 CMD_RDY=0X90;
 	 			    					   
/**
  * @brief: SPI写数据,向触摸屏IC写入1byte数据    
  * @param: num:要写入的数据
  */
void TP_Write_Byte(u8 num)    
{  
	u8 count=0;   
	for(count=0;count<8;count++)  
	{ 	  
		if(num&0x80)TDIN=1;  
		else TDIN=0;   
		num<<=1;    
		TCLK=0; 	 
		TCLK=1;		//上升沿有效	        
	}		 			    
} 		 
/**
  * @brief: SPI读数据,从触摸屏IC读取adc值
  * @param: CMD:指令
  * @retval:读到的数据
  */
u16 TP_Read_AD(u8 CMD)	  
{ 	 
	u8 count=0; 	  
	u16 Num=0; 
	TCLK=0;		//先拉低时钟 	 
	TDIN=0; 	//拉低数据线
	TCS=0; 		//选中触摸屏IC
	TP_Write_Byte(CMD);//发送命令字
	delay_us(6);//ADS7846的转换时间最长为6us
	TCLK=0; 	     	    
	delay_us(1);    	   
	TCLK=1;		//给1个时钟，清除BUSY	    	    
	TCLK=0; 	     	    
	for(count=0;count<16;count++)//读出16位数据,只有高12位有效 
	{ 				  
		Num<<=1; 	 
		TCLK=0;	//下降沿有效  	    	   
		TCLK=1;
		if(DOUT)Num++; 		 
	}  	
	Num>>=4;   	//只有高12位有效.
	TCS=1;		//释放片选	 
	return(Num);   
}
/**
  * @brief: 读取一个坐标值(x或者y),连续读取READ_TIMES次数据,对这些数据升序排列,
  *         然后去掉最低和最高LOST_VAL个数,取平均值 
  * @param: xy:指令（CMD_RDX/CMD_RDY）
  * @retval:读到的数据
  */
#define READ_TIMES 5		//读取次数
#define LOST_VAL 1			//丢弃值
u16 TP_Read_XOY(u8 xy)
{
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum=0;
	u16 temp;
	for(i=0;i<READ_TIMES;i++)buf[i]=TP_Read_AD(xy);		 		    
	for(i=0;i<READ_TIMES-1; i++)//排序
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//升序排列
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
}

/**
  * @brief: 读取x,y坐标，最小值不能少于100.
  * @param: x,y:读取到的坐标值
  * @retval:0,失败;1,成功。
  */
u8 TP_Read_XY(u16 *x,u16 *y)
{
	u16 xtemp,ytemp;			 	 		  
	xtemp=TP_Read_XOY(CMD_RDX);
	ytemp=TP_Read_XOY(CMD_RDY);	  												   
	//if(xtemp<100||ytemp<100)return 0;//读数失败
	*x=xtemp;
	*y=ytemp;
	return 1;//读数成功
}
/**
  * @brief: 连续2次读取触摸屏IC,且这两次的偏差不能超过,该函数能大大提高准确度
  * @param: x,y:读取到的坐标值
  * @retval:0,失败;1,成功。
  * @note:
  *   - ERR_RANGE,满足条件,则认为读数正确,否则读数错误.	
  */
#define ERR_RANGE 50 //误差范围 
u8 TP_Read_XY2(u16 *x,u16 *y) 
{
	u16 x1,y1;
 	u16 x2,y2;
 	u8 flag;    
    flag=TP_Read_XY(&x1,&y1);   
    if(flag==0)return(0);
    flag=TP_Read_XY(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//前后两次采样在+-50内
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else return 0;	  
}  

/**
  * @brief: 画一个触摸点, 用来校准用的
  * @param: x,y:坐标
  * @param: color:颜色
  * @retval: none
  */
void TP_Drow_Touch_Point(u16 x,u16 y,u16 color)
{
	POINT_COLOR=color;
	LCD_DrawLine(x-12,y,x+13,y);//横线
	LCD_DrawLine(x,y-12,x,y+13);//竖线
	LCD_DrawPoint(x+1,y+1);
	LCD_DrawPoint(x-1,y+1);
	LCD_DrawPoint(x+1,y-1);
	LCD_DrawPoint(x-1,y-1);
	Draw_Circle(x,y,6);//画中心圈
}	  
/**
  * @brief: 画一个大点(2*2的点)		   
  * @param: x,y:坐标
  * @param: color:颜色
  * @retval: none
  */
void TP_Draw_Big_Point(u16 x,u16 y,u16 color)
{	    
	POINT_COLOR=color;
	LCD_DrawPoint(x,y);//中心点 
	LCD_DrawPoint(x+1,y);
	LCD_DrawPoint(x,y+1);
	LCD_DrawPoint(x+1,y+1);	 	  	
}						  
  
/**
  * @brief: 触摸按键扫描
  * @param: tp:0,屏幕坐标;1,物理坐标(校准等特殊场合用)
  * @retval:当前触屏状态, 0,触屏无触摸;1,触屏有触摸
  */
u8 TP_Scan(u8 tp)
{			   
	if(PEN==0)//有按键按下
	{
		if(tp)TP_Read_XY2(&tp_dev.x,&tp_dev.y);//读取物理坐标
		else if(TP_Read_XY2(&tp_dev.x,&tp_dev.y))//读取屏幕坐标
		{
	 		tp_dev.x=tp_dev.xfac*tp_dev.x+tp_dev.xoff;//将结果转换为屏幕坐标
			tp_dev.y=tp_dev.yfac*tp_dev.y+tp_dev.yoff;  
	 	} 
		if((tp_dev.sta&TP_PRES_DOWN)==0)//之前没有被按下
		{		 
			tp_dev.sta=TP_PRES_DOWN|TP_CATH_PRES;//按键按下  
			tp_dev.x0=tp_dev.x;//记录第一次按下时的坐标
			tp_dev.y0=tp_dev.y;  	   			 
		}			   
	}else
	{
		if(tp_dev.sta&TP_PRES_DOWN)//之前是被按下的
		{
			tp_dev.sta&=~(1<<7);//标记按键松开	
		}else//之前就没有被按下
		{
			tp_dev.x0=0;
			tp_dev.y0=0;
			tp_dev.x=0xffff;
			tp_dev.y=0xffff;
		}	    
	}
	return tp_dev.sta&TP_PRES_DOWN;//返回当前的触屏状态
}	  

//保存在EEPROM里面的地址区间基址,占用13个字节(RANGE:SAVE_ADDR_BASE~SAVE_ADDR_BASE+12)
#define SAVE_ADDR_BASE 40
/**
  * @brief: 保存校准参数
  */
void TP_Save_Adjdata(void)
{
	s32 temp;			 
	//保存校正结果!		   							  
	temp=tp_dev.xfac*100000000;//保存x校正因素      
	AT24CXX_WriteLenByte(SAVE_ADDR_BASE,temp,4);   
	temp=tp_dev.yfac*100000000;//保存y校正因素    
	AT24CXX_WriteLenByte(SAVE_ADDR_BASE+4,temp,4);
	//保存x偏移量
	AT24CXX_WriteLenByte(SAVE_ADDR_BASE+8,tp_dev.xoff,2);		    
	//保存y偏移量
	AT24CXX_WriteLenByte(SAVE_ADDR_BASE+10,tp_dev.yoff,2);	
	//保存触屏类型
	AT24CXX_WriteOneByte(SAVE_ADDR_BASE+12,tp_dev.touchtype);	
	temp=0X0A;//标记校准过了
	AT24CXX_WriteOneByte(SAVE_ADDR_BASE+13,temp); 
}
/**
  * @brief: 得到保存在EEPROM里面的校准值
  * @retval: 1，成功获取数据
  *          0，获取失败，要重新校准
  */
u8 TP_Get_Adjdata(void)
{					  
	s32 tempfac;
	tempfac=AT24CXX_ReadOneByte(SAVE_ADDR_BASE+13);						//读取标记字,看是否校准过！ 		 
	if(tempfac==0X0A)																					//触摸屏已经校准过了			   
	{    												 
		tempfac=AT24CXX_ReadLenByte(SAVE_ADDR_BASE,4);					//得到x校准参数
		tp_dev.xfac=(float)tempfac/100000000;										
		tempfac=AT24CXX_ReadLenByte(SAVE_ADDR_BASE+4,4);				//得到y校准参数		          
		tp_dev.yfac=(float)tempfac/100000000;										
		tp_dev.xoff=AT24CXX_ReadLenByte(SAVE_ADDR_BASE+8,2);		//得到x偏移量		   	  
		tp_dev.yoff=AT24CXX_ReadLenByte(SAVE_ADDR_BASE+10,2);		//得到y偏移量
 		tp_dev.touchtype=AT24CXX_ReadOneByte(SAVE_ADDR_BASE+12);//读取触屏类型标记
		if(tp_dev.touchtype)																		//X,Y方向与屏幕相反
		{
			CMD_RDX=0X90;
			CMD_RDY=0XD0;	 
		}
		else																										//X,Y方向与屏幕相同
		{
			CMD_RDX=0XD0;
			CMD_RDY=0X90;	 
		}		 
		return 1;	 
	}
	return 0;
}
//提示字符串
const char* TP_REMIND_MSG_TBL="Please use the stylus click the cross on the screen.The cross will always move until the screen adjustment is completed.";
 					  
/**
  * @brief: 提示校准结果(各个参数)
  */
void TP_Adj_Info_Show(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u16 x3,u16 y3,u16 fac)
{	  
	POINT_COLOR=RED;
	LCD_ShowString(40,160,lcddev.width,lcddev.height,16,"x1:");
 	LCD_ShowString(40+80,160,lcddev.width,lcddev.height,16,"y1:");
 	LCD_ShowString(40,180,lcddev.width,lcddev.height,16,"x2:");
 	LCD_ShowString(40+80,180,lcddev.width,lcddev.height,16,"y2:");
	LCD_ShowString(40,200,lcddev.width,lcddev.height,16,"x3:");
 	LCD_ShowString(40+80,200,lcddev.width,lcddev.height,16,"y3:");
	LCD_ShowString(40,220,lcddev.width,lcddev.height,16,"x4:");
 	LCD_ShowString(40+80,220,lcddev.width,lcddev.height,16,"y4:");  
 	LCD_ShowString(40,240,lcddev.width,lcddev.height,16,"fac is:");     
	LCD_ShowNum(40+24,160,x0,4,16);			//显示数值
	LCD_ShowNum(40+24+80,160,y0,4,16);	//显示数值
	LCD_ShowNum(40+24,180,x1,4,16);			//显示数值
	LCD_ShowNum(40+24+80,180,y1,4,16);	//显示数值
	LCD_ShowNum(40+24,200,x2,4,16);			//显示数值
	LCD_ShowNum(40+24+80,200,y2,4,16);	//显示数值
	LCD_ShowNum(40+24,220,x3,4,16);			//显示数值
	LCD_ShowNum(40+24+80,220,y3,4,16);	//显示数值
 	LCD_ShowNum(40+56,lcddev.width,fac,3,16);	//显示数值,该数值必须在95~105范围之内.
}
		 
/**
  * @brief: 触摸屏校准代码,得到四个校准参数
  */
void TP_Adjust(void)
{								 
	u16 pos_temp[4][2];	//坐标缓存值
	u8  cnt=0;	
	u16 d1,d2;
	u32 tem1,tem2;
	float fac; 	
	u16 outtime=0;
 	cnt=0;				
	POINT_COLOR=BLUE;
	BACK_COLOR =WHITE;
	LCD_Clear(WHITE);		//清屏   
	POINT_COLOR=RED;		//红色 
	LCD_Clear(WHITE);		//清屏 	   
	POINT_COLOR=BLACK;
	LCD_ShowString(40,40,160,100,16,TP_REMIND_MSG_TBL);//显示提示信息
	TP_Drow_Touch_Point(20,20,RED);	//画点1 
	tp_dev.sta=0;		//消除触发信号 
	tp_dev.xfac=0;	//xfac用来标记是否校准过,所以校准之前必须清掉!以免错误	 
	while(1)				//如果连续10秒钟没有按下,则自动退出
	{
		tp_dev.scan(1);											//扫描物理坐标
		if((tp_dev.sta&0xc0)==TP_CATH_PRES)	//按键按下了一次(此时按键松开了)
		{	
			outtime=0;		
			tp_dev.sta&=~(1<<6);							//标记按键已经被处理过了.
						   			   
			pos_temp[cnt][0]=tp_dev.x;
			pos_temp[cnt][1]=tp_dev.y;
			cnt++;	  
			switch(cnt)
			{			   
				case 1:						 
					TP_Drow_Touch_Point(20,20,WHITE);							//清除点1 
					TP_Drow_Touch_Point(lcddev.width-20,20,RED);	//画点2
					break;
				case 2:
 					TP_Drow_Touch_Point(lcddev.width-20,20,WHITE);//清除点2
					TP_Drow_Touch_Point(20,lcddev.height-20,RED);	//画点3
					break;
				case 3:
 					TP_Drow_Touch_Point(20,lcddev.height-20,WHITE);	//清除点3
 					TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,RED);	//画点4
					break;
				case 4:	//全部四个点已经得到
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,2的距离
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到3,4的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05||d1==0||d2==0)//不合格
					{
						cnt=0;
						TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//清除点4
						TP_Drow_Touch_Point(20,20,RED);	//画点1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
 						continue;
					}
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,3的距离
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到2,4的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//不合格
					{
						cnt=0;
						TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//清除点4
						TP_Drow_Touch_Point(20,20,RED);	//画点1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
						continue;
					}//正确了
								   
					//对角线相等
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,4的距离
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到2,3的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//不合格
					{
						cnt=0;
						TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//清除点4
						TP_Drow_Touch_Point(20,20,RED);	//画点1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
						continue;
					}//正确了
					//计算结果
					tp_dev.xfac=(float)(lcddev.width-40)/(pos_temp[1][0]-pos_temp[0][0]);			//得到xfac		 
					tp_dev.xoff=(lcddev.width-tp_dev.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;	//得到xoff
						  
					tp_dev.yfac=(float)(lcddev.height-40)/(pos_temp[2][1]-pos_temp[0][1]);		//得到yfac
					tp_dev.yoff=(lcddev.height-tp_dev.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//得到yoff  
					if(abs(tp_dev.xfac)>2||abs(tp_dev.yfac)>2)//触屏和预设的相反了.
					{
						cnt=0;
						TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//清除点4
						TP_Drow_Touch_Point(20,20,RED);	//画点1
						LCD_ShowString(40,26,lcddev.width,lcddev.height,16,"TP Need readjust!");
						tp_dev.touchtype=!tp_dev.touchtype;//修改触屏类型.
						if(tp_dev.touchtype)//X,Y方向与屏幕相反
						{
							CMD_RDX=0X90;
							CMD_RDY=0XD0;	 
						}
						else	//X,Y方向与屏幕相同
						{
							CMD_RDX=0XD0;
							CMD_RDY=0X90;	 
						}			    
						continue;
					}		
					POINT_COLOR=BLUE;
					LCD_Clear(WHITE);//清屏
					LCD_ShowString(35,110,lcddev.width,lcddev.height,16,"Touch Screen Adjust OK!");//校正完成
					delay_ms(1000);
					TP_Save_Adjdata();  
 					LCD_Clear(WHITE);//清屏   
					return;//校正完成				 
			}
		}
		delay_ms(10);
		outtime++;
		if(outtime>1000)
		{
			TP_Get_Adjdata();
			break;
	 	} 
 	}
}		  
/**
  * @brief: 触摸屏初始化  		    
  * @retval:0 - 没有进行校准
  *         1 - 进行过校准
  * @note:
  *   - T_SCK  -> PB1
  *   - T_CS   -> PB2
  *   - T_MISO -> PF8
  *   - T_MOSI -> PF9
  *   - T_PEN  -> PF10
  */
u8 TP_Init(void)
{			    		   
	RCC->APB2ENR|=1<<3;			//PB时钟使能	   
	RCC->APB2ENR|=1<<7;			//PF时钟使能	   
 	
	GPIOB->CRL&=0XFFFFF00F;	//PB1 PB2
	GPIOB->CRL|=0X00000330; 
	GPIOB->ODR|=3<<1;				//PB1 PB2 推挽输出 	 
	GPIOF->CRH&=0XFFFFF000;
	GPIOF->CRH|=0X00000838; //PF8-上拉输入  PF9-推挽输出  PF10-上拉输入
	GPIOF->ODR|=7<<8;				//PF8 PF9 PF10 全部上拉	  
	TP_Read_XY(&tp_dev.x,&tp_dev.y);//第一次读取初始化
 	IIC_Init();							//初始化IIC
 	while(AT24CXX_Check())	//检测不到24c02
	{
		LCD_ShowString(16,150,200,16,16,"24C02 Check Failed!");
		delay_ms(500);
		LCD_ShowString(16,150,200,16,16,"Please Check!      ");
		delay_ms(500);
	}
	if(TP_Get_Adjdata())return 0;	//已经校准
	else													//未校准
	{ 										    
		LCD_Clear(WHITE);						//清屏
		TP_Adjust();								//屏幕校准 
		TP_Save_Adjdata();	 
	}
	TP_Get_Adjdata();	
	return 1; 									 
}

