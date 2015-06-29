
#include <string.h>

#include "system_type.h"

#include "flash_outside_api.h"

#include "file_poor_api.h"
#include "file_api.h"

#include <stdio.h>
#include "lcd_api.h"


typedef struct {
    folder_name name;
    folder_attribute attribute;
    folder_data_length length;
    subfolder_count subfolder_list_count;
    file_count file_list_count;
} folder_header,*point_folder_header;

typedef struct {
    file_name name;
    file_attribute attribute;
    file_time time;
    file_data_length length;
} file_header,*point_file_header;


/*

    PAGE DATA STRUCTION:
     __________________________________________________________________    ___
    |                 |                |             |                 |    |
    | SPACE PAGE FLAG | PAGE TYPE FLAG | PAGE LENGTH | NEXT PAGE INDEX |    |
    |------------------------------------------------------------------|    PAGE HEADER
    |      8 bit      |     8 bit      |    16 bit   |     32 bit      |    |
    |------------------------------------------------------------------|   ---
    |                                                                  |
    |                           PAGE DATA                              |
    |                        PAGE_SIZE-64 bit                          |
    |__________________________________________________________________|

*/

#define FLASH_PAGE_HEADER_LENGTH 8
#define FLASH_PAGE_DATA_LENGTH (FLASH_PAGE_LENGTH-FLASH_PAGE_HEADER_LENGTH)

#define FLASH_PAGE_TYPE_FLAG_OFFSET 1

#define FLASH_PAGE_LENGTH_OFFSET    2

#define FLASH_PAGE_NEXT_INDEX_OFFSET 4

#define FLASH_PAGE_DATA_OFFSET 8

bool page_read(page_index in_page_index,point_page_header out_page_header,void* out_page_data,page_length out_page_length) {
    flash_read((void*)(in_page_index*FLASH_PAGE_LENGTH),&out_page_header->page_empty_flag,sizeof(char));
    if (flash_page_flag_nonempty==out_page_header->page_empty_flag) {
        flash_read((void*)(in_page_index*FLASH_PAGE_LENGTH+FLASH_PAGE_TYPE_FLAG_OFFSET),&out_page_header->page_type_flag,sizeof(flash_page_type_flag));
        flash_read((void*)(in_page_index*FLASH_PAGE_LENGTH+FLASH_PAGE_LENGTH_OFFSET),&out_page_header->page_data_length,sizeof(page_length));
        flash_read((void*)(in_page_index*FLASH_PAGE_LENGTH+FLASH_PAGE_NEXT_INDEX_OFFSET),&out_page_header->next_page_index,sizeof(page_index));
        
        if (NULL!=out_page_data || NULL!=out_page_length)
            flash_read((void*)(in_page_index*FLASH_PAGE_LENGTH+FLASH_PAGE_DATA_OFFSET),out_page_data,(out_page_length>FLASH_PAGE_DATA_LENGTH)?FLASH_PAGE_DATA_LENGTH:out_page_length);
        return true;
    }
    return false;
}

void page_write(page_index in_page_index,point_page_header in_page_header,const void* in_page_data) {
    if (FLASH_PAGE_DATA_LENGTH>=in_page_header->page_data_length) {
        flash_write((void*)(in_page_index*FLASH_PAGE_LENGTH),&in_page_header->page_empty_flag,sizeof(char));
        flash_write((void*)(in_page_index*FLASH_PAGE_LENGTH+FLASH_PAGE_TYPE_FLAG_OFFSET),&in_page_header->page_type_flag,sizeof(flash_page_type_flag));
        flash_write((void*)(in_page_index*FLASH_PAGE_LENGTH+FLASH_PAGE_LENGTH_OFFSET),&in_page_header->page_data_length,sizeof(page_length));
        flash_write((void*)(in_page_index*FLASH_PAGE_LENGTH+FLASH_PAGE_NEXT_INDEX_OFFSET),&in_page_header->next_page_index,sizeof(page_index));
        
        if (NULL!=in_page_data)
            flash_write((void*)(in_page_index*FLASH_PAGE_LENGTH+FLASH_PAGE_DATA_OFFSET),in_page_data,(in_page_header->page_data_length>FLASH_PAGE_DATA_LENGTH)?FLASH_PAGE_DATA_LENGTH:in_page_header->page_data_length);
    }
}

