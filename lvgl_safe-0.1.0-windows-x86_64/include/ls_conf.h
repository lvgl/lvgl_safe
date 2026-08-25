/**
 * @file ls_conf.h
 *
 */

#ifndef LS_CONF_H
#define LS_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

#define LS_COLOR_FORMAT 16

#define LS_USE_INDEV    1
#define LS_USE_TEST 1
#define LS_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE 1

/*Rendering*/
#define LS_USE_RENDER_FILL  1
#define LS_USE_RENDER_IMAGE 1
#define LS_USE_RENDER_TEXT  1
#define LS_USE_RENDER_IMAGE_ROTATED 1
#define LS_USE_RENDER_IMAGE_PERSPECTIVE 1

/*Widgets*/
#define LS_USE_RECTANGLE    1

#define LS_USE_LABEL        1

#define LS_USE_IMAGE        1

#define LS_USE_9PATCH       1


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LS_CONF_H*/
