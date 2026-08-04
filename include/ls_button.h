/**
 * @file ls_button.h
 *
 */

#ifndef LS_BUTTON_H
#define LS_BUTTON_H

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

	/* Don't use an array like bg_opa[state] because it can be indexed incorrectly and maps
	 * badly to XML*/
	ls_color_t bg_color_normal;
	ls_color_t bg_color_pressed;
	ls_color_t bg_color_focused;
	ls_color_t bg_color_pressed_focused;
	ls_color_t bg_color_disabled;

	ls_color_t bg_color_checked;
	ls_color_t bg_color_checked_pressed;
	ls_color_t bg_color_checked_focused;
	ls_color_t bg_color_checked_pressed_focused;
	ls_color_t bg_color_checked_disabled;

	uint8_t bg_opa_normal;
	uint8_t bg_opa_pressed;
	uint8_t bg_opa_focused;
	uint8_t bg_opa_pressed_focused;
	uint8_t bg_opa_disabled;

	uint8_t bg_opa_checked;
	uint8_t bg_opa_checked_pressed;
	uint8_t bg_opa_checked_focused;
	uint8_t bg_opa_checked_pressed_focused;
	uint8_t bg_opa_checked_disabled;

	uint32_t focus_index;

	/**
	 * If true the button will use the checked colors
	 */
	bool checked;

	/**
	 * If true the button will use the disabled colors and cannot be clicked
	 */
	bool disabled;

}ls_button_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

ls_error_code_t ls_button_create(ls_screen_t * screen, ls_button_t * button);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LS_BUTTON_H*/