void page_erase(page_index in_page_index) {
    flash_erase_page(in_page_index);
}

page_index page_first_empty_get(void) {
    page_index page_count=flash_get_size()/FLASH_PAGE_LENGTH;
    page_index page_find_index=0;
    flash_page_empty_flag page_empty_flag=flash_page_flag_empty;
    
    for (;page_find_index<page_count;++page_find_index) {
        flash_read((void*)(page_find_index*FLASH_PAGE_LENGTH),&page_empty_flag,sizeof(flash_page_empty_flag));
        if (flash_page_flag_empty==page_empty_flag)
            return page_find_index;
        if (flash_page_flag_nonempty!=page_empty_flag) {
            page_erase(page_find_index);
            return page_find_index;
        }
    }
    return FLASH_PAGE_INDEX_ERROR;
}

flash_page_type_flag page_type_get(page_index in_page_index) {
    bool flash_page_empty_flag=true;
    flash_page_type_flag return_result=flash_page_type_flag_get_error;
    flash_read((void*)(in_page_index*FLASH_PAGE_LENGTH),&flash_page_empty_flag,sizeof(char));
    if (flash_page_flag_nonempty==flash_page_empty_flag)
        flash_read((void*)(in_page_index*FLASH_PAGE_LENGTH+FLASH_PAGE_TYPE_FLAG_OFFSET),&return_result,sizeof(flash_page_type_flag));
    return return_result;
}

page_index page_create(flash_page_type_flag page_type_flag) {
    page_header new_page_header;
    page_index first_empty_page=page_first_empty_get();
    if (FLASH_PAGE_INDEX_ERROR!=first_empty_page) {
        new_page_header.next_page_index=FLASH_PAGE_INDEX_ERROR;
        new_page_header.page_data_length=0;
        new_page_header.page_empty_flag=flash_page_flag_nonempty;
        new_page_header.page_type_flag=page_type_flag;
        
        page_write(first_empty_page,&new_page_header,NULL);
    }
    return first_empty_page;
}

void page_delete(page_index in_page_index) {
    page_header delete_page_header;
    delete_page_header.next_page_index=FLASH_PAGE_INDEX_ERROR;
    delete_page_header.page_data_length=0;
    delete_page_header.page_empty_flag=flash_page_flag_empty;
    delete_page_header.page_type_flag=flash_page_type_flag_empty;
    page_write(in_page_index,&delete_page_header,NULL);
}

void page_relation_read(page_index first_page_index,void* out_page_data,page_data_length out_page_data_length) {
    page_header read_page_header;
    page_index  read_page_index=first_page_index;
    page_length read_index=0;
    
    page_read(first_page_index,&read_page_header,NULL,0);
    if (flash_page_flag_nonempty==read_page_header.page_empty_flag) {
        for (;out_page_data_length;read_index+=(out_page_data_length>=FLASH_PAGE_DATA_LENGTH)?FLASH_PAGE_DATA_LENGTH:out_page_data_length,out_page_data_length-=(out_page_data_length>=FLASH_PAGE_DATA_LENGTH)?FLASH_PAGE_DATA_LENGTH:out_page_data_length) {
            page_read(read_page_index,&read_page_header,(void*)((unsigned long)out_page_data+read_index),(out_page_data_length>=FLASH_PAGE_DATA_LENGTH)?FLASH_PAGE_DATA_LENGTH:out_page_data_length);
            read_page_index=read_page_header.next_page_index;
            if (FLASH_PAGE_INDEX_ERROR==read_page_index)
                return;
        }
    }
}

