
#ifndef _GRAPHICS_BITMAP_H__
#define _GRAPHICS_BITMAP_H__

#include "graphics_api.h"

typedef struct {
unsigned short type;      //位图文件的类型，必须为BM(1-2字节）
unsigned long  size;      //位图文件的大小，以字节为单位（3-6字节，低位在前）
unsigned short reserved1; //位图文件保留字，必须为0(7-8字节）
unsigned short reserved2; //位图文件保留字，必须为0(9-10字节）
unsigned long  offset;    //位图数据的起始位置，以相对于位图（11-14字节，低位在前）
//文件头的偏移量表示，以字节为单位
} bitmap_header,*point_bitmap_header;

#define BITMAP_HEADER_LENGTH 14

typedef struct {
unsigned long size;                  //本结构所占用字节数（15-18字节）
unsigned long width;                 //位图的宽度，以像素为单位（19-22字节）
unsigned long height;                //位图的高度，以像素为单位（23-26字节）
unsigned short planes;               //目标设备的级别，必须为1(27-28字节）
unsigned short bit_count;            //每个像素所需的位数，必须是1（双色），（29-30字节）
//4(16色），8(256色）16(高彩色)或24（真彩色）之一
unsigned long compression;           //位图压缩类型，必须是0（不压缩），（31-34字节）
//0(BI_RGB)原色 1(BI_RLE8压缩类型）或2(BI_RLE4压缩类型）之一
unsigned long size_image;            //位图的大小(其中包含了为了补齐行数是4的倍数而添加的空字节)，以字节为单位（35-38字节）
unsigned long x_pelsper_meter;       //位图水平分辨率，每米像素数（39-42字节）
unsigned long y_pelsper_meter;       //位图垂直分辨率，每米像素数（43-46字节)
unsigned long clr_used;              //位图实际使用的颜色表中的颜色数（47-50字节）
unsigned long clr_important;         //位图显示过程中重要的颜色数（51-54字节）
} bitmap_information,*point_bitmap_information;

#define BITMAP_INFORMATION_LENGTH 40

typedef struct {
char blue;     //蓝色的亮度（值范围为0-255)
char green;    //绿色的亮度（值范围为0-255)
char red;      //红色的亮度（值范围为0-255)
char reserved; //保留，必须为0
} rgb_quad,*point_rgb_quad;

#define RBG_QUAD_LENGTH 4

void graphics_draw_bitmap(bitmap_data bitmap_data,graphics_location draw_point_x,graphics_location draw_point_y);
void graphics_get_bitmap (bitmap_data bitmap_data,unsigned long* bitmap_size,unsigned long* bitmap_height,unsigned long* bitmap_width);

#endif






