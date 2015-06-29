
#include "system_type.h"

#include <string.h>

#include "malloc.h"

#include "script_execute.h"
#include "script_syntax.h"

#define VARIABLES_MAX 64

typedef struct {
    char variables_name[VARIABLES_NAME_LENGTH];
    char variables_type[VARIABLES_TYPE_LENGTH+1];
    unsigned long variables_stack_index;
} variables_table;

struct {
    unsigned long   variables_count;
    variables_table variables_table_list[VARIABLES_MAX];
} globel_variables_table;

char* script_data=NULL;
unsigned long script_data_length=0;
char* script_stack=NULL;
unsigned long script_stack_size=0;
unsigned long script_line=0;

static bool read_script_line(unsigned long in_script_line,char* output_buffer) {
    unsigned long find_line=0;
    unsigned long find_char_index_right=0;
    unsigned long find_char_index_left=0;

    if (in_script_line<=script_line) {
        if (!in_script_line) {
            for (;find_char_index_right<script_data_length;++find_char_index_right) {
                if ('\n'==*(char*)((unsigned long)script_data+find_char_index_right)) {
                    memcpy(output_buffer,script_data,find_char_index_right);
                    return true;
                }
            }
        } else {
            for (;find_char_index_right<script_data_length;++find_char_index_right) {
                if ('\n'==*(char*)((unsigned long)script_data+find_char_index_right)) {
                    if (in_script_line==find_line) {
                        for (find_char_index_left=find_char_index_right-1;find_char_index_right!=0;--find_char_index_left) {
                            if ('\n'==*(char*)((unsigned long)script_data+find_char_index_left)) {
                                memcpy(output_buffer,(const char*)((unsigned long)script_data+find_char_index_left+1),find_char_index_right-find_char_index_left-1);
                                return true;
                           }
                        }
                    } else
                        ++find_line;
                }
            }
        }
    }
    return false;
}

static unsigned long get_script_line(void) {
    unsigned long statistics_line=0;
    unsigned long find_char_index=0;

    for (;find_char_index<script_data_length;++find_char_index)
        if ('\n'==*(char*)((unsigned long)script_data+find_char_index))
            ++statistics_line;
    
    return statistics_line;
}

#define GET_SCRIPT_STACK_ERROR 0xFFFFFFFF

static unsigned long get_script_stack_size(void) {
    unsigned long stack_size=0;
    unsigned long find_final_data_line=0;
    unsigned long init_data_line=1;
    char code_line[SCRIPT_LINE_LENGTH]={0};
    char code_variables_name[VARIABLES_NAME_LENGTH]={0};
    char code_variables_type[VARIABLES_TYPE_LENGTH+1]={0};
    unsigned long code_variables_type_size=0;

    if (read_script_line(0,code_line)) {
        if (0==strcmp(code_line,"DATA SECTION:")) {
            for (;find_final_data_line<script_line;++find_final_data_line) {
                memset(code_line,0,SCRIPT_LINE_LENGTH);
                read_script_line(find_final_data_line,code_line);
                if (0==strcmp(code_line,"DATA END")) {
                    for (;init_data_line<find_final_data_line;++init_data_line) {
                        memset(code_line,0,SCRIPT_LINE_LENGTH);
                        read_script_line(init_data_line,code_line);
                        if (check_syntax_line(code_line)) {
                            resolve_variables_declare(code_line,code_variables_name,code_variables_type,&code_variables_type_size);
                            stack_size+=code_variables_type_size;
                            memcpy(globel_variables_table.variables_table_list[globel_variables_table.variables_count].variables_name,code_variables_name,strlen(code_variables_name));
                            memcpy(globel_variables_table.variables_table_list[globel_variables_table.variables_count].variables_type,code_variables_type,strlen(code_variables_type));
                            if (!globel_variables_table.variables_count)
                                globel_variables_table.variables_table_list[globel_variables_table.variables_count].variables_stack_index=0;
                            else
                                globel_variables_table.variables_table_list[globel_variables_table.variables_count].variables_stack_index=globel_variables_table.variables_table_list[globel_variables_table.variables_count-1].variables_stack_index+code_variables_type_size;
                            ++globel_variables_table.variables_count;
                            memset(code_variables_name,0,VARIABLES_NAME_LENGTH);
                            memset(code_variables_type,0,VARIABLES_TYPE_LENGTH+1);
                        } else
                            return GET_SCRIPT_STACK_ERROR;
                    }
                    return stack_size;
                }
            }
        }
    }
    return GET_SCRIPT_STACK_ERROR;
}

void clean_script(void) {
    if (NULL!=script_data)
        myfree(SRAMEX,script_data);
    if (NULL!=script_stack) {
        myfree(SRAMEX,script_stack);
        memset(&globel_variables_table,0,sizeof(globel_variables_table));
    }
    script_data_length=0;
    script_stack_size=0;
    script_line=0;
}
