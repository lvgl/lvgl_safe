/**
 * @file ls_label.h
 *
 */

#ifndef LS_LABEL_H
#define LS_LABEL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "ls_screen.h"
#include "ls_widget.h"
#include "ls_color.h"
#include "ls_font.h"
#include "ls_translation.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
	LS_LABEL_ALIGN_LEFT = 0,
	LS_LABEL_ALIGN_CENTER = 1,
	LS_LABEL_ALIGN_RIGHT = 2,
	_LS_LABEL_ALIGN_LAST = 3
}lv_label_align_t;

typedef struct
{
	ls_widget_common_t common;
	uint32_t x;
	uint32_t y;
	uint32_t width;
	uint32_t height;
	uint8_t opa;
	ls_color_t color;
	ls_font_t * font;
	const char * text;
	const ls_translation_t * translation;
	int32_t * bind_int;
	const char * bind_fmt;
	uint32_t letter_space;
	lv_label_align_t align;
	uint32_t underline_thickness;
}ls_label_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

ls_error_code_t ls_label_create(ls_screen_t * screen, ls_label_t * label);

ls_error_code_t ls_label_get_text_size(void * widget, uint32_t * width, uint32_t * height);

ls_error_code_t ls_label_render(ls_display_t * display, void * widget);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LS_LABEL_H*/
