/**
 * @file ls_widget.h
 *
 */

#ifndef LS_WIDGET_H
#define LS_WIDGET_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stdbool.h>
#include "ls_error_codes.h"
#include "ls_render.h"
#include "ls_input.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Store generic widget data. Must be the first element
 * in every higher order widget structures.
 */
typedef struct {

	/**
	 * Pointer to a string containing the widget type as a string
	 * For example "ls_rectangle"
	 */
	const char * type;

	/**
	 * Pointer to the next widget on the screen forming
	 * a single linked list
	 */
	void * widget_next;

	/**
	 * A callback to be called to render the widget on its screen
	 */
	ls_render_cb_t render_cb;

#if LS_USE_INDEV
	/**
	 * Make the widget react on input event by calling this callback
	 */
	ls_indev_event_cb_t indev_event_cb;

	/**
	 * Check if a point of on the widget
	 */
	ls_indev_click_test_cb_t indev_click_test_cb;
#endif

	/**
	 * If true the widget won't be rendered and won't react on input events either
	 */
	bool hidden;

	/**
	 * If true the widget is being pressed
	 */
	bool pressed;
}ls_widget_common_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LS_WIDGET_H*/
