/**
 * @file ls_rectangle.h
 *
 */

#ifndef LS_RECTANGLE_H
#define LS_RECTANGLE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "ls_screen.h"
#include "ls_widget.h"
#include "ls_color.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct
{
	ls_widget_common_t common;
	uint32_t x;
	uint32_t y;
	uint32_t width;
	uint32_t height;
	uint8_t bg_opa;
	ls_color_t bg_color;
	ls_color_t border_color;
	uint32_t border_width;
}ls_rectangle_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

ls_error_code_t ls_rectangle_create(ls_screen_t * screen, ls_rectangle_t * rectangle);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LS_RECTANGLE_H*/