void page_relation_write(page_index first_page_index,const void* in_page_data,page_data_length in_page_length) {
    page_header read_page_header;
    page_index  write_page_index=first_page_index;
    page_data_length write_data_offset=first_page_index;
    
    page_read(first_page_index,&read_page_header,NULL,0);
    if (flash_page_flag_nonempty==read_page_header.page_empty_flag) {
        for (;in_page_length;write_data_offset+=(in_page_length>=FLASH_PAGE_DATA_LENGTH)?FLASH_PAGE_DATA_LENGTH:in_page_length,in_page_length-=(in_page_length>=FLASH_PAGE_DATA_LENGTH)?FLASH_PAGE_DATA_LENGTH:in_page_length) {
            read_page_header.page_data_length=(in_page_length>=FLASH_PAGE_DATA_LENGTH)?FLASH_PAGE_DATA_LENGTH:in_page_length;
            read_page_header.next_page_index=(in_page_length>FLASH_PAGE_DATA_LENGTH)?page_create(read_page_header.page_type_flag):FLASH_PAGE_INDEX_ERROR;
            page_write(write_page_index,&read_page_header,(const void*)((unsigned long)in_page_data+write_data_offset));
            write_page_index=read_page_header.next_page_index;
        }
    }
}

/*
    FOLDER DATA STRUCTION:
     ________________________________________________________________________________    ___
    |                                                                                |    |
    |                                   FOLDER NAME                                  |    |
    |--------------------------------------------------------------------------------|    |
    |                                     8*32 bit                                   |    |
    |--------------------------------------------------------------------------------|    FOLDER HEADER
    |  FOLDER ATTRIBUTE  |  FOLDER DATA LENGTH  |  SUBFOLDER COUNT  |   FILE COUNT   |    |
    |--------------------------------------------------------------------------------|    |
    |       8 bit        |       24 bit         |      16 bit       |     16 bit     |    |
    |--------------------------------------------------------------------------------|   ---  SUBFOLDER LIST OFFSET = 0
    |  SUBFOLDER LIST:                                                               |    |
    |--------------------------------------------------------------------------------|    |
    |            SUBFOLDER NAME              |         SUBFOLDER PAGE INDEX          |    |
    |--------------------------------------------------------------------------------|    |
    |               8*32 bit                 |               32 bit                  |    |
    |--------------------------------------------------------------------------------|    |
    |            SUBFOLDER NAME              |         SUBFOLDER PAGE INDEX          |    |
    |--------------------------------------------------------------------------------|    |
    |               8*32 bit                 |               32 bit                  |    |
    |--------------------------------------------------------------------------------|    |
    |                                        .                                       |    |
    |                                        .                                       |    |
    |                                        .                                       |    |
    |--------------------------------------------------------------------------------|    v
    |  FILE LIST:                                                                    |   ---  FILE LIST OFFSET = SUBFOLDER COUNT * sizeof(SUBFOLDER LIST)
    |--------------------------------------------------------------------------------|
    |              FILE NAME                 |           FILE PAGE INDEX             |
    |--------------------------------------------------------------------------------|
    |               8*32 bit                 |               32 bit                  |
    |--------------------------------------------------------------------------------|
    |              FILE NAME                 |           FILE PAGE INDEX             |
    |--------------------------------------------------------------------------------|
    |               8*32 bit                 |               32 bit                  |
    |--------------------------------------------------------------------------------|
    |                                        .                                       |
    |                                        .                                       |
    |                                        .                                       |
    |________________________________________________________________________________|
    
    TIPS: 文件夹名不包含拓展名,在这个文件夹下的子文件夹和文件都用列表来保存着..
    一个文件夹可以保存65535 个文件和文件夹(37748160),文件夹数据结构可以储存16777215 个字节.
    其实可以在文件夹属性里面减少1 位,这样就刚好让文件夹可以容纳37748160 个字节,以后改吧  ..
    
*/

static bool is_folder(page_index in_page_index) {
    return (flash_page_type_flag_folder==page_type_get(in_page_index))?true:false;
}

#define FOLDER_HEADER_LENGTH          40

#define FOLDER_ATTRIBUTE_OFFSET       FOLDER_NAME_LENGTH
#define FOLDER_DATA_LENGTH_OFFSET     FOLDER_NAME_LENGTH+1
#define FOLDER_SUBFOLDER_COUNT_OFFSET FOLDER_NAME_LENGTH+4
#define FOLDER_FILE_COUNT_OFFSET      FOLDER_NAME_LENGTH+6

