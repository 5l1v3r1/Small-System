
#include <string.h>

#include "malloc.h"

#include "script_resolve.h"
#include "script_syntax.h"

static unsigned long get_variables_type_size(char* variables_type) {
    char code_variables_type[VARIABLES_TYPE_LENGTH+1]={0};

    memcpy(code_variables_type,variables_type,VARIABLES_TYPE_LENGTH);
    string_change_low(code_variables_type);
    if (0==strcmp(code_variables_type,"long")) {
        return 4;
    } else if (0==strcmp(code_variables_type,"double")) {
        return 4;
    } else if (0==strcmp(code_variables_type,"char")) {
        return 1;
    } else if (0==strcmp(code_variables_type,"string")) {
        return 0;
    } else
        return 0;
}

code_line_type resolve_code_line_type(char* in_code_line) {
    char code_command[SCRIPT_COMMAND_LENGTH+1]={0};
    char code_variables_name[VARIABLES_NAME_LENGTH]={0};

    clean_escape(in_code_line);
    clean_space(in_code_line);
    string_change_low(in_code_line);
    string_get_space_left(in_code_line,code_command);

    if (0==strcmp("dim",code_command))
        return variables_declare;
    else if (0==strcmp("if",code_command))
        return if_determine;
    else if (0==strcmp("while",code_command))
        return while_loop;
    else if (0==strcmp("for",code_command))
        return for_loop;
    else if (0==strcmp("end",code_command))
        return block_end;
    else if (0==strcmp("call",code_command))
        return function_call;
    else {
        string_get_equal_left(in_code_line,code_variables_name);
        if (is_valid_variables_name(code_variables_name))
            return assignemt;
    }
    return err_type;
}
bool resolve_variables_declare(char* in_code_line,char* out_variables_name,char* out_variables_type,unsigned long* out_variables_type_size) {
    char code_line[SCRIPT_LINE_LENGTH]={0};
    char code_command[SCRIPT_COMMAND_LENGTH]={0};
    char code_variables_name[VARIABLES_NAME_LENGTH]={0};
    char code_variables_type[VARIABLES_TYPE_LENGTH+3]={0};
    char code_other[VARIABLES_NAME_LENGTH/2]={0};

    clean_escape(in_code_line);
    clean_space(in_code_line);
    memcpy(code_line,in_code_line,strlen(in_code_line));
    string_change_low(code_line);
    string_get_space_left(code_line,code_command);
    clean_space(code_command);

    if (0==strcmp("dim",code_command)) {
        string_move_left(code_line,4);
        string_get_space_left(code_line,code_variables_name);
        if (is_valid_variables_name(code_variables_name)) {
            string_move_left(code_line,strlen(code_variables_name)+1);
            string_get_space_left(code_line,code_other);
            clean_space(code_other);
            if (0==strcmp("as",code_other)) {
                string_move_left(code_line,3);
                memcpy(code_variables_type,code_line,strlen(code_line));
                *(char*)(code_variables_type+VARIABLES_TYPE_LENGTH)='\0';
                if ((0==strcmp("long",code_variables_type)) || (0==strcmp("double",code_variables_type)) || (0==strcmp("char",code_variables_type))) {
                    memcpy(out_variables_name,code_variables_name,strlen(code_variables_name));
                    memcpy(out_variables_type,code_variables_type,strlen(code_variables_type));
                    *out_variables_type_size=get_variables_type_size(code_variables_type);
                    return true;
                }
                if (0==strcmp("string",code_variables_type)) {
                    //  dim 变量名 as string*长度
                    string_move_left(code_line,7);
                    if (is_number(code_line)) {
                        memcpy(out_variables_name,code_variables_name,strlen(code_variables_name));
                        memcpy(out_variables_type,code_variables_type,strlen(code_variables_type));
                        *out_variables_type_size=string_to_number(code_line);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
bool resolve_if_determine(char* in_code_line,char* out_term,char* out_code_line) {
    char code_command[SCRIPT_COMMAND_LENGTH]={0};
    char code_term[VARIABLES_NAME_LENGTH*2+1]={0};
    char code_other[VARIABLES_NAME_LENGTH/2]={0};

    clean_escape(in_code_line);
    clean_space(in_code_line);
    string_change_low(in_code_line);
    string_get_space_left(in_code_line,code_command);
    clean_space(code_command);

    if (0==strcmp("if",code_command)) {
        //  if 条件 then 代码
        string_move_left(in_code_line,3);
        string_get_space_left(in_code_line,code_term);
        string_move_left(in_code_line,strlen(code_term)+1);
        string_get_space_left(in_code_line,code_other);
        clean_space(code_other);
        if ((0==strcmp("then",code_other)) && is_valid_term(code_term)) {
            string_move_left(in_code_line,5);
            if (check_syntax_line(in_code_line)) {
                memcpy(out_term,code_term,strlen(code_term));
                memcpy(out_code_line,in_code_line,strlen(in_code_line));
                return true;
            }
        }
    }
    return false;
}
bool resolve_while_loop(char* in_code_line,char* out_term,char* out_code_line) {
    char code_command[SCRIPT_COMMAND_LENGTH]={0};
    char code_term[VARIABLES_NAME_LENGTH*2+1]={0};

    clean_escape(in_code_line);
    clean_space(in_code_line);
    string_change_low(in_code_line);
    string_get_space_left(in_code_line,code_command);
    clean_space(code_command);

    if (0==strcmp("while",code_command)) {
        //  while 条件 代码行
        string_move_left(in_code_line,6);
        string_get_space_left(in_code_line,code_term);
        string_move_left(in_code_line,strlen(code_term)+1);
        if (check_syntax_line(in_code_line) && is_valid_term(code_term)) {
            memcpy(out_term,code_term,strlen(code_term));
            memcpy(out_code_line,in_code_line,strlen(in_code_line));
            return true;
        }
    }
    return false;
}
bool resolve_for_loop(char* in_code_line,char* out_init_term,char* out_final_term,char* out_step_value,char* out_code_line) {
    char code_command[SCRIPT_COMMAND_LENGTH]={0};
    char code_term[VARIABLES_NAME_LENGTH*2+1]={0};
    char code_final_term[VARIABLES_NAME_LENGTH/2]={0};
    char code_other[VARIABLES_NAME_LENGTH/2]={0};
    char code_step[VARIABLES_NAME_LENGTH/2]={0};

    clean_escape(in_code_line);
    clean_space(in_code_line);
    string_change_low(in_code_line);
    string_get_space_left(in_code_line,code_command);
    clean_space(code_command);

    if (0==strcmp("for",code_command)) {
        //  for 初值 to 终值 step 步增值 代码
        string_move_left(in_code_line,4);
        string_get_space_left(in_code_line,code_term);
        string_move_left(in_code_line,strlen(code_term)+1);
        string_get_space_left(in_code_line,code_other);
        if ((0==strcmp("to",code_other)) && is_valid_term(code_term)) {
            string_move_left(in_code_line,3);
            string_get_space_left(in_code_line,code_final_term);
            string_move_left(in_code_line,strlen(code_final_term)+1);
            string_get_space_left(in_code_line,code_other);
            string_move_left(in_code_line,strlen(code_other)+1);
            if ((0==strcmp("step",code_other)) && is_number(code_final_term)) {
                string_get_space_left(in_code_line,code_step);
                string_move_left(in_code_line,strlen(code_step)+1);
                if (check_syntax_line(in_code_line) && is_number(code_step)) {
                    memcpy(out_init_term,code_term,strlen(code_term));
                    memcpy(out_final_term,code_final_term,strlen(code_final_term));
                    memcpy(out_step_value,code_step,strlen(code_step));
                    memcpy(out_code_line,in_code_line,strlen(in_code_line));
                    return true;
                }
            }
        }
    }
    return false;
}
bool resolve_call(char* in_code_line,char* out_variables_name,char* out_function_expression) {
    unsigned find_equal_index=0;
    unsigned find_length=0;
    char code_command[SCRIPT_COMMAND_LENGTH]={0};
    char code_variables_name[VARIABLES_NAME_LENGTH]={0};

    clean_escape(in_code_line);
    clean_space(in_code_line);
    string_change_low(in_code_line);
    string_get_space_left(in_code_line,code_command);
    clean_space(code_command);

    if (0==strcmp("call",code_command)) {
        //  call kernel_function(parameter1,parameter2...)
        //  call variables=kernel_function(parameter1,parameter2...)
        string_move_left(in_code_line,5);
        if (is_valid_function_expression(in_code_line)) {
            for (find_length=strlen(in_code_line);find_equal_index<find_length;++find_equal_index) {
                if ('='==*(char*)(in_code_line+find_equal_index)) {
                    memcpy(code_variables_name,in_code_line,find_equal_index);
                    if (is_valid_variables_name(code_variables_name)) {
                        memcpy(out_variables_name,in_code_line,find_equal_index);
                        memcpy(out_function_expression,in_code_line+find_equal_index+1,find_length-find_equal_index-1);
                    }
                    return true;
                }
            }
            memcpy(out_function_expression,in_code_line,find_length);
            return true;
        }
    }
    return false;
}
bool resolve_assignment(char* in_code_line,char* out_assignment_name,assignment_type* out_assignment_type,char* out_assignment_data) {
    char code_variables_name[VARIABLES_NAME_LENGTH]={0};

    clean_escape(in_code_line);
    clean_space(in_code_line);
    string_change_low(in_code_line);

    string_get_equal_left(in_code_line,code_variables_name);
    string_move_left(in_code_line,strlen(code_variables_name)+1);
    if (is_valid_variables_name(code_variables_name)) {
        if (is_number(in_code_line)) {
        //  变量1=123
            memcpy(out_assignment_name,code_variables_name,strlen(code_variables_name));
            *out_assignment_type=type_long;
            memcpy(out_assignment_data,in_code_line,strlen(in_code_line));
            return true;
        } else if (is_float(in_code_line)) {
        //  变量1=1.23
            memcpy(out_assignment_name,code_variables_name,strlen(code_variables_name));
            *out_assignment_type=type_float;
            memcpy(out_assignment_data,in_code_line,strlen(in_code_line));
            return true;
        } else if (is_char(in_code_line)) {
        //  变量1='A'
            memcpy(out_assignment_name,code_variables_name,strlen(code_variables_name));
            *out_assignment_type=type_char;
            memcpy(out_assignment_data,in_code_line,strlen(in_code_line));
            return true;
        } else if (is_string(in_code_line)) {
        //  变量1="string!"
            memcpy(out_assignment_name,code_variables_name,strlen(code_variables_name));
            *out_assignment_type=type_string;
            memcpy(out_assignment_data,in_code_line,strlen(in_code_line));
            return true;
        } else if (is_valid_variables_name(code_variables_name)) {
            //  变量1=变量2[+|-|*|/变量3]
            if (is_valid_calculate(in_code_line)) {
                memcpy(out_assignment_name,code_variables_name,strlen(code_variables_name));
                *out_assignment_type=type_calculate;
                memcpy(out_assignment_data,in_code_line,strlen(in_code_line));
            } else {  //  变量1=变量2
                memcpy(out_assignment_name,code_variables_name,strlen(code_variables_name));
                *out_assignment_type=type_variables;
                memcpy(out_assignment_data,in_code_line,strlen(in_code_line));
            }
            return true;
        }
    }
    *out_assignment_type=error_type;
    return false;
}

bool check_syntax_line(char* check_code_line) {
    char code_line[SCRIPT_LINE_LENGTH]={0};
    char code_variables_name[VARIABLES_NAME_LENGTH]={0};
    char code_variables_type[VARIABLES_TYPE_LENGTH+1]={0};
    char code_other[VARIABLES_NAME_LENGTH/2]={0};
    char code_command[SCRIPT_COMMAND_LENGTH]={0};
    char code_term[VARIABLES_NAME_LENGTH*2+1]={0};
    char code_final_term[VARIABLES_NAME_LENGTH/2]={0};
    char code_step[VARIABLES_NAME_LENGTH/2]={0};

    clean_escape(check_code_line);
    clean_space(check_code_line);
    memcpy(code_line,check_code_line,strlen(check_code_line));
    string_change_low(code_line);
    string_get_space_left(code_line,code_command);
    clean_space(code_command);

    if (0==strcmp("dim",code_command)) {
        //  dim 变量名 as 数据类型
        string_move_left(code_line,4);
        string_get_space_left(code_line,code_variables_name);
        if (is_valid_variables_name(code_variables_name)) {
            string_move_left(code_line,strlen(code_variables_name)+1);
            string_get_space_left(code_line,code_other);
            clean_space(code_other);
            if (0==strcmp("as",code_other)) {
                string_move_left(code_line,3);
                memcpy(code_variables_type,code_line,strlen(code_line));
                *(char*)(code_variables_type+VARIABLES_TYPE_LENGTH)='\0';
                if ((0==strcmp("long",code_variables_type)) || (0==strcmp("double",code_variables_type)) || (0==strcmp("char",code_variables_type)))
                    return true;
                if (0==strcmp("string",code_variables_type)) {
                    //  dim 变量名 as string*长度
                    string_move_left(code_line,7);
                    if (is_number(code_line))
                        return true;
                }
            }
        }
    } else if (0==strcmp("if",code_command)) {
        //  if 条件 then 代码
        string_move_left(code_line,3);
        string_get_space_left(code_line,code_term);
        string_move_left(code_line,strlen(code_term)+1);
        string_get_space_left(code_line,code_other);
        clean_space(code_other);
        if ((0==strcmp("then",code_other)) && is_valid_term(code_term)) {
            string_move_left(code_line,5);
            return check_syntax_line(code_line);
        }
    } else if (0==strcmp("while",code_command)) {
        //  while 条件 代码行
        string_move_left(code_line,6);
        string_get_space_left(code_line,code_term);
        string_move_left(code_line,strlen(code_term)+1);
        return (bool)(check_syntax_line(code_line) && is_valid_term(code_term));
    } else if (0==strcmp("for",code_command)) {
        //  for 初值 to 终值 step 步增值 代码
        string_move_left(code_line,4);
        string_get_space_left(code_line,code_term);
        string_move_left(code_line,strlen(code_term)+1);
        string_get_space_left(code_line,code_other);
        if ((0==strcmp("to",code_other)) && is_valid_term(code_term)) {
            string_move_left(code_line,3);
            string_get_space_left(code_line,code_final_term);
            string_move_left(code_line,strlen(code_final_term)+1);
            string_get_space_left(code_line,code_other);
            string_move_left(code_line,strlen(code_other)+1);
            if ((0==strcmp("step",code_other)) && is_number(code_final_term)) {
                string_get_space_left(code_line,code_step);
                string_move_left(code_line,strlen(code_step)+1);
                return (bool)(check_syntax_line(code_line) && is_number(code_step));
            }
        }
    } else if (0==strcmp("end",code_command)) {
        //  end if
        //  end while
        //  end for
        string_move_left(code_line,4);
        if (0==strcmp("if",code_line)) {
            return true;
        } else if (0==strcmp("while",code_line)) {
            return true;
        } else if (0==strcmp("for",code_line)) {
            return true;
        }
    } else if (0==strcmp("call",code_command)) {
        //  call kernel_function(parameter1,parameter2...)
        //  call variables=kernel_function(parameter1,parameter2...)
        string_move_left(code_line,5);
        return is_valid_function_expression(code_line);
    } else {
        //  赋值语句:
        //  变量1=变量2
        //  变量1=变量2[+|-|*|/变量3]
        string_get_equal_left(code_line,code_variables_name);
        string_move_left(code_line,strlen(code_variables_name)+1);
        if (is_valid_variables_name(code_variables_name)) {
            if (is_number(code_line)) {
            //  变量1=123
                    return true;
            } else if (is_float(code_line)) {
            //  变量1=1.23
                    return true;
            } else if (is_char(code_line)) {
            //  变量1='A'
                    return true;
            } else if (is_string(code_line)) {
            //  变量1="string!"
                    return true;
            } else if (is_valid_variables_name(code_variables_name)) {
            //  变量1=变量2
                    return true;
            } else {
            //  变量1=变量2[+|-|*|\变量3]
                if (is_valid_term(code_line))
                    return true;
            }
            return false;
        }
    }
    return false;
}
