/**
 * @file ls_screen.h
 *
 */

#ifndef LS_SCREEN_H
#define LS_SCREEN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "ls_color.h"
#include "ls_display.h"
#include "ls_render.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _ls_screen_t
{
	/**
	 * Pointer to the first widget on the screen
	 */
	void * widget_head;

	/**
	 * Pointer to the last widget on the screen
	 */
	void * widget_tail;

	/**
	 * Pointer to the next screen, forming a single linked list
	 */
	struct _ls_screen_t * screen_next;

	/**
	 * Background color of the screen
	 */
	ls_color_t bg_color;

	/**
	 * Show which item should be shown as focused
	 */
	uint32_t active_focus_index;

	/**
	 * A callback to be called to render the screen
	 */
	ls_render_cb_t render_cb;
}ls_screen_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize a screen so that widgets can be added to it after that
 * @param screen	Pointer to an `ls_screen_t` variable
 * @return			LS_ERROR_CODE_NONE: no error
 */
ls_error_code_t ls_screen_init(ls_screen_t * screen);


/**
 * USed internally to add a widget to a screen
 * @param screen	Pointer to a screen
 * @param widget	Pointer to a widget to add
 * @return			LS_ERROR_CODE_NONE: no error
 * 					LS_ERROR_CODE_MEMORY_CORRUPTION: inconsistent data detected, the widget is not added
 * 					LS_ERROR_CODE_INVALID_ARGUMENT: at least on of the parameters are NULL or invalid
 */
ls_error_code_t ls_screen_add_widget(ls_screen_t *screen, void * widget);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LS_SCREEN_H*/
