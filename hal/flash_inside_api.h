
#ifndef _FLASH_INSIDE_API_H__
#define _FLASH_INSIDE_API_H__

//  WARNING! ����FLASH_ProgramHalfWord ������д��Щ����,�������ھ�����
//  ��ϵͳ�״ΰ�װʱ,����д����FLASH ��,�Ժ�ֻ�ṩ�������ṩд..
//  ����ȫ��������д,ֻ�������ܺܵ�
//  2014/7/4

void flash_inside_init (void);
void flash_inside_write(void* address,void* data,long length);
void flash_inside_read (void* address,void* data,long length);
void flash_inside_erase(void* address,long length);
void flash_inside_erase_all(void);

long flasg_inside_get_size(void);

#endif
