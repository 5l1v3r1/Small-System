
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "malloc.h"

#include "script_resolve.h"
#include "script_syntax.h"

void clean_space(char* clean_code_line) {
    unsigned long clean_left_space_count=0;
    unsigned long clean_right_space_count=0;
    unsigned long clean_between_space_count=0;
    long clean_repeat_space_index=0;
    unsigned long find_final_between_space_location=0;
    unsigned long code_line_length=strlen(clean_code_line);
    while (' '==*clean_code_line) {
        memcpy(clean_code_line,clean_code_line+1,code_line_length-clean_left_space_count);
        ++clean_left_space_count;
        *(char*)((unsigned long)clean_code_line+code_line_length-clean_left_space_count)='\0';
    }
    while (' '==*(char*)((unsigned long)clean_code_line+code_line_length-clean_left_space_count-clean_right_space_count-1)) {
        *(char*)((unsigned long)clean_code_line+code_line_length-clean_left_space_count-clean_right_space_count-1)='\0';
        ++clean_right_space_count;

    }
    for (;clean_repeat_space_index<(long)(code_line_length-clean_left_space_count-clean_right_space_count-clean_between_space_count-1);++clean_repeat_space_index) {
        if (' '==*(char*)((unsigned long)clean_code_line+clean_repeat_space_index)) {
            for (find_final_between_space_location=clean_repeat_space_index+1;find_final_between_space_location<(code_line_length-clean_left_space_count-clean_right_space_count-clean_between_space_count-1);++find_final_between_space_location)
                if (' '!=*(char*)((unsigned long)clean_code_line+find_final_between_space_location))
                    break;
            if (find_final_between_space_location-clean_repeat_space_index-1) {
                memcpy(clean_code_line+clean_repeat_space_index+1,clean_code_line+find_final_between_space_location,code_line_length-clean_repeat_space_index-1);
                clean_between_space_count+=find_final_between_space_location-clean_repeat_space_index-1;
            }
        }
    }
}
void clean_escape(char* clean_code_line) {
    unsigned long clean_left_escape_count=0;
    unsigned long clean_right_escape_count=0;
    unsigned long code_line_length=strlen(clean_code_line);
    while ('\t'==*clean_code_line) {
        memcpy(clean_code_line,clean_code_line+1,code_line_length-clean_left_escape_count);
        ++clean_left_escape_count;
        *(char*)((unsigned long)clean_code_line+code_line_length-clean_left_escape_count)='\0';
    }
    while ('\t'==*(char*)((unsigned long)clean_code_line+code_line_length-clean_left_escape_count-clean_right_escape_count-1)) {
        *(char*)((unsigned long)clean_code_line+code_line_length-clean_left_escape_count-clean_right_escape_count-1)='\0';
        ++clean_right_escape_count;
    }
}
void clean_string_flag(char* clean_string_line) {
    if (is_string(clean_string_line)) {
        *(clean_string_line+strlen(clean_string_line)-1)='\0';
        memcpy(clean_string_line,clean_string_line+1,strlen(clean_string_line)-1);
        *(clean_string_line+strlen(clean_string_line)-1)='\0';
    }
}
void string_change_low(char* change_string) {
    unsigned long change_index=0;
    unsigned long change_string_length=strlen(change_string);
    char* change_point=change_string;
    
    for (;change_index<change_string_length;++change_index,++change_point)
        if (('A'<=*change_point) && (*change_point<='Z')) *change_point+=32;
}
void string_get_space_left(const char* in_code_line,char* out_buffer) {
    unsigned long find_space=0;
    unsigned long string_length=strlen(in_code_line);

    for (;find_space<string_length;++find_space) {
        if (' '==*(char*)((unsigned long)in_code_line+find_space)) {
            memcpy(out_buffer,in_code_line,find_space);
            return;
        }
    }
}
void string_get_equal_left(const char* in_code_line,char* out_buffer) {
    unsigned long find_equal=0;
    unsigned long string_length=strlen(in_code_line);

    for (;find_equal<string_length;++find_equal) {
        if ('='==*(char*)((unsigned long)in_code_line+find_equal)) {
            memcpy(out_buffer,in_code_line,find_equal);
            return;
        }
    }
}
void string_get_equal_right(const char* in_code_line,char* out_buffer) {
    unsigned long find_equal=0;
    unsigned long string_length=strlen(in_code_line);

    for (;find_equal<string_length;++find_equal) {
        if ('='==*(char*)((unsigned long)in_code_line+find_equal)) {
            memcpy(out_buffer,(const void*)((unsigned long)in_code_line+find_equal+1),string_length-find_equal-1);
            return;
        }
    }
}
void string_move_left(char* in_code_line,unsigned long move_length) {
    unsigned long string_length=strlen(in_code_line);

    if (string_length>move_length) {
        memcpy(in_code_line,(const void*)((unsigned long)in_code_line+move_length),string_length-move_length);
        memset((void*)((unsigned long)in_code_line+string_length-move_length),0,move_length);
    }
}
bool is_valid_variables_name(const char* variables_name) {
    unsigned long check_index=0;
    unsigned long string_length=strlen(variables_name);
    char check_char='\0';

    if (VARIABLES_NAME_LENGTH<string_length || !string_length) return false;
    for (;check_index<string_length;++check_index) {
        check_char=*(variables_name+check_index);
        if (('a'<=check_char && check_char<='z') || ('A'<=check_char && check_char<='Z') || '_'==check_char)
            continue;
        else
            return false;
    }
    return true;

}
bool is_valid_term(const char* code_line) {
    unsigned long find_index=0;
    unsigned long find_length=strlen(code_line);
    char find_char='\0';
    char term_left[VARIABLES_NAME_LENGTH]={0};
    char term_right[VARIABLES_NAME_LENGTH]={0};

    for (;find_index<find_length;++find_index) {
        find_char=*(char*)(code_line+find_index);
        if (('+'==find_char) || ('-'==find_char) || ('*'==find_char) || ('/'==find_char) || ('='==find_char) || ('<'==find_char) || ('>'==find_char)) {
            memcpy(term_left,code_line,find_index);
            memcpy(term_right,code_line+find_index+1,find_length-find_index-1);
            if (is_valid_variables_name(term_left)) {
                if (is_valid_variables_name(term_right)) {
                    return true;
                } else if (is_number(term_right)) {
                    return true;
                } else if (is_float(term_right)) {
                    return true;
                } else if (is_char(term_right)) {
                    return true;
                } else if (is_string(term_right)) {
                    return true;
                }
            }
        }
        if (('<'==find_char && '='==*(char*)(code_line+find_index+1)) || ('>'==find_char && '='==*(char*)(code_line+find_index+1)) || ('<'==find_char && '>'==*(char*)(code_line+find_index+1))) {
            memset(term_left,0,VARIABLES_NAME_LENGTH);
            memset(term_right,0,VARIABLES_NAME_LENGTH);
            memcpy(term_left,code_line,find_index);
            memcpy(term_right,code_line+find_index+2,find_length-find_index-2);
            if (is_valid_variables_name(term_left)) {
                if (is_valid_variables_name(term_right)) {
                    return true;
                } else if (is_number(term_right)) {
                    return true;
                } else if (is_float(term_right)) {
                    return true;
                } else if (is_char(term_right)) {
                    return true;
                } else if (is_string(term_right)) {
                    return true;
                }
            }
            return false;
        }
    }
    return false;
}
static bool is_valid_function(const char* in_code_line) {
    unsigned long find_left_parenthesis_index=0;
    unsigned long find_right_parenthesis_index=0;
    unsigned long find_left_comma_index=0;
    unsigned long find_right_comma_index=0;
    unsigned long find_length=strlen(in_code_line);

    for (;find_left_parenthesis_index<find_length;++find_left_parenthesis_index) {
        if ('('==*(char*)(in_code_line+find_left_parenthesis_index)) {
            for (find_right_parenthesis_index=find_left_parenthesis_index+1;find_right_parenthesis_index<find_length;++find_right_parenthesis_index) {
                if (')'==*(char*)(in_code_line+find_right_parenthesis_index)) {
                    if (find_left_parenthesis_index==(find_right_parenthesis_index+1))
                        return true;
                    for (find_right_comma_index=find_right_parenthesis_index+1,find_left_comma_index=find_right_comma_index;find_right_comma_index<find_length;++find_right_comma_index) {
                        if ((','==*(char*)(in_code_line+find_right_comma_index)) && (find_right_comma_index-find_left_comma_index))
                            find_left_comma_index=find_right_comma_index;
                        else if ((')'==*(char*)(in_code_line+find_right_comma_index)) && (find_right_comma_index-find_left_comma_index-1))
                            break;
                        else
                            return false;
                    }
                    return true;
                }
            }
            return false;
        }
    }
    return false;
}
bool is_valid_function_expression(const char* in_code_line) {
    unsigned long find_equal_index=0;
    unsigned long find_length=strlen(in_code_line);
    char variables_name[VARIABLES_NAME_LENGTH]={0};

    for (;find_equal_index<find_length;++find_equal_index) {
        if ('='==*(char*)(in_code_line+find_equal_index)) {
            string_get_equal_left(in_code_line,variables_name);
            if (is_valid_variables_name(variables_name))
                return is_valid_function(in_code_line+find_equal_index+1);
        }
    }
    return is_valid_function(in_code_line);
}
bool is_number(const char* in_code_line) {
    unsigned long check_index=0;
    unsigned long string_length=strlen(in_code_line);

    if ('-'==*(char*)in_code_line)
        ++check_index;
    for (;check_index<string_length;++check_index)
        if (*(in_code_line+check_index)<'0' || '9'<*(in_code_line+check_index))
            return false;
    return true;
}
bool is_float(const char* in_code_line) {
    unsigned long check_index=0;
    unsigned long string_length=strlen(in_code_line);

    for (;check_index<string_length;++check_index) {
        if ('0'<*in_code_line || *in_code_line<'9' || '.'==*in_code_line) {
            if (check_index==string_length && '.'==*in_code_line)
                return false;
        } else
            return false;
    }
    return true;
}
bool is_char(const char* in_code_line) {
    if (3==strlen(in_code_line))
        if ('\''==*in_code_line && '\''==*(in_code_line+2))
            return true;
    return false;
}
bool is_string(const char* in_code_line) {
    unsigned long check_index=1;
    unsigned long string_length=strlen(in_code_line);

    if (string_length<2) return false;
    if ('"'==*in_code_line && '"'==*(in_code_line+string_length-1)) {
        for (;check_index<string_length-1;++check_index)
            if ('"'==(*in_code_line+check_index))
                return false;
        return true;
    }
    return false;
}
calcu_type is_valid_calculate(const char* in_code_line) {
    unsigned long find_flag_index=0;
    unsigned long string_length=strlen(in_code_line);
    char calculate_left[VARIABLES_NAME_LENGTH]={0};
    char calculate_right[VARIABLES_NAME_LENGTH]={0};
    calcu_type return_result=calcu_err;

    for (;find_flag_index<string_length-1;++find_flag_index) {
        if ('+'==*(char*)(in_code_line+find_flag_index))
            return_result=calcu_add;
        else if ('-'==*(char*)(in_code_line+find_flag_index))
            return_result=calcu_dec;
        else if ('*'==*(char*)(in_code_line+find_flag_index))
            return_result=calcu_mul;
        else if ('/'==*(char*)(in_code_line+find_flag_index))
            return_result=calcu_div;
        if (return_result) {
            memcpy(calculate_left,in_code_line,find_flag_index-1);
            memcpy(calculate_right,(const void*)(in_code_line+find_flag_index+1),string_length-find_flag_index-1);
            if (is_valid_variables_name(calculate_left)) {
                if (is_valid_variables_name(calculate_right))
                    return return_result;
                else if (is_number(calculate_right))
                    return return_result;
                else if (is_float(calculate_right))
                    return return_result;
                else if (is_char(calculate_right))
                    return return_result;
                else if (is_string(calculate_right))
                    return return_result;
                return calcu_err;
            } else if (is_number(calculate_left) && is_number(calculate_right))
                return return_result;
            else if (is_float(calculate_left) && is_float(calculate_right))
                return return_result;
            else if (is_char(calculate_left) && is_char(calculate_right) && (calcu_add==return_result || calcu_dec==return_result))
                return return_result;
            else if (is_string(calculate_left) && is_string(calculate_right) && calcu_add==return_result)
                return return_result;
            return calcu_err;
        }
    }
    return calcu_err;
}
void number_to_string(long in_number,char* output_string) {
    sprintf(output_string,"%ld",in_number);
}
long string_to_number(const char* input_string) {
    long return_number=0;
    int number_index=strlen(input_string)-1;

    if ('-'!=*(char*)input_string) {
        for (;number_index>=0;--number_index,++input_string)
            return_number+=(*input_string-48)*(long)pow(10,number_index);
    } else {
        for (++input_string,--number_index;number_index>=0;--number_index,++input_string)
            return_number+=(*input_string-48)*(long)pow(10,number_index);
        return_number-=2*return_number;
    }

    return return_number;
}
float string_to_float(const char* input_string) {
    long return_number=0;
    int number_index=strlen(input_string)-1;

    for (;number_index>=0;--number_index,++input_string)
        return_number+=(*input_string-48)*(long)pow(10,number_index);
    return return_number;
}
