
#ifndef _GRAPHICS_BITMAP_H__
#define _GRAPHICS_BITMAP_H__

#include "graphics_api.h"

typedef struct {
unsigned short type;      //λͼ�ļ������ͣ�����ΪBM(1-2�ֽڣ�
unsigned long  size;      //λͼ�ļ��Ĵ�С�����ֽ�Ϊ��λ��3-6�ֽڣ���λ��ǰ��
unsigned short reserved1; //λͼ�ļ������֣�����Ϊ0(7-8�ֽڣ�
unsigned short reserved2; //λͼ�ļ������֣�����Ϊ0(9-10�ֽڣ�
unsigned long  offset;    //λͼ���ݵ���ʼλ�ã��������λͼ��11-14�ֽڣ���λ��ǰ��
//�ļ�ͷ��ƫ������ʾ�����ֽ�Ϊ��λ
} bitmap_header,*point_bitmap_header;

#define BITMAP_HEADER_LENGTH 14

typedef struct {
unsigned long size;                  //���ṹ��ռ���ֽ�����15-18�ֽڣ�
unsigned long width;                 //λͼ�Ŀ�ȣ�������Ϊ��λ��19-22�ֽڣ�
unsigned long height;                //λͼ�ĸ߶ȣ�������Ϊ��λ��23-26�ֽڣ�
unsigned short planes;               //Ŀ���豸�ļ��𣬱���Ϊ1(27-28�ֽڣ�
unsigned short bit_count;            //ÿ�����������λ����������1��˫ɫ������29-30�ֽڣ�
//4(16ɫ����8(256ɫ��16(�߲�ɫ)��24�����ɫ��֮һ
unsigned long compression;           //λͼѹ�����ͣ�������0����ѹ��������31-34�ֽڣ�
//0(BI_RGB)ԭɫ 1(BI_RLE8ѹ�����ͣ���2(BI_RLE4ѹ�����ͣ�֮һ
unsigned long size_image;            //λͼ�Ĵ�С(���а�����Ϊ�˲���������4�ı�������ӵĿ��ֽ�)�����ֽ�Ϊ��λ��35-38�ֽڣ�
unsigned long x_pelsper_meter;       //λͼˮƽ�ֱ��ʣ�ÿ����������39-42�ֽڣ�
unsigned long y_pelsper_meter;       //λͼ��ֱ�ֱ��ʣ�ÿ����������43-46�ֽ�)
unsigned long clr_used;              //λͼʵ��ʹ�õ���ɫ���е���ɫ����47-50�ֽڣ�
unsigned long clr_important;         //λͼ��ʾ��������Ҫ����ɫ����51-54�ֽڣ�
} bitmap_information,*point_bitmap_information;

#define BITMAP_INFORMATION_LENGTH 40

typedef struct {
char blue;     //��ɫ�����ȣ�ֵ��ΧΪ0-255)
char green;    //��ɫ�����ȣ�ֵ��ΧΪ0-255)
char red;      //��ɫ�����ȣ�ֵ��ΧΪ0-255)
char reserved; //����������Ϊ0
} rgb_quad,*point_rgb_quad;

#define RBG_QUAD_LENGTH 4

void graphics_draw_bitmap(bitmap_data bitmap_data,graphics_location draw_point_x,graphics_location draw_point_y);
void graphics_get_bitmap (bitmap_data bitmap_data,unsigned long* bitmap_size,unsigned long* bitmap_height,unsigned long* bitmap_width);

#endif






