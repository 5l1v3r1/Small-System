
#ifndef _GRAPHICS_FONT_H__
#define _GRAPHICS_FONT_H__

#include "graphics_api.h"

#define FONT_HEIGHT 16
#define FONT_WIDTH   8

void graphics_draw_font(const char font,graphics_location x,graphics_location y,color draw_color);
void graphics_draw_font_string(const char* string,graphics_location x,graphics_location y,color draw_color);

#endif
