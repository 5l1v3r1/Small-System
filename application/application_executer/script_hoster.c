
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "malloc.h"

#include "file_api.h"
#include "graphics_window.h"

#include "script_hoster.h"
#include "script_resolve.h"
#include "script_syntax.h"

#define VARIABLES_MAX 64

typedef struct {
    char variables_name[VARIABLES_NAME_LENGTH];
    char variables_type[VARIABLES_TYPE_LENGTH+1];
    unsigned long variables_size;
    unsigned long variables_stack_offset;
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
unsigned long script_code_line=0;

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
                    script_code_line=find_final_data_line+1;
                    for (;init_data_line<find_final_data_line;++init_data_line) {
                        memset(code_line,0,SCRIPT_LINE_LENGTH);
                        read_script_line(init_data_line,code_line);
                        if (check_syntax_line(code_line)) {
                            resolve_variables_declare(code_line,code_variables_name,code_variables_type,&code_variables_type_size);
                            stack_size+=code_variables_type_size;
                            memcpy(globel_variables_table.variables_table_list[globel_variables_table.variables_count].variables_name,code_variables_name,strlen(code_variables_name));
                            memcpy(globel_variables_table.variables_table_list[globel_variables_table.variables_count].variables_type,code_variables_type,strlen(code_variables_type));
                            globel_variables_table.variables_table_list[globel_variables_table.variables_count].variables_size=code_variables_type_size;
                            if (!globel_variables_table.variables_count)
                                globel_variables_table.variables_table_list[globel_variables_table.variables_count].variables_stack_offset=0;
                            else
                                globel_variables_table.variables_table_list[globel_variables_table.variables_count].variables_stack_offset=globel_variables_table.variables_table_list[globel_variables_table.variables_count-1].variables_stack_offset+globel_variables_table.variables_table_list[globel_variables_table.variables_count-1].variables_size;
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

static bool is_valid_variables(const char* variables_name,char* variables_type,unsigned long* variables_size,unsigned long* variables_offset) {
    unsigned long find_index=0;
    unsigned long find_count=globel_variables_table.variables_count;

    for (;find_index<find_count;++find_index) {
        if (!strcmp(globel_variables_table.variables_table_list[find_index].variables_name,variables_name)) {
            memcpy(variables_type,globel_variables_table.variables_table_list[find_index].variables_type,strlen(globel_variables_table.variables_table_list[find_index].variables_type));
            *variables_size=globel_variables_table.variables_table_list[find_index].variables_size;
            *variables_offset=globel_variables_table.variables_table_list[find_index].variables_stack_offset;
            return true;
        }
    }
    return false;
}

static long read_stack_long(unsigned long variables_offset) {
    return *(long*)(script_stack+variables_offset);
}

static float read_stack_float(unsigned long variables_offset) {
    return *(float*)(script_stack+variables_offset);
}

static char read_stack_char(unsigned long variables_offset) {
    return *(char*)(script_stack+variables_offset);
}
static bool read_stack_string(unsigned long variables_offset,unsigned long string_length,char* buffer) {
    if ((variables_offset+string_length)<script_stack_size) {
        memcpy(buffer,(const void*)((unsigned long)script_stack+variables_offset),string_length);
        return true;
    }
    return false;
}
static void write_stack_long(unsigned long variables_offset,long data) {
    *(long*)(script_stack+variables_offset)=data;
}

static void write_stack_float(unsigned long variables_offset,float data) {
    *(float*)(script_stack+variables_offset)=data;
}

static void write_stack_char(unsigned long variables_offset,char data) {
    *(char*)(script_stack+variables_offset)=data;
}
static void write_stack_string(unsigned long variables_offset,unsigned long string_length,const char* buffer) {
    if (((variables_offset+string_length-1)<script_stack_size) && strlen(buffer)<=string_length) {
        memset((void*)((unsigned long)script_stack+variables_offset),0,string_length);
        memcpy((void*)((unsigned long)script_stack+variables_offset),buffer,strlen(buffer));
    }
}

typedef enum {
    error=0,
    equal,
    inequal,
    greater,
    less,
    greater_equal,
    less_equal,
} term_calcu_type;

static bool calcu_term_result(const char* term_expression) {
    unsigned long find_equal=0;
    unsigned long find_expression_length=strlen(term_expression);
    char code_variables_name_left[VARIABLES_NAME_LENGTH]={0};
    char code_variables_type_left[VARIABLES_NAME_LENGTH]={0};
    unsigned long code_variables_size_left=0;
    unsigned long code_variables_offset_left=0;
    char code_variables_name_right[VARIABLES_NAME_LENGTH]={0};
    char code_variables_type_right[VARIABLES_NAME_LENGTH]={0};
    unsigned long code_variables_size_right=0;
    unsigned long code_variables_offset_right=0;
    term_calcu_type calcu_type=error;

    for (;find_equal<find_expression_length;++find_equal) {
        if ('='==*(char*)(term_expression+find_equal))
            calcu_type=equal;
        if ('>'==*(char*)(term_expression+find_equal))
            calcu_type=greater;
        if ('<'==*(char*)(term_expression+find_equal))
            calcu_type=less;
        if (('<'==*(char*)(term_expression+find_equal)) && ('='==*(char*)(term_expression+find_equal+1)))
            calcu_type=less_equal;
        if (('>'==*(char*)(term_expression+find_equal)) && ('='==*(char*)(term_expression+find_equal+1)))
            calcu_type=greater_equal;
        if (('<'==*(char*)(term_expression+find_equal)) && ('>'==*(char*)(term_expression+find_equal+1)))
            calcu_type=inequal;
        if (calcu_type) {
            if (equal==calcu_type || less==calcu_type || greater==calcu_type) {
                memcpy(code_variables_name_left,term_expression,find_equal);
                memcpy(code_variables_name_right,term_expression+find_equal+1,find_expression_length-find_equal-1);
            } else {
                memcpy(code_variables_name_left,term_expression,find_equal);
                memcpy(code_variables_name_right,term_expression+find_equal+2,find_expression_length-find_equal-2);
            }
            if (is_valid_variables_name(code_variables_name_left)) {
                if (is_valid_variables(code_variables_name_left,code_variables_type_left,&code_variables_size_left,&code_variables_offset_left)) {
                    if (is_valid_variables_name(code_variables_name_right)) {
                        if (is_valid_variables(code_variables_name_right,code_variables_type_right,&code_variables_size_right,&code_variables_offset_right)) {
                            if (!strcmp(code_variables_type_left,"long") && !strcmp(code_variables_type_right,"long")) {
                                switch (calcu_type) {
                                    case equal:
                                        if (read_stack_long(code_variables_offset_left)==read_stack_long(code_variables_offset_right))
                                            return true;
                                        return false;
                                    case inequal:
                                        if (read_stack_long(code_variables_offset_left)!=read_stack_long(code_variables_offset_right))
                                            return true;
                                        return false;
                                    case greater:
                                        if (read_stack_long(code_variables_offset_left)>read_stack_long(code_variables_offset_right))
                                            return true;
                                        return false;
                                    case less:
                                        if (read_stack_long(code_variables_offset_left)<read_stack_long(code_variables_offset_right))
                                            return true;
                                        return false;
                                    case greater_equal:
                                        if (read_stack_long(code_variables_offset_left)>=read_stack_long(code_variables_offset_right))
                                            return true;
                                        return false;
                                    case less_equal:
                                        if (read_stack_long(code_variables_offset_left)<=read_stack_long(code_variables_offset_right))
                                            return true;
                                        return false;
                                }
                            } else if (!strcmp(code_variables_type_left,"float") && !strcmp(code_variables_type_right,"float")) {
                                switch (calcu_type) {
                                    case equal:
                                        if (read_stack_float(code_variables_offset_left)==read_stack_float(code_variables_offset_right))
                                            return true;
                                        return false;
                                    case inequal:
                                        if (read_stack_float(code_variables_offset_left)!=read_stack_float(code_variables_offset_right))
                                            return true;
                                        return false;
                                    case greater:
                                        if (read_stack_float(code_variables_offset_left)>read_stack_float(code_variables_offset_right))
                                            return true;
                                        return false;
                                    case less:
                                        if (read_stack_float(code_variables_offset_left)<read_stack_float(code_variables_offset_right))
                                            return true;
                                        return false;
                                    case greater_equal:
                                        if (read_stack_float(code_variables_offset_left)>=read_stack_float(code_variables_offset_right))
                                            return true;
                                        return false;
                                    case less_equal:
                                        if (read_stack_float(code_variables_offset_left)<=read_stack_float(code_variables_offset_right))
                                            return true;
                                        return false;
                                }
                            } else if (!strcmp(code_variables_type_left,"char") && !strcmp(code_variables_type_right,"char")) {
                                switch (calcu_type) {
                                    case equal:
                                        if (read_stack_char(code_variables_offset_left)==read_stack_char(code_variables_offset_right))
                                            return true;
                                        return false;
                                    case inequal:
                                        if (read_stack_char(code_variables_offset_left)!=read_stack_char(code_variables_offset_right))
                                            return true;
                                        return false;
                                    case greater:
                                        if (read_stack_char(code_variables_offset_left)>read_stack_char(code_variables_offset_right))
                                            return true;
                                        return false;
                                    case less:
                                        if (read_stack_char(code_variables_offset_left)<read_stack_char(code_variables_offset_right))
                                            return true;
                                        return false;
                                    case greater_equal:
                                        if (read_stack_char(code_variables_offset_left)>=read_stack_char(code_variables_offset_right))
                                            return true;
                                        return false;
                                    case less_equal:
                                        if (read_stack_char(code_variables_offset_left)<=read_stack_char(code_variables_offset_right))
                                            return true;
                                        return false;
                                }
                            } else if (!strcmp(code_variables_type_left,"string") && !strcmp(code_variables_type_right,"string")) {
                                //  WARNING!..
                            }
                            return false;
                        }
                    } else if (is_number(code_variables_name_right)) {
                        switch (calcu_type) {
                            case equal:
                                if (read_stack_long(code_variables_offset_left)==string_to_number(code_variables_name_right))
                                    return true;
                                 return false;
                            case inequal:
                                if (read_stack_long(code_variables_offset_left)!=string_to_number(code_variables_name_right))
                                    return true;
                                return false;
                            case greater:
                                if (read_stack_long(code_variables_offset_left)>string_to_number(code_variables_name_right))
                                    return true;
                                return false;
                            case less:
                                if (read_stack_long(code_variables_offset_left)<string_to_number(code_variables_name_right))
                                    return true;
                                return false;
                            case greater_equal:
                                if (read_stack_long(code_variables_offset_left)>=string_to_number(code_variables_name_right))
                                    return true;
                                return false;
                            case less_equal:
                                if (read_stack_long(code_variables_offset_left)<=string_to_number(code_variables_name_right))
                                    return true;
                                return false;
                         }
                         return false;
                    } else if (is_float(code_variables_name_right)) {
                        //  WARNING!  字符串转float 其实还是转long ..
                        switch (calcu_type) {
                            case equal:
                                if (read_stack_float(code_variables_offset_left)==string_to_float(code_variables_name_right))
                                    return true;
                                 return false;
                            case inequal:
                                if (read_stack_float(code_variables_offset_left)!=string_to_float(code_variables_name_right))
                                    return true;
                                return false;
                            case greater:
                                if (read_stack_float(code_variables_offset_left)>string_to_float(code_variables_name_right))
                                    return true;
                                return false;
                            case less:
                                if (read_stack_float(code_variables_offset_left)<string_to_float(code_variables_name_right))
                                    return true;
                                return false;
                            case greater_equal:
                                if (read_stack_float(code_variables_offset_left)>=string_to_float(code_variables_name_right))
                                    return true;
                                return false;
                            case less_equal:
                                if (read_stack_float(code_variables_offset_left)<=string_to_float(code_variables_name_right))
                                    return true;
                                return false;
                         }
                         return false;
                    } else if (is_char(code_variables_name_right)) {
                        switch (calcu_type) {
                            case equal:
                                if (read_stack_char(code_variables_offset_left)==*(char*)(code_variables_name_right+1))
                                    return true;
                                 return false;
                            case inequal:
                                if (read_stack_char(code_variables_offset_left)!=*(char*)(code_variables_name_right+1))
                                    return true;
                                return false;
                            case greater:
                                if (read_stack_char(code_variables_offset_left)>*(char*)(code_variables_name_right+1))
                                    return true;
                                return false;
                            case less:
                                if (read_stack_char(code_variables_offset_left)<*(char*)(code_variables_name_right+1))
                                    return true;
                                return false;
                            case greater_equal:
                                if (read_stack_char(code_variables_offset_left)>=*(char*)(code_variables_name_right+1))
                                    return true;
                                return false;
                            case less_equal:
                                if (read_stack_char(code_variables_offset_left)<=*(char*)(code_variables_name_right+1))
                                    return true;
                                return false;
                         }
                         return false;
                    } else if (is_string(code_variables_name_right)) {
                        //  WARNING!..
                    }
                }
            } else if (is_number(code_variables_name_left)) {
                if (is_number(code_variables_name_right)) {
                    switch (calcu_type) {
                        case equal:
                            if (string_to_number(code_variables_name_left)==string_to_number(code_variables_name_right))
                                return true;
                            return false;
                        case inequal:
                            if (string_to_number(code_variables_name_left)!=string_to_number(code_variables_name_right))
                                return true;
                            return false;
                        case greater:
                            if (string_to_number(code_variables_name_left)>string_to_number(code_variables_name_right))
                                return true;
                            return false;
                        case less:
                            if (string_to_number(code_variables_name_left)<string_to_number(code_variables_name_right))
                                return true;
                            return false;
                        case greater_equal:
                            if (string_to_number(code_variables_name_left)>=string_to_number(code_variables_name_right))
                                return true;
                            return false;
                        case less_equal:
                            if (string_to_number(code_variables_name_left)<=string_to_number(code_variables_name_right))
                                return true;
                            return false;
                     }
                }
                return false;
            } else if (is_float(code_variables_name_left)) {
                if (is_float(code_variables_name_right)) {
                    switch (calcu_type) {
                        case equal:
                            if (string_to_float(code_variables_name_left)==string_to_float(code_variables_name_right))
                                return true;
                            return false;
                        case inequal:
                            if (string_to_float(code_variables_name_left)!=string_to_float(code_variables_name_right))
                                return true;
                            return false;
                        case greater:
                            if (string_to_float(code_variables_name_left)>string_to_float(code_variables_name_right))
                                return true;
                            return false;
                        case less:
                            if (string_to_float(code_variables_name_left)<string_to_float(code_variables_name_right))
                                return true;
                            return false;
                        case greater_equal:
                            if (string_to_float(code_variables_name_left)>=string_to_float(code_variables_name_right))
                                return true;
                            return false;
                        case less_equal:
                            if (string_to_float(code_variables_name_left)<=string_to_float(code_variables_name_right))
                                return true;
                            return false;
                     }
                }
                return false;
            } else if (is_char(code_variables_name_left)) {
                if (is_char(code_variables_name_right)) {
                    switch (calcu_type) {
                        case equal:
                            if (*(char*)(code_variables_name_left+1)==*(char*)(code_variables_name_right+1))
                                return true;
                            return false;
                        case inequal:
                            if (*(char*)(code_variables_name_left+1)!=*(char*)(code_variables_name_right+1))
                                return true;
                            return false;
                        case greater:
                            if (*(char*)(code_variables_name_left+1)>*(char*)(code_variables_name_right+1))
                                return true;
                            return false;
                        case less:
                            if (*(char*)(code_variables_name_left+1)<*(char*)(code_variables_name_right+1))
                                return true;
                            return false;
                        case greater_equal:
                            if (*(char*)(code_variables_name_left+1)>=*(char*)(code_variables_name_right+1))
                                return true;
                            return false;
                        case less_equal:
                            if (*(char*)(code_variables_name_left+1)<=*(char*)(code_variables_name_right+1))
                                return true;
                            return false;
                     }
                }
                return false;
            } else if (is_string(code_variables_name_left)) {
                if (is_string(code_variables_name_right))
                    return !strcmp(code_variables_name_left,code_variables_name_right)?true:false;
            } else
                return false;
        }
    }
    return false;
}

static bool calcu_expression(const char* in_calcu_expression,const char* save_variables_name) {
    unsigned long find_flag_index=0;
    unsigned long string_length=strlen(in_calcu_expression);
    char save_variables_type[VARIABLES_TYPE_LENGTH+1]={0};
    unsigned long save_variables_size=0;
    unsigned long save_variables_offset=0;
    char calculate_left[VARIABLES_NAME_LENGTH]={0};
    char calculate_left_type[VARIABLES_TYPE_LENGTH+1]={0};
    unsigned long calculate_left_size=0;
    unsigned long calculate_left_offset=0;
    char calculate_right[VARIABLES_NAME_LENGTH]={0};
    char calculate_right_type[VARIABLES_TYPE_LENGTH+1]={0};
    unsigned long calculate_right_size=0;
    unsigned long calculate_right_offset=0;
    long calculate_left_=0;
    long calculate_right_=0;
    calcu_type return_result=calcu_err;
    char* string_buffer=NULL;

    if (!is_valid_calculate(in_calcu_expression)) return false;
    if (!is_valid_variables(save_variables_name,save_variables_type,&save_variables_size,&save_variables_offset)) return false;
    for (;find_flag_index<string_length-1;++find_flag_index) {
        if ('+'==*(char*)(in_calcu_expression+find_flag_index))
            return_result=calcu_add;
        else if ('-'==*(char*)(in_calcu_expression+find_flag_index))
            return_result=calcu_dec;
        else if ('*'==*(char*)(in_calcu_expression+find_flag_index))
            return_result=calcu_mul;
        else if ('/'==*(char*)(in_calcu_expression+find_flag_index))
            return_result=calcu_div;
        if (return_result) {
            memcpy(calculate_left,in_calcu_expression,find_flag_index);
            memcpy(calculate_right,(const void*)(in_calcu_expression+find_flag_index+1),string_length-find_flag_index-1);
            if (is_valid_variables(calculate_left,calculate_left_type,&calculate_left_size,&calculate_left_offset)) {
                if (is_valid_variables(calculate_right,calculate_right_type,&calculate_right_size,&calculate_right_offset)) {
                    if (strcmp(calculate_left_type,calculate_right_type)) return false;
                    if (strcmp(calculate_left_type,save_variables_type)) return false;
                    if (!strcmp(calculate_left_type,"long")) {
                        calculate_left_=read_stack_long(calculate_left_offset);
                        calculate_right_=read_stack_long(calculate_right_offset);
                    } else if (!strcmp(calculate_left_type,"double")) {
                        calculate_left_=read_stack_float(calculate_left_offset);
                        calculate_right_=read_stack_float(calculate_right_offset);
                    } else if (!strcmp(calculate_left_type,"char")) {
                        if (calcu_mul==return_result || calcu_div==return_result) return false;
                        calculate_left_=read_stack_char(calculate_left_offset);
                        calculate_right_=read_stack_char(calculate_right_offset);
                    } else if (!strcmp(calculate_left_type,"string")) {
                        if (calcu_add!=return_result) return false;
                        string_buffer=(char*)malloc(calculate_right_size);
                        if (NULL==string_buffer) return false;
                        memset(string_buffer,0,calculate_right_size);
                        read_stack_string(calculate_right_offset,calculate_right_size,string_buffer);
                        write_stack_string(calculate_left_offset,(strlen(string_buffer)>=calculate_left_size)?calculate_left_size:strlen(string_buffer),string_buffer);
                        free(string_buffer);
                        return true;
                    }
                    switch (return_result) {
                        case calcu_add:
                            if (!strcmp(calculate_left_type,"char")) {
                                calculate_left_=(calculate_left_+calculate_right_<=0xFF)?calculate_left_+calculate_right_:0xFF;
                                write_stack_char(save_variables_offset,*(char*)&calculate_left_);
                                return true;
                            }
                            calculate_left_+=calculate_right_;
                            write_stack_long(save_variables_offset,calculate_left_);
                            return true;
                        case calcu_dec:
                            if (strcmp(calculate_left_type,"char")) {
                                calculate_left_=(calculate_left_-calculate_right_>=0x00)?calculate_left_-calculate_right_:0x00;
                                write_stack_char(save_variables_offset,*(char*)&calculate_left_);
                                return true;
                            }
                            calculate_left_-=calculate_right_;
                            write_stack_long(save_variables_offset,calculate_left_);
                            return true;
                        case calcu_mul:
                            calculate_left_*=calculate_right_;
                            write_stack_long(save_variables_offset,calculate_left_);
                            return true;
                        case calcu_div:
                            calculate_left_/=calculate_right_;
                            write_stack_long(save_variables_offset,calculate_left_);
                            return true;
                    }
                    return true;
                } else if (is_number(calculate_right)) {
                    calculate_left_=read_stack_long(calculate_left_offset);
                    calculate_right_=string_to_number(calculate_right);
                    switch (return_result) {
                        case calcu_add:
                            calculate_left_+=calculate_right_;
                            break;
                        case calcu_dec:
                            calculate_left_-=calculate_right_;
                            break;
                        case calcu_mul:
                            calculate_left_*=calculate_right_;
                            break;
                        case calcu_div:
                            calculate_left_/=calculate_right_;
                            break;
                    }
                    write_stack_long(save_variables_offset,calculate_left_);
                    return true;
                } else if (is_float(calculate_right)) {
                    calculate_left_=read_stack_long(calculate_left_offset);
                    calculate_right_=string_to_number(calculate_right);
                    switch (return_result) {
                        case calcu_add:
                            calculate_left_+=calculate_right_;
                            break;
                        case calcu_dec:
                            calculate_left_-=calculate_right_;
                            break;
                        case calcu_mul:
                            calculate_left_*=calculate_right_;
                            break;
                        case calcu_div:
                            calculate_left_/=calculate_right_;
                            break;
                    }
                    write_stack_float(save_variables_offset,*(float*)&calculate_left_);
                    return true;
                } else if (is_char(calculate_right)) {
                    calculate_left_=read_stack_long(calculate_left_offset);
                    calculate_right_=string_to_number(calculate_right);
                    switch (return_result) {
                        case calcu_add:
                            calculate_left_+=calculate_right_;
                            break;
                        case calcu_dec:
                            calculate_left_-=calculate_right_;
                            break;
                    }
                    write_stack_char(save_variables_offset,*(char*)&calculate_left_);
                    return true;
                } else if (is_string(calculate_right)) {
                    return false;  //  WARNING
                }
                return false;
            } else if (is_number(calculate_left) && is_number(calculate_right)) {
                calculate_left_=string_to_number(calculate_left);
                calculate_right_=string_to_number(calculate_right);
                switch (return_result) {
                    case calcu_add:
                        calculate_left_+=calculate_right_;
                        break;
                    case calcu_dec:
                        calculate_left_-=calculate_right_;
                        break;
                    case calcu_mul:
                        calculate_left_*=calculate_right_;
                        break;
                    case calcu_div:
                        calculate_left_/=calculate_right_;
                        break;
                }
                write_stack_long(save_variables_offset,calculate_left_);
                return true;
            } else if (is_float(calculate_left) && is_float(calculate_right)) {
                calculate_left_=string_to_number(calculate_left);
                calculate_right_=string_to_number(calculate_right);
                switch (return_result) {
                    case calcu_add:
                        calculate_left_+=calculate_right_;
                        break;
                    case calcu_dec:
                        calculate_left_-=calculate_right_;
                        break;
                    case calcu_mul:
                        calculate_left_*=calculate_right_;
                        break;
                    case calcu_div:
                        calculate_left_/=calculate_right_;
                        break;
                }
                write_stack_long(save_variables_offset,calculate_left_);
                return true;
            } else if (is_char(calculate_left) && is_char(calculate_right) && (calcu_add==return_result || calcu_dec==return_result)) {
                calculate_left_=string_to_number(calculate_left);
                calculate_right_=string_to_number(calculate_right);
                switch (return_result) {
                    case calcu_add:
                        calculate_left_=(calculate_left_+calculate_right_<=0xFF)?calculate_left_+calculate_right_:0xFF;
                        break;
                    case calcu_dec:
                        calculate_left_=(calculate_left_-calculate_right_>=0x00)?calculate_left_+calculate_right_:0x00;
                        break;
                }
                write_stack_char(save_variables_offset,*(char*)&calculate_left_);
                return true;
            } else if (is_string(calculate_left) && is_string(calculate_right) && calcu_add==return_result) {
                if (strlen(calculate_left)<=save_variables_size) {
                    write_stack_string(save_variables_offset,strlen(calculate_left),calculate_left);
                    if (strlen(calculate_left)+strlen(calculate_right)<=save_variables_size)
                        write_stack_string(save_variables_offset,strlen(calculate_left),calculate_left);
                    else
                        write_stack_string(save_variables_offset,save_variables_size-(strlen(calculate_left)+strlen(calculate_right)),calculate_right);
                    return true;
                } else {
                    write_stack_string(save_variables_offset,save_variables_size-strlen(calculate_left),calculate_left);
                    return true;
                }
            }
            return false;
        }
    }
    return false;
}

static bool execute_call_funtion(const char* function_expression,unsigned long* function_return) {
    //  其实这个的结构和is_valid_funtion_expression 一样,你要理解我啊,语法器本来就不应该有的(只是提供给IDE 用)..  2014.8.4
    unsigned long find_left_parenthesis_index=0;
    unsigned long find_right_parenthesis_index=0;
    unsigned long find_left_comma_index=0;
    unsigned long find_right_comma_index=0;
    unsigned long find_length=strlen(function_expression);
    bool resolve_stat=false;
    char function_name[FUNCTION_NAME_LENGTH]={0};
    char function_parameters[FUNCTION_PARAMETERS_MAX][FUNCTION_PARAMETERS_LENGTH+1]={0};
    char function_parameters_type[VARIABLES_TYPE_LENGTH+1]={0};
    long function_number_parameters_buffer=0;
    unsigned long function_parameters_convert_index=0;
    unsigned long function_parameters_offset=0;
    unsigned long function_parameters_size=0;
    unsigned long function_parameters_count=0;

    for (;find_left_parenthesis_index<find_length;++find_left_parenthesis_index) {
        if ('('==*(char*)(function_expression+find_left_parenthesis_index)) {
            memcpy(function_name,function_expression,find_left_parenthesis_index);
            for (find_right_parenthesis_index=find_left_parenthesis_index+1;find_right_parenthesis_index<find_length;++find_right_parenthesis_index) {
                if (')'==*(char*)(function_expression+find_right_parenthesis_index)) {
                    if (find_left_parenthesis_index==(find_right_parenthesis_index+1)) {
                        resolve_stat=true;
                        break;
                    }
                    for (find_right_comma_index=find_left_parenthesis_index+1,find_left_comma_index=find_left_parenthesis_index;find_right_comma_index<find_length;++find_right_comma_index) {
                        if ((','==*(char*)(function_expression+find_right_comma_index)) && (find_right_comma_index-find_left_comma_index)) {
                            memcpy(function_parameters[function_parameters_count],(const void*)((unsigned long)function_expression+find_left_comma_index+1),find_right_comma_index-find_left_comma_index-1);
                            ++function_parameters_count;
                            find_left_comma_index=find_right_comma_index;
                        } else if ((')'==*(char*)(function_expression+find_right_comma_index)) && (find_right_comma_index-find_left_comma_index-1)) {
                            memcpy(function_parameters[function_parameters_count],(const void*)((unsigned long)function_expression+find_left_comma_index+1),find_right_comma_index-find_left_comma_index-1);
                            ++function_parameters_count;
                            resolve_stat=true;
                            break;
                        }
                    }
                }
                if (resolve_stat) {
                    if (!strcmp("msgbox",function_name)) {
                        for (;function_parameters_convert_index<=function_parameters_count;++function_parameters_convert_index) {
                            if (is_valid_variables(function_parameters[function_parameters_convert_index],function_parameters_type,&function_parameters_size,&function_parameters_offset)) {
                                memset(function_parameters[function_parameters_convert_index],0,strlen(function_parameters[function_parameters_convert_index]));
                                if (!strcmp(function_parameters_type,"long")) {
                                    function_number_parameters_buffer=read_stack_long(function_parameters_offset);
                                    number_to_string(function_number_parameters_buffer,function_parameters[function_parameters_convert_index]);
                                } else if (!strcmp(function_parameters_type,"double")) {
                                    function_number_parameters_buffer=(long)read_stack_float(function_parameters_offset);
                                    number_to_string(function_number_parameters_buffer,function_parameters[function_parameters_convert_index]);
                                } else if (!strcmp(function_parameters_type,"char")) {
                                    function_number_parameters_buffer=read_stack_char(function_parameters_offset);
                                    function_parameters[function_parameters_convert_index][0]=(char)function_number_parameters_buffer;
                                } else if (!strcmp(function_parameters_type,"string")) {
                                    read_stack_string(function_parameters_offset,function_parameters_size,function_parameters[function_parameters_convert_index]);
                                }
                                memset(function_parameters_type,0,strlen(function_parameters_type));
                            } else if (is_string(function_parameters[function_parameters_convert_index])) {
                                memcpy(function_parameters[function_parameters_convert_index],function_parameters[function_parameters_convert_index]+1,strlen(function_parameters[function_parameters_convert_index])-1);
                                *(char*)(function_parameters[function_parameters_convert_index]+strlen(function_parameters[function_parameters_convert_index])-1)='\0';
                                *(char*)(function_parameters[function_parameters_convert_index]+strlen(function_parameters[function_parameters_convert_index])-1)='\0';
                            } else if (is_char(function_parameters[function_parameters_convert_index])) {
                                function_parameters[function_parameters_convert_index][0]=function_parameters[function_parameters_convert_index][1];
                                function_parameters[function_parameters_convert_index][1]='\0';
                                function_parameters[function_parameters_convert_index][2]='\0';
                            } else if (is_number(function_parameters[function_parameters_convert_index])) {
                                function_number_parameters_buffer=string_to_number(function_parameters[function_parameters_convert_index]);
                                *(long*)function_parameters[function_parameters_convert_index]=function_number_parameters_buffer;
                            } else if (is_float(function_parameters[function_parameters_convert_index])) {
                                function_number_parameters_buffer=string_to_number(function_parameters[function_parameters_convert_index]);
                                *(float*)function_parameters[function_parameters_convert_index]=function_number_parameters_buffer;
                            }
                        }
                        if (NULL==function_return)
                            messagebox(NULL,function_parameters[0],function_parameters[1],*(messagebox_style*)function_parameters[2]);
                        else
                            *function_return=messagebox(NULL,function_parameters[0],function_parameters[1],*(messagebox_style*)function_parameters[2]);
                    }
                    return true;
                }
            }
            return resolve_stat;
        }
    }
    return resolve_stat;
}
static bool execute_script_line(const char* script_line) {
    char code_line[SCRIPT_LINE_LENGTH]={0};
    char code_line_additional[SCRIPT_LINE_LENGTH]={0};
    char code_variables_name[VARIABLES_NAME_LENGTH]={0};
    char code_variables_type[VARIABLES_TYPE_LENGTH+1]={0};
    unsigned long code_variables_stack_offset=0;
    unsigned long code_variables_size=0;
    char code_variables_type_right[VARIABLES_TYPE_LENGTH+1]={0};
    unsigned long code_variables_stack_offset_right=0;
    unsigned long code_variables_size_right=0;
    assignment_type code_variables_type_code=error_type;
    char code_term[VARIABLES_NAME_LENGTH*2+1]={0};
    char code_term_number[VARIABLES_NAME_LENGTH/2]={0};
    char code_final_term[VARIABLES_NAME_LENGTH/2]={0};
    char code_step[VARIABLES_NAME_LENGTH/2]={0};
    long code_loop_init_number=0;
    long code_loop_final_number=0;
    long code_loop_step_number=0;

    memcpy(code_line,script_line,strlen(script_line));
    if (check_syntax_line(code_line)) {
        switch (resolve_code_line_type(code_line)) {
            case variables_declare:
                //  代码段中不能再有变量声明
                return false;
            case if_determine:
                resolve_if_determine(code_line,code_term,code_line_additional);
                if (calcu_term_result(code_term))
                    return execute_script_line(code_line_additional);
                return true;
            case while_loop:
                resolve_while_loop(code_line,code_term,code_line_additional);
                while (calcu_term_result(code_term))
                    if (!execute_script_line(code_line_additional))
                        return false;
                return true;
            case for_loop:
                resolve_for_loop(code_line,code_term,code_final_term,code_step,code_line_additional);
                string_get_equal_left(code_term,code_variables_name);
                string_get_equal_right(code_term,code_term_number);
                if (is_valid_variables(code_variables_name,code_variables_type,&code_variables_size,&code_variables_stack_offset)) {
                    if (execute_script_line(code_term)) {
                        code_loop_init_number=string_to_number(code_term_number);
                        code_loop_final_number=string_to_number(code_final_term);
                        code_loop_step_number=string_to_number(code_step);
                        if (code_loop_init_number<=code_loop_final_number) {
                            for (;code_loop_init_number<=code_loop_final_number;code_loop_init_number+=code_loop_step_number) {
                                write_stack_long(code_variables_stack_offset,code_loop_init_number);
                                if (!execute_script_line(code_line_additional))
                                    return false;
                            }
                        } else {
                            for (;code_loop_init_number>=code_loop_final_number;code_loop_init_number+=code_loop_step_number) {
                                write_stack_long(code_variables_stack_offset,code_loop_init_number);
                                if (!execute_script_line(code_line_additional))
                                    return false;
                            }
                        }
                    }
                }
                return true;
            case function_call:
                resolve_call(code_line,code_variables_name,code_line_additional);
                if (is_valid_variables(code_variables_name,code_variables_type,&code_variables_size,&code_variables_stack_offset)) {
                    if (execute_call_funtion(code_line_additional,(unsigned long*)&code_loop_final_number)) {
                        if (!strcmp(code_variables_type,"long")) {
                            write_stack_long(code_variables_stack_offset,code_loop_final_number);
                            return true;
                        } else if (!strcmp(code_variables_type,"double")) {
                            write_stack_float(code_variables_stack_offset,code_loop_final_number);
                            return true;
                        } else if (!strcmp(code_variables_type,"char")) {
                            write_stack_char(code_variables_stack_offset,(char)code_loop_final_number);
                            return true;
                        } else if (!strcmp(code_variables_type,"string")) {
                            write_stack_long(code_variables_stack_offset,code_loop_final_number);
                            return true;
                        }
                    }
                    return false;
                }
                return execute_call_funtion(code_line_additional,NULL);
            case assignemt:
                resolve_assignment(code_line,code_variables_name,&code_variables_type_code,code_final_term);
                if (is_valid_variables(code_variables_name,code_variables_type,&code_variables_size,&code_variables_stack_offset)) {
                    switch (code_variables_type_code) {
                        case type_long:
                            if (is_number(code_final_term)) {
                                write_stack_long(code_variables_stack_offset,string_to_number(code_final_term));
                                return true;
                            }
                            if (is_string(code_final_term)) {
                                write_stack_long(code_variables_stack_offset,string_to_number(code_final_term));
                                return true;
                            }
                            return false;
                        case type_float:
                            if (is_float(code_final_term)) {
                                write_stack_float(code_variables_stack_offset,string_to_float(code_final_term));
                                return true;
                            }
                            return false;
                        case type_char:
                            if (is_char(code_final_term)) {
                                write_stack_char(code_variables_stack_offset,*(char*)(code_final_term+1));
                                return true;
                            }
                            return false;
                        case type_string:
                            if (is_string(code_final_term)) {
                                clean_string_flag(code_final_term);
                                write_stack_string(code_variables_stack_offset,code_variables_size,code_final_term);
                                return true;
                            }
                            if (is_number(code_final_term)) {
                                write_stack_string(code_variables_stack_offset,code_variables_size,code_final_term);
                                return true;
                            }
                            return false;
                        case type_variables:
                            if (is_valid_variables(code_final_term,code_variables_type_right,&code_variables_size_right,&code_variables_stack_offset_right)) {
                                if (!strcmp(code_variables_type,code_variables_type_right)) {
                                    if (!strcmp(code_variables_type,"long")) {
                                        write_stack_long(code_variables_stack_offset,read_stack_long(code_variables_stack_offset_right));
                                        return true;
                                    }
                                    if (!strcmp(code_variables_type,"double")) {
                                        write_stack_float(code_variables_stack_offset,read_stack_float(code_variables_stack_offset_right));
                                        return true;
                                    }
                                    if (!strcmp(code_variables_type,"char")) {
                                        write_stack_char(code_variables_stack_offset,read_stack_char(code_variables_stack_offset_right));
                                        return true;
                                    }
                                    if (!strcmp(code_variables_type,"string")) {
                                        //  WARNING! ..
                                        return false;
                                    }
                                }
                            } else if (!strcmp(code_variables_type,"long") && !strcmp(code_variables_type_right,"string")) {
                                if (read_stack_long(code_variables_stack_offset)==string_to_number(code_final_term))
                                    return true;
                                return false;
                            } else if (!strcmp(code_variables_type,"string") && !strcmp(code_variables_type_right,"long")) {
                                if (read_stack_long(code_variables_stack_offset_right)==string_to_number(code_variables_name))
                                    return true;
                                return false;
                            }
                            return false;
                        case type_calculate:
                            if (is_valid_calculate(code_final_term))
                                return calcu_expression(code_final_term,code_variables_name);
                            return false;
                    }
                }
                return false;
            case block_end:
                break;
        }
    }
    return false;
}

void clean_script(void) {
    if (NULL!=script_data)
        free(script_data);
    if (NULL!=script_stack) {
        free(script_stack);
        memset(&globel_variables_table,0,sizeof(globel_variables_table));
    }
    script_data_length=0;
    script_stack_size=0;
    script_line=0;
    script_code_line=0;
}

bool load_script(const char* script_path) {
    unsigned long script_execute_line=0;
    char code_line[SCRIPT_LINE_LENGTH]={0};
    bool execute_stat=true;
    FIL* script=NULL;

    if (file_open(script_path,script)) {
        //  Script Code Load
        script_data_length=file_length(script);
        if (NULL!=script_data_length) {
            script_data=(char*)malloc(script_data_length);
            if (NULL!=script_data) {
                memset(script_data,0,script_data_length);
                file_read(script,script_data,script_data_length);
                //   Script Stack Init
                memset(&globel_variables_table,0,sizeof(globel_variables_table));
                script_line=get_script_line();
                script_stack_size=get_script_stack_size();
                if (GET_SCRIPT_STACK_ERROR!=script_stack_size) {
                    script_stack=(char*)malloc(script_stack_size);
                    if (NULL!=script_stack) {
                        memset(script_stack,0,script_stack_size);
                        file_close(script);
                        //  Script Execute ..
                        for (script_execute_line=script_code_line;script_execute_line<script_line;++script_execute_line) {
                            read_script_line(script_execute_line,code_line);
                            if (!execute_script_line(code_line)) {
                                execute_stat=false;
                                break;
                            }
                            memset(code_line,0,SCRIPT_LINE_LENGTH);
                        }
                        if (execute_stat)
                            return true;
                    }
                }
                clean_script();
            }
        }
        file_close(script);
    }
    return false;
}
