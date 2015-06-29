
#ifndef _KERNEL_UI_API_H__
#define _KERNEL_UI_API_H__

void kernel_load_enter(void);
void kernel_load_exit(void);

void kernel_set_load_rate(unsigned int rate);
void kernel_err_report(const char* err_report_title,const char* err_report_detail,const char* error_fix_advise);

#endif
