/**
 * @file ls_indev.h
 *
 */

#ifndef LS_INDEV_H
#define LS_INDEV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "ls_conf.h"
#if LS_USE_INDEV == 1

#include <stdint.h>
#include <stdbool.h>
#include "ls_error_codes.h"
#include "ls_display.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
	LS_INDEV_STATE_PRESSED,
	LS_INDEV_STATE_RELEASED,
}ls_indev_state_t;

typedef enum {
	LS_INDEV_EVENT_TYPE_PRESSED,
	LS_INDEV_EVENT_TYPE_PRESSING,
	LS_INDEV_EVENT_TYPE_CLICKED,
}ls_indev_event_type_t;

typedef void (*ls_indev_event_cb_t) (void * widget, ls_indev_event_type_t type);
typedef bool (*ls_indev_click_test_cb_t) (ls_display_t * display, void * widget, uint32_t x, uint32_t y);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

ls_error_code_t ls_indev_process(ls_display_t * display, uint32_t x, uint32_t y, ls_indev_state_t state, ls_indev_state_t prev_state);

/**********************
 *      MACROS
 **********************/

#endif /*LS_USE_INDEV == 1*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LS_INDEV_H*/
