
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

    �������﷨:
        �ű��ṹ:
            DATA SECTION:
            ��������
            DATA END
            �ű�����

        �����﷨:
            dim ������ as ��������
            ������1=������2+|-|*|\������3

            if ���� then ����
            if ���� then
                ������1
                ������2
                ...
          [[else if ����2 then
                ������1
                ������2
                ...]
            else
                ������1
                ������2
                ...]
            end if

            while ���� ������
            while ����
                ������1
                ������2
                ...
            end while

            for ��ֵ to ��ֵ step ����ֵ ������
            for ��ֵ to ��ֵ step ����ֵ
                ������1
                ������2
                ...
            end for

            call kernel_function(parameter1,parameter2...)
            call variables=kernel_function(parameter1,parameter2...)
        
        ��������:
            LONG:32 λ����
            DOUBLE:32 λ������
            CHAR:8 λ�ַ�
            STRING [STRING_LENGTH]:STRING_LENGTH*8 λ���ַ���

        �����ж�:
            var1 =  var2            ����
            var1 <> var2          ������
            var1 >  var2            ����
            var1 <  var2            С��
            var1 >= var2        ���ڵ���
            var1 <= var2        С�ڵ���

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
