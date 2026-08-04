/**
 * @file ls_image.h
 *
 */

#ifndef LS_IMAGE_H
#define LS_IMAGE_H

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

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
	LS_IMAGE_COLOR_FORMAT_A8,
	LS_IMAGE_COLOR_FORMAT_ARGB8888,
	LS_IMAGE_COLOR_FORMAT_RGB565,
}ls_image_color_format_t;

typedef struct {
    ls_frame_buffer_color_t color_format;
    int32_t width;
    int32_t height;
    int32_t stride;
    const void * data;
}ls_image_dsc_t;

typedef struct
{
	ls_widget_common_t common;
	uint32_t x;
	uint32_t y;
	const ls_image_dsc_t * src;
	uint8_t opa;
	ls_color_t a8_color;
	int32_t pivot_x;
	int32_t pivot_y;
	int32_t rotation;
}ls_image_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

ls_error_code_t ls_image_create(ls_screen_t * screen, ls_image_t * image);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LS_IMAGE_H*/