bool folder_read_header(page_index first_page_index,point_folder_header out_folder_header) {
    page_header page_header;
    char read_folder_header[FOLDER_HEADER_LENGTH];
    
    char print_string[32]={0};
    
    page_read(first_page_index,&page_header,read_folder_header,FOLDER_HEADER_LENGTH);
    if (page_header.page_type_flag==flash_page_type_flag_folder) {
            memcpy(out_folder_header->name,read_folder_header,FOLDER_NAME_LENGTH);
            memcpy(&out_folder_header->attribute,(void*)((long)read_folder_header+FOLDER_ATTRIBUTE_OFFSET),sizeof(folder_attribute));
            memcpy(&out_folder_header->length,(void*)((long)read_folder_header+FOLDER_DATA_LENGTH_OFFSET),sizeof(folder_data_length));
            memcpy(&out_folder_header->subfolder_list_count,(void*)((long)read_folder_header+FOLDER_SUBFOLDER_COUNT_OFFSET),sizeof(subfolder_count));
            memcpy(&out_folder_header->file_list_count,(void*)((long)read_folder_header+FOLDER_FILE_COUNT_OFFSET),sizeof(file_count));
            
    sprintf(print_string,"folder name=%s",out_folder_header->name);
    lcd_print_line(print_string);
    sprintf(print_string,"folder length=%ld",out_folder_header->length);
    lcd_print_line(print_string);
    sprintf(print_string,"folder attribute=%d",out_folder_header->attribute);
    lcd_print_line(print_string);
    sprintf(print_string,"subfolder_list_count=%ld",(long)out_folder_header->subfolder_list_count);
    lcd_print_line(print_string);
    sprintf(print_string,"file_list_count=%ld",(long)out_folder_header->file_list_count);
    lcd_print_line(print_string);
        return true;
    }
    lcd_print_line("is not folder!");
    sprintf(print_string,"empty flag=%d",page_header.page_empty_flag);
    lcd_print_line(print_string);
    lcd_print_line("read ERR!");
    return false;
}
bool folder_write_header(page_index first_page_index,point_folder_header in_folder_header) {
    page_header read_page_header;
    char write_folder_header[FOLDER_HEADER_LENGTH];
    
    char print_string[32]={0};
    
    page_read(first_page_index,&read_page_header,write_folder_header,FOLDER_HEADER_LENGTH);
    if (read_page_header.page_type_flag==flash_page_type_flag_folder) {
        memcpy(write_folder_header,in_folder_header->name,FOLDER_NAME_LENGTH);
        memcpy((void*)((long)write_folder_header+FOLDER_ATTRIBUTE_OFFSET),&in_folder_header->attribute,sizeof(folder_attribute));
        memcpy((void*)((long)write_folder_header+FOLDER_DATA_LENGTH_OFFSET),&in_folder_header->length,sizeof(folder_data_length));
        memcpy((void*)((long)write_folder_header+FOLDER_SUBFOLDER_COUNT_OFFSET),&in_folder_header->subfolder_list_count,sizeof(subfolder_count));
        memcpy((void*)((long)write_folder_header+FOLDER_FILE_COUNT_OFFSET),&in_folder_header->file_list_count,sizeof(file_count));
        page_write(first_page_index,&read_page_header,write_folder_header);
            
    sprintf(print_string,"empty flag=%d",read_page_header.page_empty_flag);
    lcd_print_line(print_string);
    sprintf(print_string,"folder name=%s",in_folder_header->name);
    lcd_print_line(print_string);
    sprintf(print_string,"folder length=%ld",in_folder_header->length);
    lcd_print_line(print_string);
    sprintf(print_string,"folder attribute=%d",in_folder_header->attribute);
    lcd_print_line(print_string);
    sprintf(print_string,"subfolder_list_count=%ld",(long)in_folder_header->subfolder_list_count);
    lcd_print_line(print_string);
    sprintf(print_string,"file_list_count=%ld",(long)in_folder_header->file_list_count);
    lcd_print_line(print_string);
        return true;
    }
    return false;
}

