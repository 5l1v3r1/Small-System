
#ifndef _FILE_API_H__
#define _FILE_API_H__

#include "system_type.h"

#include "src//ff.h"

#define MAX_FOLDER_NAME_LENGTH 256
#define MAX_FILE_NAME_LENGTH   256

void file_system_init(void);

void disk_size(unsigned long* total,unsigned long* free);

bool folder_create (const char* new_folder_path);
bool folder_rename (const char* old_folder_path,const char* new_folder_path);
bool folder_delete (const char* folder_path);
bool folder_enum_count(const char* folder_path,unsigned long* enum_count);
bool folder_enum   (const char* folder_path,unsigned long enum_index,char* out_buffer,unsigned long* out_buffer_length);

bool file_create  (const char* file_path);
bool file_delete  (const char* file_path);

typedef FIL* file_io;

typedef enum {
    seek_set=0,
    seek_cur,
    seek_end,
} seek_code;

bool          file_open  (const char* file_path,file_io out_file_io);
unsigned long file_length(file_io in_file_io);
bool          file_seek  (file_io in_file_io,seek_code seek_code,const long point_offset);
unsigned long file_write (file_io in_file_io,const char* in_data,unsigned long in_data_length);
unsigned long file_read  (file_io in_file_io,char* out_data,unsigned long out_data_length);
bool          file_close (file_io in_file_io);

#endif
