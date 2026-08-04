/**
 * @file ls_display.h
 *
 */

#ifndef LS_DISPLAY_H
#define LS_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include "ls_error_codes.h"
#include "ls_color.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _ls_screen_t;

typedef struct
{
	/**
	 * Horizontal resolution in pixels
	 */
	uint32_t hor_res;

	/**
	 * Vertical resolution in pixels
	 */
	int32_t ver_res;

	/**
	 * Stride in bytes (number of bytes in a line)
	 */
	int32_t stride;

	/**
	 * `ls_render` renders to this buffer.
	 * Must be at least stride x ver_res x bytes_per_pixel byte sized TODO: Is this correct? Shouldn't it be only stride x ver_res?
	 */
	ls_frame_buffer_color_t * frame_buffer;

	/**
	 * Pointer to the active screen
	 */
	struct _ls_screen_t * screen_active;

}ls_display_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize a display.
 * The user needs to set the following parameters after return
 * - hor_res
 * - ver_res
 * - stride
 * - frame_buffer
 * After that screens can be added to the display
 * @param display	Pointer to an `ls_display_t` variable
 * @return			LS_ERROR_CODE_NONE: no error
 * @return			LS_ERROR_CODE_INVALID_ARGUMENT: `display` is NULL or invalid
 */
ls_error_code_t ls_display_init(ls_display_t * display);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LS_DISPLAY_H*/
