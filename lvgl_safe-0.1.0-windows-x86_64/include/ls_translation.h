/**
 * @file ls_translation.h
 *
 */

#ifndef LS_TRANSLATION_H
#define LS_TRANSLATION_H

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

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
	/**
	 * A tag to identify the translation (e.g. "dog")
	 */
	const char * tag;

	/**
	 * Internally set to the current translation.
	 */
	const char * current;

	/**
	 * List of translations for each language
	 */
	const char ** translations;
}ls_translation_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
ls_error_code_t ls_translation_init(ls_translation_t * _translations, uint32_t _translation_cnt, uint32_t _language_cnt);

ls_error_code_t ls_translation_set_language(uint32_t _current_language_index);

ls_error_code_t ls_translation_get(const char * tag, const ls_translation_t ** output);
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LS_TRANSLATION_H*/
