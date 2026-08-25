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
#include "ls_image_dsc.h"

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
