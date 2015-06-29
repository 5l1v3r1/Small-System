
#ifndef _GRAPHLICS_API_H__
#define _GRAPHLICS_API_H__

#include "system_type.h"

void graphics_init(void);

typedef unsigned int graphics_location;
typedef const char* bitmap_data;

#include "graphics_bitmap.h"
#include "graphics_font.h"
#include "graphics_window.h"
#include "graphics_widget.h"

#endif
