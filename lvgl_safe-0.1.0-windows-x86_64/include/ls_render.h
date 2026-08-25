/**
 * @file ls_render.h
 *
 */

#ifndef LS_RENDER_H
#define LS_RENDER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "ls_error_codes.h"
#include "ls_display.h"
#include "ls_color.h"

/*********************
 *      DEFINES
 *********************/

#define LS_OPA_COVER 0xff

/**********************
 *      TYPEDEFS
 **********************/
/**
 * Stored in every widget to render itself.
 * @param widget	Pointer to a widget to render.
 * @return			LS_ERROR_CODE_NONE: if rendered correctly
 * 					LS_ERROR_CODE_RENDER_FAILED: render error, the status of the buffer is unknown
 */
typedef ls_error_code_t (*ls_render_cb_t)(ls_display_t * display, void * widget);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Render the active screen of a display to the frame buffer set in the display
 * @param display	Pointer to a display
 * @return			LS_ERROR_CODE_NONE: no error
 * 					LV_ERROR_CODE_RENDER_FAILED: render error, the content of the frame buffer
 * 					                             is unknown
 */
ls_error_code_t ls_render(ls_display_t * display);


/**
 * Set the the frame buffer of a display to fully black
 * @param display		Pointer to a display to clear
 * @return				LS_ERROR_CODE_NONE: no error
 * 	 					LS_ERROR_CODE_INVALID_ARGUMENT: one of the arguments are invalid
 */
ls_error_code_t ls_render_clear_frame_buffer(ls_display_t * display);

/**
 * Used during rendering to seek to a given pixel of the currently rendered frame buffer.
 * X and Y must be smaller then the currently rendered display `hor_res` and v`er_res`.
 * @param display		  Pointer to the display that should be rendered
 * @param frame_buffer_p  Pointer to an `ls_frame_buffer_color_t_t *` where the target address will be written
 * @param x				  The target X coordinate.
 * @param y				  The target Y coordinate.
 * @return				  LS_ERROR_CODE_NONE: no error
 * 	 					  LS_ERROR_CODE_INVALID_ARGUMENT: one of the arguments are invalid
 * 	 					  LS_ERROR_CODE_NOT_ALLOWED: there is no currently rendered display (`ls_render` is not running)
 */
ls_error_code_t ls_render_goto_frame_buffer_px(ls_display_t * display, ls_frame_buffer_color_t ** frame_buffer_p, int32_t x, int32_t y);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LS_RENDER_H*/