bool folder_read_subfolder(page_index first_page_index,void* out_subfolder_list,unsigned long out_subfolder_list_length) {
    page_header page_header;
    char folder_header[FOLDER_HEADER_LENGTH];
    if (!is_folder(first_page_index)) return false;
    if (page_read(first_page_index,&page_header,folder_header,FOLDER_HEADER_LENGTH)) {
        if (page_header.page_type_flag==flash_page_type_flag_folder) {
            page_relation_read(first_page_index,out_subfolder_list,out_subfolder_list_length);
            return true;
        }
    }
    return false;
}
bool folder_read_file(page_index first_page_index,void* out_file_list,unsigned long out_file_list_length) {
    page_header page_header;
    char folder_header[FOLDER_HEADER_LENGTH];
    if (!is_folder(first_page_index)) return false;
    if (page_read(first_page_index,&page_header,folder_header,FOLDER_HEADER_LENGTH)) {
        if (page_header.page_type_flag==flash_page_type_flag_folder) {
            page_relation_read(first_page_index,out_file_list,out_file_list_length);
            return true;
        }
    }
    return false;
}


/*
    FILE DATA STRUCTION:
     __________________________________________________________    ___
    |                                                          |    |
    |                         FILE NAME                        |    |
    |----------------------------------------------------------|    |
    |                          8*32 bit                        |    |
    |----------------------------------------------------------|    FILE HEADER
    |  FILE ATTRIBUTE | FILE TIME |     FILE DATA LENGTH       |    |
    |----------------------------------------------------------|    |
    |      8 bit      |   24 bit  |          32 bit            |    |
    |----------------------------------------------------------|   ---
    |                                                          |
    |                        FILE DATA                         |
    |                     FILE DATA LENGTH                     |
    |__________________________________________________________|

    TIPS: 文件名包含文件拓展名;文件数据长度是指整个文件大小而不是所占页面的大小..

    TIPS: ATTRIBUTE 里面可能也需要指定该数据是文件还是文件夹
    ATTRIBUTE 8-bit:
     ___________________________________
    |0         |1       |2 3 4 5 6 7    |
    |-----------------------------------|
    |文件夹标志|文件标志|保留           |
    |___________________________________|
    
*/

#define FILE_HEADER_LENGTH      (10*32)

#define FILE_ATTRIBUTE_OFFSET       FILE_NAME_LENGTH
#define FILE_TIME_OFFSET            33
#define FILE_DATA_LENGTH_OFFSET     36
#define FILE_DATA_OFFSET            FILE_HEADER_LENGTH

static bool is_file(page_index first_page_index) {
    return (flash_page_type_flag_file==page_type_get(first_page_index))?true:false;
}

bool file_read_header(page_index first_page_index,point_file_header out_file_header) {
    page_header page_header;
    char read_file_header[FOLDER_HEADER_LENGTH];
    if (!is_file(first_page_index)) return false;
    if (page_read(first_page_index,&page_header,read_file_header,FILE_HEADER_LENGTH)) {
        if (page_header.page_type_flag==flash_page_type_flag_file) {
            memcpy(out_file_header->name,read_file_header,FILE_NAME_LENGTH);
            memcpy(&out_file_header->attribute,(void*)((long)read_file_header+FILE_ATTRIBUTE_OFFSET),sizeof(file_attribute));
            memcpy(&out_file_header->time,(void*)((long)read_file_header+FILE_TIME_OFFSET),sizeof(file_time)-1);
            memcpy(&out_file_header->length,(void*)((long)read_file_header+FILE_DATA_LENGTH_OFFSET),sizeof(folder_data_length));
            return true;
        }
    }
    return false;
}
bool file_read_data(page_index first_page_index,void* out_file_data,unsigned long out_file_data_length) {
    page_header page_header;
    char file_data;
    if (!is_folder(first_page_index)) return false;
    if (page_read(first_page_index,&page_header,&file_data,FILE_HEADER_LENGTH)) {
        if (page_header.page_type_flag==flash_page_type_flag_file) {
            page_relation_read(first_page_index,out_file_data,out_file_data_length);
            return true;
        }
    }
    return false;
}
bool file_write_data(page_index first_page_index,const void* in_file_data,unsigned long in_file_data_length) {
    page_header page_header;
    char file_data;
    if (!is_folder(first_page_index)) return false;
    if (page_read(first_page_index,&page_header,&file_data,FILE_HEADER_LENGTH)) {
        if (page_header.page_type_flag==flash_page_type_flag_file) {
            page_relation_write(first_page_index,in_file_data,in_file_data_length);
            return true;
        }
    }
    return false;
}

