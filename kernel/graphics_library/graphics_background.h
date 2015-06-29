
#ifndef _GRAPHICS_BACKGROUND_H__
#define _GRAPHICS_BACKGROUND_H__

#include "system_type.h"

void graphics_background_clean(void);
void graphics_background_init(void);
bool graphics_background_load(const char* file_path);
void graphics_draw_background(void);

#endif
