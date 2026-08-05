/**
 * @file translations.h
 *
 * Every string the API tour puts on screen lives in translations.c, in all three
 * languages. main.c holds the layout and the logic and no wording at all, so
 * re-wording the deck or adding a language never touches it.
 *
 */

#ifndef TRANSLATIONS_H
#define TRANSLATIONS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>

#include <ls_label.h>
#include <ls_translation.h>

/*********************
 *      DEFINES
 *********************/

/*English, German, French - the index the language button cycles through, and
 *the order of the three strings in every row of the table.*/
#define TR_LANGUAGE_CNT     3u

/*The deck. `main.c` checks its own SLIDE_CNT against this at boot.*/
#define TR_SLIDE_CNT        11u

/*Lines in the longest explanation block below. main.c sizes its per-line label
 *arrays from this, so raise it here when a block grows.*/
#define TR_LINE_MAX         13u

/*A blank row inside an explanation block. It still costs a label, because the
 *line pitch in the panel is index * PITCH, not a running cursor.*/
#define TR_LINE_SPACER      NULL

/**********************
 *      TYPEDEFS
 **********************/

/*The text of one screen: two tags for the header and one tag per line of the
 *explanation block in the left half of the panel.*/
typedef struct {
    const char * title_tag;
    const char * subtitle_tag;
    const char * const * line_tags;   /*TR_LINE_SPACER for a blank row*/
    uint32_t line_cnt;
} tr_slide_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

extern const tr_slide_t tr_slide[TR_SLIDE_CNT];

/**
 * Hand the whole table to the library and select a starting language.
 * @return 0 on success, 1 after printing what went wrong
 */
int tr_init(uint32_t language_index);

/**
 * Switch language. Every label bound with tr_bind() follows on the next render;
 * strings the application derives itself have to be rebuilt by the caller.
 * @return 0 on success, 1 after printing what went wrong
 */
int tr_set_language(uint32_t language_index);

/**
 * Point a label at a tag. The label must not also have `text` or `bind_int`
 * set - those two win over `translation`.
 * @return 0 on success, 1 after printing which tag was missing
 */
int tr_bind(ls_label_t * label, const char * tag);

/**
 * The string a tag currently resolves to, for the places a translation pointer
 * cannot be used: `bind_fmt` format strings and text assembled with snprintf.
 * The result changes with the language, so callers have to ask again after
 * every tr_set_language().
 */
const char * tr_current(const char * tag);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*TRANSLATIONS_H*/
