/**
 * @file ls_arc.h
 *
 */

#ifndef LS_ARC_H
#define LS_ARC_H

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
	int32_t center_x;
	int32_t center_y;
	uint32_t radius;
	ls_color_t color;
	uint8_t opa;
	uint32_t thickness;
	uint32_t start_deg;
	uint32_t end_deg;
}ls_arc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

ls_error_code_t ls_arc_create(ls_screen_t * screen, ls_arc_t * arc);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LS_ARC_H*/
