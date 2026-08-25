/**
 * @file ls_image_dsc.h
 * @author Peter TARJAN
 * @brief Declares image resource descriptors shared by rendering and widgets.
 * @version 1.0.0
 * @date 2026-08-07
 *
 * @copyright Copyright (c) 2026 LVGL Safe. All rights reserved.
 */

#ifndef LS_IMAGE_DSC_H
#define LS_IMAGE_DSC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "ls_color.h"

typedef enum {
    LS_IMAGE_COLOR_FORMAT_A8,
    LS_IMAGE_COLOR_FORMAT_ARGB8888,
    LS_IMAGE_COLOR_FORMAT_RGB565,
} ls_image_color_format_t;

typedef struct {
    ls_frame_buffer_color_t color_format;
    int32_t width;
    int32_t height;
    int32_t stride;
    const void * data;
} ls_image_dsc_t;

#ifdef __cplusplus
}
#endif

#endif