
#ifndef _SCRIPT_SYNTAX_H__
#define _SCRIPT_SYNTAX_H__

#include "system_type.h"

#define VARIABLES_NAME_LENGTH  16
#define VARIABLES_TYPE_LENGTH   6

#define FUNCTION_NAME_LENGTH   16
#define FUNCTION_PARAMETERS_MAX 6
#define FUNCTION_PARAMETERS_LENGTH 8

#define SCRIPT_COMMAND_LENGTH   6
#define SCRIPT_LINE_LENGTH     64

/*

    解析器语法:
        脚本结构:
            DATA SECTION:
            数据声名
            DATA END
            脚本代码

        基本语法:
            dim 变量名 as 数据类型
            变量名1=变量名2+|-|*|\变量名3

            if 条件 then 代码
            if 条件 then
                代码行1
                代码行2
                ...
          [[else if 条件2 then
                代码行1
                代码行2
                ...]
            else
                代码行1
                代码行2
                ...]
            end if

            while 条件 代码行
            while 条件
                代码行1
                代码行2
                ...
            end while

            for 初值 to 终值 step 步增值 代码行
            for 初值 to 终值 step 步增值
                代码行1
                代码行2
                ...
            end for

            call kernel_function(parameter1,parameter2...)
            call variables=kernel_function(parameter1,parameter2...)
        
        数据类型:
            LONG:32 位整数
            DOUBLE:32 位浮点数
            CHAR:8 位字符
            STRING [STRING_LENGTH]:STRING_LENGTH*8 位长字符串

        条件判断:
            var1 =  var2            等于
            var1 <> var2          不等于
            var1 >  var2            大于
            var1 <  var2            小于
            var1 >= var2        大于等于
            var1 <= var2        小于等于

*/

bool check_syntax_line(char* check_code_line);

typedef enum {
    err_type=0,
    variables_declare,
    if_determine,
    while_loop,
    for_loop,
    function_call,
    assignemt,
    block_end,
} code_line_type;

typedef enum {
    error_type=0,
    type_long,
    type_float,
    type_char,
    type_string,
    type_variables,
    type_function,
    type_calculate,
} assignment_type;

code_line_type resolve_code_line_type(char* in_code_line);
bool resolve_variables_declare(char* in_code_line,char* out_variables_name,char* out_variables_type,unsigned long* out_variables_type_size);
bool resolve_if_determine(char* in_code_line,char* out_term,char* out_code_line);
bool resolve_while_loop(char* in_code_line,char* out_term,char* out_code_line);
bool resolve_for_loop(char* in_code_line,char* out_init_term,char* out_final_term,char* out_step_value,char* out_code_line);
bool resolve_call(char* in_code_line,char* out_variables_name,char* out_function_expression);
bool resolve_assignment(char* in_code_line,char* out_assignment_name,assignment_type* out_assignment_type,char* out_assignment_data);

#endif
