/**
 * @file ls_image_button.h
 *
 */

#ifndef LS_IMAGE_BUTTON_H
#define LS_IMAGE_BUTTON_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "ls_screen.h"
#include "ls_widget.h"
#include "ls_color.h"
#include "ls_image.h"

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

	const ls_image_dsc_t * src_normal;
	const ls_image_dsc_t * src_pressed;
	const ls_image_dsc_t * src_focused;
	const ls_image_dsc_t * src_pressed_focused;
	const ls_image_dsc_t * src_disabled;

	const ls_image_dsc_t * src_checked;
	const ls_image_dsc_t * src_checked_pressed;
	const ls_image_dsc_t * src_checked_focused;
	const ls_image_dsc_t * src_checked_pressed_focused;
	const ls_image_dsc_t * src_checked_disabled;

	uint32_t focus_index;

	/**
	 * If true the button will use the checked colors
	 */
	bool checked;

	/**
	 * If true the button will use the disabled colors and cannot be clicked
	 */
	bool disabled;

}ls_image_button_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

ls_error_code_t ls_image_button_create(ls_screen_t * screen, ls_image_button_t * image_button);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LS_IMAGE_BUTTON_H*/
