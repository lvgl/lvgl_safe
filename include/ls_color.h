/**
 * @file ls_color.h
 *
 */

#ifndef LS_COLOR_H
#define LS_COLOR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>

/*********************
 *      DEFINES
 *********************/
#define LS_BYTE_PER_PIXEL	2

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Store a color with red, green, blue channels.
 * All three channels are interpreted in 0..255 range.
 */
typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} ls_color_t;

typedef uint16_t ls_frame_buffer_color_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

static inline uint16_t ls_color_to_u16(ls_color_t color)
{
    return ((color.red & 0xF8) << 8) + ((color.green & 0xFC) << 3) + ((color.blue & 0xF8) >> 3);
}


/**********************
 *      MACROS
 **********************/

/**
 * Create a color red, green, and blue channels.
 * All channels are interpreted in 0..255 range.
 */
#define LS_COLOR_MAKE(r8, g8, b8) (ls_color_t){b8, g8, r8}

/**
 * Create a color red, green, and blue channels.
 * All channels are interpreted in 0..255 range.
 */
#define LS_COLOR_HEX(rgb888) LS_COLOR_MAKE((((rgb888) >> 16) & 0xff), (((rgb888) >> 8) & 0xff), ((rgb888) & 0xff))

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LS_COLOR_H*/
