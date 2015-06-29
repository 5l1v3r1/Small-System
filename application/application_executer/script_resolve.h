
#ifndef _SCRIPT_RESOLVE_H__
#define _SCRIPT_RESOLVE_H__

#include "system_type.h"

typedef enum {
    calcu_err=0,
    calcu_add,
    calcu_dec,
    calcu_mul,
    calcu_div,
} calcu_type;

void clean_space(char* clean_code_line);
void clean_escape(char* clean_code_line);
void clean_string_flag(char* clean_string_line);
void string_change_low(char* change_string);
void string_get_space_left(const char* in_code_line,char* out_buffer);
void string_get_equal_left(const char* in_code_line,char* out_buffer);
void string_get_equal_right(const char* in_code_line,char* out_buffer);
void string_move_left(char* in_code_line,unsigned long move_length);
bool is_valid_variables_name(const char* variables_name);
bool is_valid_term(const char* code_line);
bool is_valid_function_expression(const char* in_code_line);
calcu_type is_valid_calculate(const char* in_code_line);
bool is_number(const char* in_code_line);
bool is_float(const char* in_code_line);
bool is_char(const char* in_code_line);
bool is_string(const char* in_code_line);
void number_to_string(long in_number,char* output_string);
long string_to_number(const char* input_string);
float string_to_float(const char* input_string);

#endif
