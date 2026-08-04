/**
 * @file ls_font.h
 *
 */

#ifndef LS_FONT_H
#define LS_FONT_H

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

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
	uint32_t w_px;
	uint32_t offset;
}ls_font_glyph_dsc_t;

typedef struct {
	int32_t glyph_id_first;    /*First Unicode letter in this font*/
	int32_t glyph_id_last;    /*Last Unicode letter in this font*/
	uint32_t h_px;             /*Font height in pixels*/
	uint32_t base_line;
	uint32_t glyph_cnt;            /*Number of glyphs in the font*/
	const uint8_t * glyph_bitmap; /*Bitmap of glyphs*/
	const ls_font_glyph_dsc_t * glyph_dsc;       /*Description of glyphs*/
} ls_font_t;

typedef uint16_t ls_frame_buffer_color_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LS_FONT_H*/