void file_poor_test_(void) {
    page_header read_page_header;
    char buffer[11];
    char print_string[30];
    page_header new_page_header;
    page_index new_page=0xFFFFFFFF;
    folder_header write_folder_header;
    folder_header read_folder_header;
/*        
    page_erase(0);
    page_erase(1);
    page_delete(1);
    new_page_header.page_empty_flag=flash_page_flag_nonempty;
    new_page_header.page_data_length=4;
    new_page_header.next_page_index=1;
    new_page_header.page_type_flag=flash_page_type_flag_empty;
    page_write(0,&new_page_header,"TEST");
    page_write(1,&new_page_header,"TEST");
    lcd_print("write end");
    

    page_relation_write(0,"HELLO THIS IS A TEST ABOUT FILE SYSTEM ",40,0);
    lcd_print("write end");
    
*/ /*   
    new_page_header.page_empty_flag=flash_page_flag_nonempty;
    new_page_header.page_data_length=5;
    new_page_header.next_page_index=0;
    new_page_header.page_type_flag=flash_page_type_flag_empty;
    page_write(0,&new_page_header,"HELLO");
    new_page_header.page_data_length=4;
    new_page_header.page_type_flag=flash_page_type_flag_folder;
    page_write(1,&new_page_header,"TEST");
    page_read(0,&read_page_header,buffer,5);
    lcd_print("read end");
    sprintf(print_string,"%d %d %d %d",(int)read_page_header.page_empty_flag,(int)read_page_header.page_data_length,(int)read_page_header.next_page_index,(int)read_page_header.page_type_flag);
    lcd_print(print_string);
    lcd_print(buffer);
    page_read(1,&read_page_header,buffer,4);
    lcd_print("read end");
    sprintf(print_string,"%d %d %d %d",(int)read_page_header.page_empty_flag,(int)read_page_header.page_data_length,(int)read_page_header.next_page_index,(int)read_page_header.page_type_flag);
    lcd_print(print_string);
    lcd_print(buffer);
    page_relation_read(0,buffer,40,6);
    lcd_print("read end");
    lcd_print(buffer);

    new_page=page_create(flash_page_type_flag_empty);
    page_relation_write(new_page,"HELLO TEST",10);
//    page_relation_write(new_page," WORLD",6,5);
    page_read(new_page,&read_page_header,buffer,11);
    sprintf(print_string,"%d %d %d %d %d",(int)new_page,(int)read_page_header.page_empty_flag,(int)read_page_header.page_data_length,(int)read_page_header.next_page_index,(int)read_page_header.page_type_flag);
    lcd_print(print_string);
    lcd_print(buffer);
    page_relation_read(new_page,buffer,10);
    lcd_print(buffer);
    page_erase(new_page);*/ 
    write_folder_header.attribute=folder_flag;
    write_folder_header.file_list_count=1;
    write_folder_header.subfolder_list_count=2;
    memcpy(write_folder_header.name,"ROOT FILE",10);
    write_folder_header.length=16;
    
    new_page=page_create(flash_page_type_flag_folder);
//    page_read(new_page,&read_page_header,NULL,0);
//    sprintf(print_string,"page_empty_flag=%d",read_page_header.page_empty_flag);
    lcd_print_line(print_string);
    folder_write_header(new_page,&write_folder_header);
    folder_read_header(new_page,&read_folder_header);
    sprintf(print_string,"new page=%d",(int)new_page);
    lcd_print_line(print_string);
   
    sprintf(print_string,"folder name=%s",read_folder_header.name);
    lcd_print_line(print_string);
    sprintf(print_string,"folder length=%ld",read_folder_header.length);
    lcd_print_line(print_string);
    sprintf(print_string,"folder attribute=%d",read_folder_header.attribute);
    lcd_print_line(print_string);
    sprintf(print_string,"subfolder_list_count=%ld",(long)read_folder_header.subfolder_list_count);
    lcd_print_line(print_string);
    sprintf(print_string,"file_list_count=%ld",(long)read_folder_header.file_list_count);
    lcd_print_line(print_string);
    page_erase(new_page);
}










