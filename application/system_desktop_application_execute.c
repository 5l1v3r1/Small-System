
#include <string.h>
#include <stdlib.h>

#include "file_api.h"

#include "malloc.h"

#include "system_desktop_application_execute.h"

#include "script_hoster.h"

#define MAX_WIDGET_COUNT 10

#define MAX_CODE_LENGTH  100*1024

#define APP_NAME_LENGHT  12
#define APP_PATH_LENGTH  21
#define FILE_NAME_LENGTH APP_PATH_LENGTH+APP_NAME_LENGHT+1

#define APP_MAIN_CODE_EXTENSION       ".sbi"
#define APP_WIDGET_CODE_EXTENSION     ".sbw"

typedef struct {
    unsigned long widget_count;
    char widget_recall_file_name[MAX_WIDGET_COUNT][FILE_NAME_LENGTH];
} application_widget_recall_list;

bool system_desktop_application_execute_init(const char* app_root_path,const char* app_name) {
    char app_init_code_file[FILE_NAME_LENGTH]={0};
    FIL* app_init_code_file_=NULL;

    memcpy(app_init_code_file,app_root_path,APP_PATH_LENGTH);
    strcat(app_init_code_file,"/");
    strcat(app_init_code_file,app_name);
    strcat(app_init_code_file,"/");
    strcat(app_init_code_file,APP_MAIN_CODE_EXTENSION);
    if (file_open(app_init_code_file,app_init_code_file_)) {
        if (MAX_CODE_LENGTH>=file_length(app_init_code_file_)) {
            file_close(app_init_code_file_);
            return load_script(app_init_code_file);
        }
        file_close(app_init_code_file_);
    }
    return false;
}

void system_desktop_application_execute_widget_notice(notice_type in_notice_type) {
}
