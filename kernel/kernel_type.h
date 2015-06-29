
#ifndef _KERNEL_TYPE_H__
#define _KERNEL_TYPE_H__

typedef void* memory_address;
typedef long  memory_length;

typedef enum {
    folder_attribute_empty=0,
    folder_flag=0x80,
    file_flag=0x40,
} folder_attribute;
#define FOLDER_NAME_LENGTH 32
typedef char folder_name[FOLDER_NAME_LENGTH];
typedef unsigned long folder_data_length;
typedef unsigned long folder_attribute_and_data_length;
typedef unsigned int subfolder_count;
typedef unsigned int file_count;

#define FILE_NAME_LENGTH 32
typedef char file_name[FILE_NAME_LENGTH];
typedef folder_attribute file_attribute;
typedef unsigned long file_time;
typedef unsigned long file_attribute_and_time;
typedef unsigned long file_data_length;
typedef unsigned long file_point;

typedef enum {
	FILE_IO_READ,
	FILE_IO_WRITE,
	FILE_IO_ALL,
} file_io_flag;

typedef enum {
	FILE_SHARE_NO,
	FILE_SHARE_READ,
	FILE_SHARE_WRITE,
	FILE_SHARE_ALL,
} file_share_flag;

typedef enum {
	FILE_UNLOCK,
	FILE_LOCK,
} file_lock_flag;

typedef struct {
    file_point      point;
	file_io_flag    io_flag;
	file_share_flag share_flag;
	file_lock_flag  lock_flag;
} file_io;

#endif
