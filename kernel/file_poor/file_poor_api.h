
#ifndef _FILE_POOR_API_H__
#define _FILE_POOR_API_H__

#include "system_type.h"

#include "file_api.h"

#define FLASH_PAGE_LENGTH 20//(4*1024)

typedef enum {
    flash_page_flag_empty=0xFF,
    flash_page_flag_nonempty=1,
} flash_page_empty_flag;

typedef enum {
    flash_page_type_flag_get_error=-1,
    flash_page_type_flag_empty=0x00,
    flash_page_type_flag_folder=0x10,
    flash_page_type_flag_file=0x20,
} flash_page_type_flag;

typedef unsigned short page_length;
typedef unsigned long page_index;

#define FLASH_PAGE_INDEX_ERROR 0xFFFFFFFF

page_index page_first_empty_get(void);

typedef struct {
    flash_page_empty_flag page_empty_flag;
    flash_page_type_flag page_type_flag;
    page_length page_data_length;
    page_index next_page_index;
} page_header,*point_page_header;

bool page_read(page_index in_page_index,point_page_header out_page_header,void* out_page_data,page_length out_page_length);
void page_write(page_index in_page_index,point_page_header in_page_header,const void* in_page_data);
flash_page_type_flag page_type_get(page_index in_page_index);

typedef unsigned long page_data_length;
typedef unsigned long page_data_offset;

page_index page_create(flash_page_type_flag page_type_flag);
void page_delete(page_index in_page_index);

void page_relation_read(page_index first_page_index,void* out_page_data,page_data_length out_page_data_length);
void page_relation_write(page_index first_page_index,const void* in_page_data,page_data_length in_page_length);

bool folder_read_header(page_index first_page_index,point_folder_header out_folder_header);
bool folder_read_subfolder(page_index first_page_index,void* out_subfolder_list,unsigned long out_subfolder_list_length);
bool folder_read_file(page_index first_page_index,void* out_file_list,unsigned long out_file_list_length);

bool file_read_header(page_index first_page_index,point_file_header out_file_header);
bool file_read_data(page_index first_page_index,void* out_file_data,unsigned long out_file_data_length);
bool file_write_data(page_index first_page_index,const void* in_file_data,unsigned long in_file_data_length);

#endif
