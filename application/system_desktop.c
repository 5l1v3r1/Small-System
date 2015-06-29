
#include "file_api.h"

#include "kernel_ui_api.h"
#include "graphics_window.h"

#include "system_desktop.h"
#include "system_desktop_ui.h"

window_handle desktop_handle=INVALID_WINDOW_HANDLE;

bool system_desktop_init(void) {
    desktop_handle=create_window("system_desktop","System Desktop",true,system_desktop_ui_notice);
    
    if (INVALID_WINDOW_HANDLE!=desktop_handle) {
        system_desktop_ui_init(desktop_handle);
        return true;
    } else
        kernel_err_report("System desktop Init Error","It is a kernel load error!","No advice");
    
    return false;
}
