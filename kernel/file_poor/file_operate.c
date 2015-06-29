
#include "system_type.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "lcd_api.h"

#include "exfuns//exfuns.h"
#include "src//ff.h"

#include "file_api.h"

void file_system_init(void) {
    exfuns_init();
    f_mount(0,fs[0]);
}

void disk_size(unsigned long* total,unsigned long* free) {
    exf_getfree((u8*)"0",(u32*)total,(u32*)free);
}

bool folder_create(const char* new_folder_path) {
    return (FR_OK==f_mkdir(new_folder_path))?true:false;
}
bool folder_rename(const char* old_folder_path,const char* new_folder_path) {
    return (FR_OK==f_rename(old_folder_path,new_folder_path))?true:false;
}
bool folder_delete(const char* folder_path) {
    return (FR_OK==f_unlink(folder_path))?true:false;
}
bool folder_enum_count(const char* folder_path,unsigned long* subfolder_count) {
    DIR directory;
    FILINFO file_data;
    if (FR_OK==f_opendir(&directory,folder_path)) {
        while (FR_OK==f_readdir(&directory,&file_data))
            ++*subfolder_count;
        return true;
    }
    return false;
}
bool folder_enum(const char* folder_path,unsigned long enum_index,char* out_buffer,unsigned long* out_buffer_length) {
    unsigned long find_subfolder_index=0;
    DIR directory;
    FILINFO file_data;
    
    file_data.lfname=malloc(MAX_FOLDER_NAME_LENGTH);
    if (NULL==file_data.lfname) return false;
    file_data.lfsize=MAX_FOLDER_NAME_LENGTH;

    if (FR_OK==f_opendir(&directory,folder_path)) {
        while (FR_OK==f_readdir(&directory,&file_data)) {
            if (file_data.fname[0]) {
                if (enum_index==find_subfolder_index) {
                    memcpy(out_buffer,file_data.lfname,file_data.lfsize);
                    *out_buffer_length=file_data.lfsize;
                    return true;
                } else if (enum_index>find_subfolder_index)
                    return false;
                else
                    ++find_subfolder_index;
            } else
                return false;
        }
    }
    return false;
}

bool file_create(const char* file_path) {
    FIL file;
    if (FR_OK==f_open(&file,file_path,FA_CREATE_NEW)) {
        f_close(&file);
        return true;
    }
    return false;
}
bool file_delete(const char* file_path) {
    f_unlink(file_path);
    return true;
}

bool file_open(const char* file_path,file_io out_file_io) {
    if (FR_OK==f_open(out_file_io,file_path,FA_READ | FA_WRITE))
        return true;
    return false;
}
unsigned long file_length(file_io in_file_io) {
	return f_size(file);
    
}
bool file_seek(file_io in_file_io,seek_code seek_code,const long point_offset) {
    unsigned long file_length_=0;
    unsigned long new_point=0;
    
    file_length_=file_length(in_file_io);
    
    switch (seek_code) {
        case seek_set:
            if (point_offset>file_length_) return false;
            return (FR_OK==f_lseek(in_file_io,new_point))?true:false;
        case seek_cur:
            new_point=f_tell(in_file_io);
            if ((point_offset+new_point)>file_length_) return false;
            return (FR_OK==f_lseek(in_file_io,point_offset+new_point))?true:false;
        case seek_end:
            if (new_point>file_length_) return false;
            return (FR_OK==f_lseek(in_file_io,file_length_-new_point))?true:false;
    }
    return false;
}
unsigned long file_write(file_io in_file_io,const char* in_data,unsigned long in_data_length) {
    unsigned long write_byte=0;
    if (FR_OK==f_write(in_file_io,in_data,in_data_length,(UINT*)&write_byte))
        return write_byte;
    return 0;
}
unsigned long file_read(file_io in_file_io,char* out_data,unsigned long out_data_length) {
    unsigned long read_byte=0;
    if (FR_OK==f_read(in_file_io,out_data,out_data_length,(UINT*)&read_byte))
        return read_byte;
    return 0;
}
bool file_close(file_io in_file_io) {
    if (FR_OK==f_close(in_file_io))
        return true;
    return false;
}
