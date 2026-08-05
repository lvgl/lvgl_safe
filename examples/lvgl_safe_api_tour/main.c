/**
 * @file main
 *
 * LVGL Safe - guided API tour (1024x600 SDL2 simulator).
 *
 * This example is the reference tour of the library itself: a deck of eleven
 * screens, one topic per screen, meant to be walked front to back by an
 * engineer evaluating LVGL Safe. The footer arrows move through the deck.
 *
 *   01  OVERVIEW                     what the library is, in four numbers
 *   02  LS_RECTANGLE                 fills, opacity, composed frames
 *   03  LS_LABEL                     three text sources, three alignments
 *   04  LS_ARC                       angles, tracks and composed gauges
 *   05  LS_BUTTON                    ten states, zero arrays
 *   06  LS_IMAGE_BUTTON              one source image per state
 *   07  LS_IMAGE                     ARGB8888, A8 tinting and rotation
 *   08  COMMON.INDEV_EVENT_CB        hook 1 - input events
 *   09  COMMON.INDEV_CLICK_TEST_CB   hook 2 - custom hit testing
 *   10  COMMON.RENDER_CB             hook 3 - your own pixel loop
 *   11  SCREEN.RENDER_CB             hook 4 - a whole-screen backdrop
 *
 * That covers every widget type and every extension point that exists in
 * v0.1.0. `ls_screen` is covered implicitly - there are eleven of them.
 *
 * Everything the library asks of an integrator is visible here and nowhere
 * hidden behind a helper:
 *   - Every widget is a caller-owned `static` struct created once at boot.
 *     There is no allocation of any kind at run time and no widget is ever
 *     destroyed - the demo hides widgets (`common.hidden`) instead.
 *   - Every fallible call returns `ls_error_code_t` and every one of them is
 *     checked at the call site.
 *   - Widgets are configured by writing struct fields directly; the library
 *     has no setters/getters.
 *   - Render order is the screen hook first, then widgets in creation order.
 *
 * No wording lives in this file. Every string the deck displays is a row of the
 * table in translations.c, reached by tag, in English, German and French; the
 * button in the footer cycles the languages and the whole deck follows on the
 * next render.
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <SDL2/SDL.h>

#include <ls_arc.h>
#include <ls_button.h>
#include <ls_color.h>
#include <ls_display.h>
#include <ls_error_codes.h>
#include <ls_font.h>
#include <ls_image.h>
#include <ls_image_button.h>
#include <ls_input.h>
#include <ls_label.h>
#include <ls_math.h>
#include <ls_rectangle.h>
#include <ls_render.h>
#include <ls_screen.h>
#include <ls_translation.h>
#include <ls_widget.h>

#include "translations.h"

/*********************
 *      DEFINES
 *********************/
#define WIN_HOR_RES 1024
#define WIN_VER_RES 600

#define UNUSED(x) (void)(x)

/*--- Palette -------------------------------------------------------------*/
#define COLOR_BG_TOP        LS_COLOR_HEX(0x243444)   /*Top of the screen gradient*/
#define COLOR_BG_BOTTOM     LS_COLOR_HEX(0x0b0f13)   /*Bottom of the screen gradient*/
#define COLOR_BG_FLAT       LS_COLOR_HEX(0x11161b)   /*Used when the gradient hook is off*/
#define COLOR_PANEL         LS_COLOR_HEX(0x171d23)
#define COLOR_PANEL_EDGE    LS_COLOR_HEX(0x2b343d)
#define COLOR_PANEL_SUNKEN  LS_COLOR_HEX(0x10151a)
#define COLOR_CONTROL       LS_COLOR_HEX(0x232c35)
#define COLOR_CONTROL_FOCUS LS_COLOR_HEX(0x33404d)
#define COLOR_CONTROL_OFF   LS_COLOR_HEX(0x1b2127)   /*A disabled button*/
#define COLOR_TEXT          LS_COLOR_HEX(0xe6ebef)
#define COLOR_TEXT_MUTED    LS_COLOR_HEX(0x93a1ad)
#define COLOR_TEXT_DIM      LS_COLOR_HEX(0x5d6a75)
#define COLOR_TEXT_ON_OK    LS_COLOR_HEX(0x0b3d22)
#define COLOR_ACCENT        LS_COLOR_HEX(0x3d8bfd)
#define COLOR_OK            LS_COLOR_HEX(0x3ecf7e)
#define COLOR_OK_PRESSED    LS_COLOR_HEX(0x2fa864)
#define COLOR_WARN          LS_COLOR_HEX(0xf0a92c)
#define COLOR_ALERT         LS_COLOR_HEX(0xe5484d)
#define COLOR_TEAL          LS_COLOR_HEX(0x19b3c4)
#define COLOR_TRACK         LS_COLOR_HEX(0x272f37)
#define COLOR_PAPER         LS_COLOR_HEX(0xd9e0e6)   /*Light strip the opacity ladder blends onto*/
#define COLOR_TRACE_FILL    LS_COLOR_HEX(0x14454d)   /*Under the waveform trace on slide 10*/
#define COLOR_TRACE_GRID    LS_COLOR_HEX(0xffffff)   /*Its dotted zero line*/

/*--- Slide template ------------------------------------------------------*/
#define HEADER_RULE_Y   76u
#define PANEL_X         32u
#define PANEL_Y         92u
#define PANEL_W         960u
#define PANEL_H         396u
#define TEXT_X          60u          /*Left column: the explanation*/
#define TEXT_CAPTION_Y  116u
#define BULLET_TOP_Y    152u
#define BULLET_PITCH    24u
#define BULLET_TEXT_X   82u          /*On a numbered line the number sits at TEXT_X and
                                      *the text starts here*/
#define DIVIDER_X       400u
#define DEMO_X          440u         /*Right column: the live demo, 440..968*/
#define FOOTER_RULE_Y   512u

/*One label per explanation line, so the arrays below are sized by the longest
 *block in the text table. The text decides this, not the layout.*/
#define BULLET_MAX      TR_LINE_MAX

/*--- Arc gauge degree range: 270 degrees of sweep, opening at the bottom ----*/
#define GAUGE_START_DEG 135u
#define GAUGE_SWEEP_DEG 270u

/*--- Limits --------------------------------------------------------------*/
#define OPA_LADDER_CNT      5u
#define Z_ORDER_CNT         3u
#define ALIGN_DEMO_CNT      3u
#define STATE_BUTTON_CNT    4u
#define IMAGE_BUTTON_CNT    3u
#define EVENT_COUNTER_CNT   3u
#define PILLAR_CNT          4u
#define ARC_LEGEND_CNT      3u
#define TINT_SAMPLE_CNT     3u
#define RENDER_STEP_CNT     4u

#define KNOB_STEP           5
#define KNOB_TOUCH_MARGIN   14   /*Grow the ring's hit area by this many px*/

#define WAVE_W              490u
#define WAVE_H              170u
#define WAVE_SAMPLE_CNT     WAVE_W   /*One sample per pixel column of the trace*/

/*The two columns of the render-order panel on slide 11: the step number, then
 *the step text beside it.*/
#define RENDER_STEP_INDEX_X 470u
#define RENDER_STEP_TEXT_X  492u

/*The two hint rows on slide 09: the text starts at KNOB_HINT_X, and the step
 *value is right-aligned on KNOB_HINT_STEP_X so the numbers form a column.*/
#define KNOB_HINT_X         770u
#define KNOB_HINT_STEP_X    948u

/**********************
 *      TYPEDEFS
 **********************/

/*The deck. Order here is the order the arrows walk through, and every array in
 *this file is indexed by it. These are C indices, so they run 0..10 while the
 *screens themselves are called 01..11 - see the note on numbering at the top.*/
typedef enum {
    SLIDE_OVERVIEW = 0,
    SLIDE_RECTANGLE,
    SLIDE_LABEL,
    SLIDE_ARC,
    SLIDE_BUTTON,
    SLIDE_IMAGE_BUTTON,
    SLIDE_IMAGE,
    SLIDE_HOOK_EVENTS,
    SLIDE_HOOK_CLICK_TEST,
    SLIDE_HOOK_RENDER,
    SLIDE_HOOK_SCREEN_RENDER,
    SLIDE_CNT
} slide_id_t;

/*The per-screen settings that are not text: what the screen's wording is comes
 *from `tr_slide[]` in translations.c, indexed by the same slide id.*/
typedef struct {
    /*How many focus slots this screen has. The last one always belongs to the
     *footer's language button; content buttons take the ones before it.*/
    uint32_t focus_cnt;
    ls_color_t accent;
    /*Set only on a screen whose explanation block is a numbered list: one entry
     *per line, holding the number to print, or NULL on a line that carries no
     *number. NULL for a screen that numbers nothing. Numbers are not text, so
     *they live here and not in `tr_slide[]`.*/
    const char * const * line_index;
} slide_desc_t;

/*Every screen owns its own chrome: a widget is singly-linked into exactly one
 *screen through its own `common.widget_next`, so the same struct can never
 *appear on two screens.*/
typedef struct {
    ls_image_t logo;
    ls_label_t product;
    ls_label_t tagline;
    ls_label_t title;
    ls_label_t subtitle;
    ls_rectangle_t header_rule;

    ls_rectangle_t panel_edge;
    ls_rectangle_t panel_face;
    ls_label_t bullets_caption;
    ls_rectangle_t bullets_accent;
    ls_label_t bullet_indices[BULLET_MAX];   /*One per line, but only a numbered line creates its own*/
    ls_label_t bullets[BULLET_MAX];
    ls_rectangle_t column_divider;

    ls_rectangle_t footer_rule;
    ls_image_button_t nav_prev;
    ls_image_button_t nav_next;
    ls_label_t step;
    ls_label_t hint;
    ls_button_t language_button;
    ls_label_t language_label;
    ls_label_t language_caption;
    ls_label_t note;
} chrome_t;

/*A user-defined widget. `ls_rectangle_t` is the first member, so a pointer to
 *this struct is also a valid `ls_rectangle_t *` and `ls_widget_common_t *` -
 *the exact layout rule the library's own widgets follow. All the extra state
 *lives after it, and `render_cb` is swapped for our own function below.*/
typedef struct {
    ls_rectangle_t base;
    ls_render_cb_t base_render_cb;  /*The stock rectangle render hook, kept so we can chain to it*/
    const int8_t * samples;         /*-100..100, one per pixel column*/
    uint32_t sample_cnt;
    uint32_t head;                  /*Index of the oldest sample*/
    ls_color_t trace_color;
    ls_color_t fill_color;
    ls_color_t grid_color;
} waveform_t;

/*The descriptors below carry translation tags, never text. `value` on a pillar
 *is a literal number, which is not text and needs no translation.*/
typedef struct {
    uint32_t x;
    uint32_t y;
    const char * value;
    const char * caption_tag;
} pillar_desc_t;

typedef struct {
    uint32_t x;
    uint32_t y;
    const char * text_tag;
    const char * caption_tag;
} state_button_desc_t;

typedef struct {
    uint32_t y;
    const char * caption_tag;
    const char * sub_caption_tag;
} image_button_desc_t;

typedef struct {
    uint32_t y;
    const char * name_tag;
} event_counter_desc_t;

typedef struct {
    uint32_t y;
    const char * name_tag;
    ls_color_t color;
} arc_legend_desc_t;

/**********************
 *  STATIC VARIABLES
 **********************/
static SDL_Renderer * renderer;
static SDL_Texture * texture;

static ls_display_t display_1;
static ls_screen_t screens[SLIDE_CNT];
static chrome_t chrome[SLIDE_CNT];
static char step_text[SLIDE_CNT][8];
static uint32_t active_slide;

/*Saved before we install our own screen hook, so the custom hook can chain
 *back to the library's default flat fill.*/
static ls_render_cb_t stock_screen_render_cb;
static bool background_gradient_on = true;

/* ------------------------------- ASSETS ---------------------------------- */
extern const ls_image_dsc_t lvgl_logo_dark;
extern const ls_image_dsc_t fault_triangle_a8;
extern const ls_image_dsc_t switch_on;
extern const ls_image_dsc_t switch_off;
extern const ls_image_dsc_t nav_prev_normal;
extern const ls_image_dsc_t nav_prev_pressed;
extern const ls_image_dsc_t nav_next_normal;
extern const ls_image_dsc_t nav_next_pressed;

/*The spinning logo on slide 07.*/
extern const ls_image_dsc_t img_benchmark_lvgl_logo_argb;

extern ls_font_t montserrat_12;
extern ls_font_t montserrat_14;
extern ls_font_t montserrat_18;
extern ls_font_t montserrat_28;

/* ---------------------------- TRANSLATIONS ------------------------------- */
/*Every string on screen comes from the table in translations.c, reached by tag.
 *This file keeps only the index the footer button cycles through.*/
static uint32_t language_index;

/* ------------------------------- SLIDE DECK ------------------------------ */
/*Slide 03 lists the three text sources as 1 / 2 / 3. One entry per line of
 *`lines_label[]` in translations.c, spacer rows included, so the numbers land on
 *the right lines; the remaining lines are left unnumbered.*/
static const char * const label_line_index[BULLET_MAX] = {
    NULL, NULL,         /*"Text comes from one of" / "three sources, in order:"*/
    NULL,               /*spacer*/
    "1", "2", "3",      /*text / bind_int / translation*/
};

static const slide_desc_t slide_desc[SLIDE_CNT] = {
    { 1u, COLOR_ACCENT, NULL },              /*01  OVERVIEW*/
    { 1u, COLOR_ACCENT, NULL },              /*02  LS_RECTANGLE*/
    { 1u, COLOR_OK,     label_line_index },  /*03  LS_LABEL*/
    { 1u, COLOR_TEAL,   NULL },              /*04  LS_ARC*/
    { 4u, COLOR_WARN,   NULL },              /*05  LS_BUTTON*/
    { 1u, COLOR_ALERT,  NULL },              /*06  LS_IMAGE_BUTTON*/
    { 1u, COLOR_TEAL,   NULL },              /*07  LS_IMAGE*/
    { 2u, COLOR_ACCENT, NULL },              /*08  COMMON.INDEV_EVENT_CB*/
    { 1u, COLOR_OK,     NULL },              /*09  COMMON.INDEV_CLICK_TEST_CB*/
    { 1u, COLOR_TEAL,   NULL },              /*10  COMMON.RENDER_CB*/
    { 1u, COLOR_WARN,   NULL },              /*11  SCREEN.RENDER_CB*/
};

/* ------------------------- SLIDE CONTENT WIDGETS ------------------------- */

/*01 - OVERVIEW*/
static const pillar_desc_t pillar_desc[PILLAR_CNT] = {
    { 440u, 140u, "0", "ov_pillar_alloc" },
    { 706u, 140u, "6", "ov_pillar_widgets" },
    { 440u, 306u, "4", "ov_pillar_hooks" },
    { 706u, 306u, "1", "ov_pillar_buffer" },
};
static ls_rectangle_t pillar_edges[PILLAR_CNT];
static ls_rectangle_t pillar_faces[PILLAR_CNT];
static ls_label_t pillar_values[PILLAR_CNT];
static ls_label_t pillar_captions[PILLAR_CNT];

/*02 - LS_RECTANGLE*/
static ls_label_t rect_ladder_caption;
static ls_rectangle_t rect_ladder_paper;
static ls_rectangle_t rect_ladder[OPA_LADDER_CNT];
static ls_label_t rect_frame_caption;
static ls_rectangle_t rect_frame_edge;
static ls_rectangle_t rect_frame_face;
static ls_label_t rect_frame_text;
static ls_label_t rect_order_caption;
static ls_rectangle_t rect_order_squares[Z_ORDER_CNT];
static ls_label_t rect_order_note;

/*03 - LS_LABEL*/
static const char * const align_demo_tags[ALIGN_DEMO_CNT] = {
    "lbl_align_left", "lbl_align_center", "lbl_align_right"
};
static ls_label_t label_align_caption;
static ls_rectangle_t label_align_guide;
static ls_label_t label_aligns[ALIGN_DEMO_CNT];
static ls_label_t label_style_caption;
static ls_label_t label_style_demo;
static ls_label_t label_bind_caption;
static ls_label_t label_bind_demo;
static ls_label_t label_translation_caption;
static ls_label_t label_translation_demo;

/*04 - LS_ARC*/
static const arc_legend_desc_t arc_legend_desc[ARC_LEGEND_CNT] = {
    { 200u, "arc_legend_track", COLOR_TRACK },
    { 244u, "arc_legend_value", COLOR_TEAL  },
    { 288u, "arc_legend_limit", COLOR_ALERT },
};
static ls_arc_t arc_track;
static ls_arc_t arc_fill;
static ls_arc_t arc_limit_marker;
static ls_label_t arc_value;
static ls_label_t arc_unit;
static ls_rectangle_t arc_legend_swatches[ARC_LEGEND_CNT];
static ls_label_t arc_legend_names[ARC_LEGEND_CNT];
static ls_label_t arc_caption;

/*05 - LS_BUTTON*/
static const state_button_desc_t state_button_desc[STATE_BUTTON_CNT] = {
    { 460u, 150u, "btn_press",  "btn_cap_press"  },
    { 706u, 150u, "btn_toggle", "btn_cap_toggle" },
    { 460u, 286u, "btn_locked", "btn_cap_locked" },
    { 706u, 286u, "btn_focus",  "btn_cap_focus"  },
};
static ls_button_t state_buttons[STATE_BUTTON_CNT];
static ls_label_t state_button_texts[STATE_BUTTON_CNT];
static ls_label_t state_button_captions[STATE_BUTTON_CNT];
static ls_label_t state_button_note;

/*06 - LS_IMAGE_BUTTON*/
static const image_button_desc_t image_button_desc[IMAGE_BUTTON_CNT] = {
    { 150u, "ibtn_cap_checked",  "ibtn_sub_checked"  },
    { 236u, "ibtn_cap_pressed",  "ibtn_sub_pressed"  },
    { 336u, "ibtn_cap_disabled", "ibtn_sub_disabled" },
};
static ls_image_button_t demo_switch;
static ls_image_button_t demo_press_button;
static ls_image_button_t demo_disabled_button;
static ls_label_t image_button_captions[IMAGE_BUTTON_CNT];
static ls_label_t image_button_sub_captions[IMAGE_BUTTON_CNT];
static ls_label_t image_button_note;

/*07 - LS_IMAGE*/
static ls_image_t spinning_logo;
static ls_label_t spinning_logo_caption;
static ls_image_t tinted_glyph;
static ls_label_t tinted_glyph_caption;
static ls_image_t tint_samples[TINT_SAMPLE_CNT];
static ls_label_t tint_samples_caption;

/*08 - COMMON.INDEV_EVENT_CB*/
static const event_counter_desc_t event_counter_desc[EVENT_COUNTER_CNT] = {
    { 164u, "ev_counter_pressed"  },
    { 206u, "ev_counter_pressing" },
    { 248u, "ev_counter_clicked"  },
};
static ls_button_t events_button;
static ls_label_t events_button_text;
static ls_label_t event_counter_names[EVENT_COUNTER_CNT];
static ls_label_t event_counter_values[EVENT_COUNTER_CNT];
static ls_label_t events_last;
static ls_label_t events_note_bind;
static ls_label_t events_note_pressing;

/*09 - COMMON.INDEV_CLICK_TEST_CB*/
static ls_arc_t knob_track;
static ls_arc_t knob_fill;
static ls_label_t knob_value;
static ls_label_t knob_unit;
static ls_label_t knob_hint_left;
static ls_label_t knob_step_left;
static ls_label_t knob_hint_right;
static ls_label_t knob_step_right;
static ls_label_t knob_note_area;
static ls_label_t knob_note_point;

/*10 - COMMON.RENDER_CB*/
static waveform_t waveform;
static int8_t wave_samples[WAVE_SAMPLE_CNT];
static uint32_t wave_head;
/*Two angle accumulators, each wrapped back into 0..359 on every step.*/
static int32_t wave_angle_slow;
static int32_t wave_angle_fast;
static ls_image_button_t waveform_switch;
static ls_label_t waveform_switch_caption;
static ls_label_t waveform_switch_sub_caption;
static ls_label_t waveform_note;

/*11 - SCREEN.RENDER_CB*/
/*The step number is a label of its own, so the text beside it always starts at
 *the same x, just for alignment of the text which preceeds them.*/
static const char * const render_step_index[RENDER_STEP_CNT] = {
    "1", "2", "3", "4"
};
static const char * const render_step_tags[RENDER_STEP_CNT] = {
    "scr_step_screen", "scr_step_widgets", "scr_step_hidden", "scr_step_error"
};
static ls_image_button_t gradient_switch;
static ls_label_t gradient_switch_caption;
static ls_label_t gradient_switch_sub_caption;
static ls_rectangle_t render_order_panel;
static ls_label_t render_order_caption;
static ls_label_t render_order_indices[RENDER_STEP_CNT];
static ls_label_t render_order_steps[RENDER_STEP_CNT];
static ls_label_t render_order_note;

/* --------------------------------- MODEL --------------------------------- */
/*Everything the UI shows lives in plain variables. Labels are bound to the
 *int32_t ones through `bind_int`, so updating the model updates the screen
 *without touching a widget.*/
static int32_t frame_count;                      /*Slide 03's bind_int demo*/
static int32_t gauge_value = 60;                 /*0..100, animated on slide 04*/
static int32_t knob_value_pct = 45;              /*0..100, tapped on slide 09*/
static int32_t event_counts[EVENT_COUNTER_CNT];  /*PRESSED / PRESSING / CLICKED, slide 08*/

/*The "last event" line is assembled from two translations, so it cannot be a
 *plain `translation` binding: the event that was seen last is remembered here
 *instead, and the string is rebuilt whenever either input changes - a new event
 *or a new language.*/
static char events_last_text[48];
static int32_t events_last_index = -1;           /*-1 until the first event*/

/*Slide 07's rotating, fading logo.*/
static int32_t spin_deg;
static uint8_t spin_opa = 255;
static bool spin_opa_falling = true;

/*Which colour slide 07's A8 warning triangle is tinted: alert when set, warn when
 *clear. Toggled by the momentary button one screen earlier, on slide 06.*/
static bool glyph_alert;

/*Written by the knob's hit-test hook, read by its event callback: -1 for a tap
 *on the left half of the ring, +1 on the right. The event callback signature
 *carries no coordinates, so the hit test - which does get them - is the only
 *place the touch position can be captured.*/
static int32_t knob_touch_direction;

/*The input device, as the application sees it. `ls_indev_process` derives the
 *event type from the current and previous state, so both are kept here.*/
static uint32_t indev_x;
static uint32_t indev_y;
static ls_indev_state_t indev_state = LS_INDEV_STATE_RELEASED;
static ls_indev_state_t indev_prev_state = LS_INDEV_STATE_RELEASED;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void sdl_init(void);
static void sdl_update(const void * fb);

static int init_slide_chrome(uint32_t slide);
static int init_image_button(ls_screen_t * screen, ls_image_button_t * button, uint32_t x, uint32_t y,
                             const ls_image_dsc_t * normal, const ls_image_dsc_t * pressed,
                             const ls_image_dsc_t * checked, const ls_image_dsc_t * checked_pressed);
static int init_caption(ls_screen_t * screen, ls_label_t * label, uint32_t x, uint32_t y, const char * tag);
static void apply_language(void);

static int init_slide_overview(ls_screen_t * screen);
static int init_slide_rectangle(ls_screen_t * screen);
static int init_slide_label(ls_screen_t * screen);
static int init_slide_arc(ls_screen_t * screen);
static int init_slide_button(ls_screen_t * screen);
static int init_slide_image_button(ls_screen_t * screen);
static int init_slide_image(ls_screen_t * screen);
static int init_slide_hook_events(ls_screen_t * screen);
static int init_slide_hook_click_test(ls_screen_t * screen);
static int init_slide_hook_render(ls_screen_t * screen);
static int init_slide_hook_screen_render(ls_screen_t * screen);

static int init_waveform(ls_screen_t * screen, waveform_t * wave, uint32_t x, uint32_t y,
                         uint32_t width, uint32_t height);
static int8_t wave_next_sample(void);
static ls_error_code_t waveform_render_cb(ls_display_t * display, void * widget);
static ls_error_code_t gradient_screen_render_cb(ls_display_t * display, void * widget);
static bool knob_click_test_cb(ls_display_t * display, void * widget, uint32_t x, uint32_t y);

static void nav_prev_cb(void * widget, ls_indev_event_type_t type);
static void nav_next_cb(void * widget, ls_indev_event_type_t type);
static void cycle_language_cb(void * widget, ls_indev_event_type_t type);
static void state_button_toggle_cb(void * widget, ls_indev_event_type_t type);
static void demo_switch_cb(void * widget, ls_indev_event_type_t type);
static void demo_press_button_cb(void * widget, ls_indev_event_type_t type);
static void events_button_cb(void * widget, ls_indev_event_type_t type);
static void knob_cb(void * widget, ls_indev_event_type_t type);
static void waveform_switch_cb(void * widget, ls_indev_event_type_t type);
static void gradient_switch_cb(void * widget, ls_indev_event_type_t type);

static void show_slide(uint32_t slide);
static void animate(void);
static void handle_focus_key(SDL_Keycode key);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

int main(int argc, char ** argv)
{
    UNUSED(argc);
    UNUSED(argv);

    sdl_init();

    /*The one and only frame buffer.*/
    static ls_frame_buffer_color_t fb[WIN_HOR_RES * WIN_VER_RES];

    ls_error_code_t err;
    uint32_t i;

    err = ls_display_init(&display_1);
    if(err != LS_ERROR_CODE_OK) { printf("display init failed: %d\n", err); return 1; }
    display_1.hor_res = WIN_HOR_RES;
    display_1.ver_res = WIN_VER_RES;
    display_1.stride = WIN_HOR_RES * LS_BYTE_PER_PIXEL;
    display_1.frame_buffer = fb;
    display_1.screen_active = &screens[SLIDE_OVERVIEW];

    for(i = 0; i < (uint32_t) SLIDE_CNT; i++) {
        err = ls_screen_init(&screens[i]);
        if(err != LS_ERROR_CODE_OK) { printf("screen %u init failed: %d\n", i, err); return 1; }
        screens[i].bg_color = COLOR_BG_FLAT;

        /*HOOK 4 - the screen's own render callback. `ls_screen_init` installs
         *a default that flat-fills `bg_color`. We keep a pointer to it and put a
         *wrapper in its place that can either draw a vertical gradient or
         *delegate straight back to the original. Every screen gets the same
         *default.*/
        if(i == 0u) stock_screen_render_cb = screens[i].render_cb;
        screens[i].render_cb = gradient_screen_render_cb;
    }

    /*Sanity check.*/
    if((uint32_t) SLIDE_CNT != TR_SLIDE_CNT) {
        printf("slide count %u does not match the text table (%u)\n",
               (unsigned) SLIDE_CNT, (unsigned) TR_SLIDE_CNT);
        return 1;
    }

    if(tr_init(language_index)) { printf("translation init failed!\n"); return 1; }

    for(i = 0; i < (uint32_t) SLIDE_CNT; i++) {
        if(init_slide_chrome(i)) { printf("slide %u chrome creation failed!\n", i); return 1; }
    }

    /*Content is created after the chrome so it draws on top of the it.*/
    if(init_slide_overview(&screens[SLIDE_OVERVIEW])) return 1;
    if(init_slide_rectangle(&screens[SLIDE_RECTANGLE])) return 1;
    if(init_slide_label(&screens[SLIDE_LABEL])) return 1;
    if(init_slide_arc(&screens[SLIDE_ARC])) return 1;
    if(init_slide_button(&screens[SLIDE_BUTTON])) return 1;
    if(init_slide_image_button(&screens[SLIDE_IMAGE_BUTTON])) return 1;
    if(init_slide_image(&screens[SLIDE_IMAGE])) return 1;
    if(init_slide_hook_events(&screens[SLIDE_HOOK_EVENTS])) return 1;
    if(init_slide_hook_click_test(&screens[SLIDE_HOOK_CLICK_TEST])) return 1;
    if(init_slide_hook_render(&screens[SLIDE_HOOK_RENDER])) return 1;
    if(init_slide_hook_screen_render(&screens[SLIDE_HOOK_SCREEN_RENDER])) return 1;

    /*Two strings are built by the application rather than bound to a tag, so we fill
     *them in for the starting language.*/
    apply_language();

    /* ------------------------------ MAIN LOOP ------------------------------ */

    uint32_t next_animation_ms = 0;

    while(1) {
        /*Feed the input device. The library resolves which widget is under the
         *mouse pointer using each widget's `indev_click_test_cb`, then calls that
         *widget's `indev_event_cb` with the derived event type.*/
        err = ls_indev_process(&display_1, indev_x, indev_y, indev_state, indev_prev_state);
        if(err != LS_ERROR_CODE_OK) { printf("indev process failed: %d\n", err); return 1; }
        indev_prev_state = indev_state;

        uint32_t now_ms = SDL_GetTicks();
        if(now_ms >= next_animation_ms) {
            next_animation_ms = now_ms + 25u;
            animate();
        }

        /*One full redraw of the active screen into `fb`, then hand the buffer to
         *the host. There is no partial-refresh.*/
        err = ls_render(&display_1);
        if(err != LS_ERROR_CODE_OK) { printf("render failed: %d\n", err); return 1; }
        sdl_update(fb);

        /*Idal time*/
        usleep(1000 * 5);

        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    return 0;
                case SDL_KEYDOWN:
                    handle_focus_key(event.key.keysym.sym);
                    break;
                case SDL_MOUSEBUTTONUP:
                    if(event.button.button == SDL_BUTTON_LEFT) {
                        indev_state = LS_INDEV_STATE_RELEASED;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if(event.button.button == SDL_BUTTON_LEFT) {
                        indev_state = LS_INDEV_STATE_PRESSED;
                        indev_x = (uint32_t) event.motion.x;
                        indev_y = (uint32_t) event.motion.y;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    indev_x = (uint32_t) event.motion.x;
                    indev_y = (uint32_t) event.motion.y;
                    break;
                default:
                    break;
            }
        }
    }

    return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* =============================== HELPERS ================================= */

/*A small dim caption in the demo column.*/
static int init_caption(ls_screen_t * screen, ls_label_t * label, uint32_t x, uint32_t y, const char * tag)
{
    ls_error_code_t err = ls_label_create(screen, label);
    if(err != LS_ERROR_CODE_OK) { printf("caption '%s' create failed: %d\n", tag, err); return 1; }

    label->x = x;
    label->y = y;
    label->font = &montserrat_12;
    label->color = COLOR_TEXT_DIM;

    return tr_bind(label, tag);
}

/*An image button has one source slot per state and renders whichever one matches
 *the state it is in, so a NULL in the slot it needs is reported as
 *LS_ERROR_CODE_INVALID_CONFIGURATION.*/
static int init_image_button(ls_screen_t * screen, ls_image_button_t * button, uint32_t x, uint32_t y,
                             const ls_image_dsc_t * normal, const ls_image_dsc_t * pressed,
                             const ls_image_dsc_t * checked, const ls_image_dsc_t * checked_pressed)
{
    ls_error_code_t err = ls_image_button_create(screen, button);
    if(err != LS_ERROR_CODE_OK) { printf("image button create failed: %d\n", err); return 1; }

    /*A button that never latches passes NULL for the checked artwork.*/
    if(checked == NULL) checked = normal;
    if(checked_pressed == NULL) checked_pressed = pressed;

    button->x = x;
    button->y = y;

    button->src_normal = normal;
    button->src_pressed = pressed;
    button->src_focused = normal;
    button->src_pressed_focused = pressed;
    button->src_disabled = normal;

    button->src_checked = checked;
    button->src_checked_pressed = checked_pressed;
    button->src_checked_focused = checked;
    button->src_checked_pressed_focused = checked_pressed;
    button->src_checked_disabled = checked;

    return 0;
}

/*Almost every label follows the language on its own, because it holds a pointer
 *to a table row and the row's `current` is what moved. The two exceptions are
 *the strings this file builds itself: a `bind_fmt` format and the snprintf'd
 *"last event" line. Both are re-read here, once per language change.*/
static void apply_language(void)
{
    label_bind_demo.bind_fmt = tr_current("lbl_bind_fmt");

    (void) snprintf(events_last_text, sizeof(events_last_text), tr_current("ev_last_fmt"),
                    (events_last_index < 0)
                    ? tr_current("ev_last_none")
                    : tr_current(event_counter_desc[events_last_index].name_tag));
}

/* ============================ SLIDE TEMPLATE ============================= */

/*Header, explanation panel and footer - identical on every screen*/
static int init_slide_chrome(uint32_t slide)
{
    ls_screen_t * screen = &screens[slide];
    chrome_t * c = &chrome[slide];
    const slide_desc_t * desc = &slide_desc[slide];
    const tr_slide_t * text = &tr_slide[slide];
    ls_error_code_t err;
    uint32_t i;

    /* ------------------------------ HEADER ------------------------------- */

    /*The LVGL dark mark, top left.*/
    err = ls_image_create(screen, &c->logo);
    if(err != LS_ERROR_CODE_OK) { printf("logo create failed: %d\n", err); return 1; }
    c->logo.x = 18;
    c->logo.y = 14;
    c->logo.src = &lvgl_logo_dark;

    err = ls_label_create(screen, &c->product);
    if(err != LS_ERROR_CODE_OK) { printf("product create failed: %d\n", err); return 1; }
    c->product.x = 72;
    c->product.y = 12;
    c->product.font = &montserrat_18;
    c->product.color = COLOR_TEXT;
    c->product.letter_space = 2;
    if(tr_bind(&c->product, "product")) return 1;

    err = ls_label_create(screen, &c->tagline);
    if(err != LS_ERROR_CODE_OK) { printf("tagline create failed: %d\n", err); return 1; }
    c->tagline.x = 73;
    c->tagline.y = 40;
    c->tagline.font = &montserrat_12;
    c->tagline.color = COLOR_TEXT_DIM;
    if(tr_bind(&c->tagline, "tagline")) return 1;

    /*The title and subtitle are the only chrome text that changes per screen.
     *Both are right-aligned with no `width` set: alignment resolves against
     *`width`, so a width of 0 makes the offset -line_width and the line ends
     *exactly at `x`. The centred labels further down use the same technique with
     *half the line width, which is why none of them needs a box.*/
    err = ls_label_create(screen, &c->title);
    if(err != LS_ERROR_CODE_OK) { printf("title create failed: %d\n", err); return 1; }
    c->title.x = 1006;
    c->title.y = 12;
    c->title.align = LS_LABEL_ALIGN_RIGHT;
    c->title.font = &montserrat_18;
    c->title.color = desc->accent;
    if(tr_bind(&c->title, text->title_tag)) return 1;

    err = ls_label_create(screen, &c->subtitle);
    if(err != LS_ERROR_CODE_OK) { printf("subtitle create failed: %d\n", err); return 1; }
    c->subtitle.x = 1006;
    c->subtitle.y = 42;
    c->subtitle.align = LS_LABEL_ALIGN_RIGHT;
    c->subtitle.font = &montserrat_12;
    c->subtitle.color = COLOR_TEXT_MUTED;
    if(tr_bind(&c->subtitle, text->subtitle_tag)) return 1;

    err = ls_rectangle_create(screen, &c->header_rule);
    if(err != LS_ERROR_CODE_OK) { printf("header_rule create failed: %d\n", err); return 1; }
    c->header_rule.x = 0;
    c->header_rule.y = HEADER_RULE_Y;
    c->header_rule.width = WIN_HOR_RES;
    c->header_rule.height = 1;
    c->header_rule.bg_color = COLOR_PANEL_EDGE;

    /* ------------------------------- PANEL ------------------------------- */
    err = ls_rectangle_create(screen, &c->panel_edge);
    if(err != LS_ERROR_CODE_OK) { printf("panel_edge create failed: %d\n", err); return 1; }
    c->panel_edge.x = PANEL_X;
    c->panel_edge.y = PANEL_Y;
    c->panel_edge.width = PANEL_W;
    c->panel_edge.height = PANEL_H;
    c->panel_edge.bg_color = COLOR_PANEL_EDGE;

    err = ls_rectangle_create(screen, &c->panel_face);
    if(err != LS_ERROR_CODE_OK) { printf("panel_face create failed: %d\n", err); return 1; }
    c->panel_face.x = PANEL_X + 1u;
    c->panel_face.y = PANEL_Y + 1u;
    c->panel_face.width = PANEL_W - 2u;
    c->panel_face.height = PANEL_H - 2u;
    c->panel_face.bg_color = COLOR_PANEL;

    err = ls_label_create(screen, &c->bullets_caption);
    if(err != LS_ERROR_CODE_OK) { printf("bullets_caption create failed: %d\n", err); return 1; }
    c->bullets_caption.x = TEXT_X;
    c->bullets_caption.y = TEXT_CAPTION_Y;
    c->bullets_caption.font = &montserrat_12;
    c->bullets_caption.color = COLOR_TEXT_MUTED;
    c->bullets_caption.letter_space = 2;
    if(tr_bind(&c->bullets_caption, "panel_caption")) return 1;

    err = ls_rectangle_create(screen, &c->bullets_accent);
    if(err != LS_ERROR_CODE_OK) { printf("bullets_accent create failed: %d\n", err); return 1; }
    c->bullets_accent.x = TEXT_X;
    c->bullets_accent.y = TEXT_CAPTION_Y + 22u;
    c->bullets_accent.width = 26;
    c->bullets_accent.height = 1;
    c->bullets_accent.bg_color = desc->accent;

    if(text->line_cnt > BULLET_MAX) { printf("slide %u has too many bullets\n", slide); return 1; }
    for(i = 0; i < text->line_cnt; i++) {
        /*A numbered line is two labels: the number in its own column, the text
         *starting at a fixed x that no digit can push around.*/
        const char * index_text = (desc->line_index != NULL) ? desc->line_index[i] : NULL;

        if(index_text != NULL) {
            err = ls_label_create(screen, &c->bullet_indices[i]);
            if(err != LS_ERROR_CODE_OK) { printf("bullet index %u create failed: %d\n", i, err); return 1; }
            c->bullet_indices[i].x = TEXT_X;
            c->bullet_indices[i].y = BULLET_TOP_Y + (i * BULLET_PITCH);
            c->bullet_indices[i].font = &montserrat_14;
            c->bullet_indices[i].color = COLOR_TEXT_MUTED;
            c->bullet_indices[i].text = index_text;
        }

        err = ls_label_create(screen, &c->bullets[i]);
        if(err != LS_ERROR_CODE_OK) { printf("bullet %u create failed: %d\n", i, err); return 1; }
        c->bullets[i].x = (index_text != NULL) ? BULLET_TEXT_X : TEXT_X;
        c->bullets[i].y = BULLET_TOP_Y + (i * BULLET_PITCH);
        c->bullets[i].font = &montserrat_14;
        c->bullets[i].color = COLOR_TEXT_MUTED;

        /*A spacer row holds the pitch and has nothing to translate.*/
        if(text->line_tags[i] == TR_LINE_SPACER) c->bullets[i].text = "";
        else if(tr_bind(&c->bullets[i], text->line_tags[i])) return 1;
    }

    err = ls_rectangle_create(screen, &c->column_divider);
    if(err != LS_ERROR_CODE_OK) { printf("column_divider create failed: %d\n", err); return 1; }
    c->column_divider.x = DIVIDER_X;
    c->column_divider.y = TEXT_CAPTION_Y;
    c->column_divider.width = 1;
    c->column_divider.height = 356;
    c->column_divider.bg_color = COLOR_PANEL_EDGE;

    /* ------------------------------ FOOTER ------------------------------- */

    err = ls_rectangle_create(screen, &c->footer_rule);
    if(err != LS_ERROR_CODE_OK) { printf("footer_rule create failed: %d\n", err); return 1; }
    c->footer_rule.x = 0;
    c->footer_rule.y = FOOTER_RULE_Y;
    c->footer_rule.width = WIN_HOR_RES;
    c->footer_rule.height = 1;
    c->footer_rule.bg_color = COLOR_PANEL_EDGE;

    /*The two navigation buttons are ordinary image buttons - the same widget
     *slide 06 is about. Both wrap around, so the deck never dead-ends.*/
    if(init_image_button(screen, &c->nav_prev, 32, 520, &nav_prev_normal, &nav_prev_pressed, NULL, NULL)) return 1;
    c->nav_prev.common.indev_event_cb = nav_prev_cb;

    if(init_image_button(screen, &c->nav_next, 96, 520, &nav_next_normal, &nav_next_pressed, NULL, NULL)) return 1;
    c->nav_next.common.indev_event_cb = nav_next_cb;

    /*The one string on screen this file builds instead of looking up: a 1-based
     *"01 / 11" counter, which reads the same in every language. Each screen owns
     *its own buffer, because the label keeps the pointer rather than a copy.*/
    (void) snprintf(step_text[slide], sizeof(step_text[slide]), "%02u / %02u",
                    (unsigned) (slide + 1u), (unsigned) SLIDE_CNT);

    err = ls_label_create(screen, &c->step);
    if(err != LS_ERROR_CODE_OK) { printf("step create failed: %d\n", err); return 1; }
    c->step.x = 170;
    c->step.y = 524;
    c->step.font = &montserrat_18;
    c->step.color = COLOR_TEXT;
    c->step.text = step_text[slide];

    err = ls_label_create(screen, &c->hint);
    if(err != LS_ERROR_CODE_OK) { printf("hint create failed: %d\n", err); return 1; }
    c->hint.x = 171;
    c->hint.y = 552;
    c->hint.font = &montserrat_12;
    c->hint.color = COLOR_TEXT_DIM;
    if(tr_bind(&c->hint, "footer_hint")) return 1;

    /*A plain `ls_button` with a label centred on top of it - the library has no
     *"button with text" widget; buttons are backgrounds and labels are text.*/
    err = ls_button_create(screen, &c->language_button);
    if(err != LS_ERROR_CODE_OK) { printf("language_button create failed: %d\n", err); return 1; }
    c->language_button.x = 866;
    c->language_button.y = 522;
    c->language_button.width = 140;
    c->language_button.height = 36;
    c->language_button.bg_color_normal = COLOR_CONTROL;
    c->language_button.bg_color_pressed = COLOR_ACCENT;
    c->language_button.bg_color_focused = COLOR_CONTROL_FOCUS;
    c->language_button.bg_color_pressed_focused = COLOR_ACCENT;
    c->language_button.focus_index = desc->focus_cnt - 1u;
    c->language_button.common.indev_event_cb = cycle_language_cb;

    err = ls_label_create(screen, &c->language_label);
    if(err != LS_ERROR_CODE_OK) { printf("language_label create failed: %d\n", err); return 1; }
    c->language_label.x = 936;
    c->language_label.y = 531;
    c->language_label.align = LS_LABEL_ALIGN_CENTER;
    c->language_label.font = &montserrat_14;
    c->language_label.color = COLOR_TEXT;
    if(tr_bind(&c->language_label, "language_name")) return 1;

    err = ls_label_create(screen, &c->language_caption);
    if(err != LS_ERROR_CODE_OK) { printf("language_caption create failed: %d\n", err); return 1; }
    c->language_caption.x = 852;
    c->language_caption.y = 532;
    c->language_caption.align = LS_LABEL_ALIGN_RIGHT;
    c->language_caption.font = &montserrat_12;
    c->language_caption.color = COLOR_TEXT_DIM;
    if(tr_bind(&c->language_caption, "footer_lang_caption")) return 1;

    err = ls_label_create(screen, &c->note);
    if(err != LS_ERROR_CODE_OK) { printf("note create failed: %d\n", err); return 1; }
    c->note.x = 1006;
    c->note.y = 566;
    c->note.align = LS_LABEL_ALIGN_RIGHT;
    c->note.font = &montserrat_12;
    c->note.color = COLOR_TEXT_DIM;
    if(tr_bind(&c->note, "footer_note")) return 1;

    return 0;
}

/* ============================ 01 - OVERVIEW ============================== */

static int init_slide_overview(ls_screen_t * screen)
{
    ls_error_code_t err;
    uint32_t i;

    for(i = 0; i < PILLAR_CNT; i++) {
        const pillar_desc_t * desc = &pillar_desc[i];

        err = ls_rectangle_create(screen, &pillar_edges[i]);
        if(err != LS_ERROR_CODE_OK) { printf("pillar_edges[%u] create failed: %d\n", i, err); return 1; }
        pillar_edges[i].x = desc->x;
        pillar_edges[i].y = desc->y;
        pillar_edges[i].width = 250;
        pillar_edges[i].height = 150;
        pillar_edges[i].bg_color = COLOR_PANEL_EDGE;

        err = ls_rectangle_create(screen, &pillar_faces[i]);
        if(err != LS_ERROR_CODE_OK) { printf("pillar_faces[%u] create failed: %d\n", i, err); return 1; }
        pillar_faces[i].x = desc->x + 1u;
        pillar_faces[i].y = desc->y + 1u;
        pillar_faces[i].width = 248;
        pillar_faces[i].height = 148;
        pillar_faces[i].bg_color = COLOR_PANEL_SUNKEN;

        err = ls_label_create(screen, &pillar_values[i]);
        if(err != LS_ERROR_CODE_OK) { printf("pillar_values[%u] create failed: %d\n", i, err); return 1; }
        pillar_values[i].x = desc->x + 125u;
        pillar_values[i].y = desc->y + 38u;
        pillar_values[i].align = LS_LABEL_ALIGN_CENTER;
        pillar_values[i].font = &montserrat_28;
        pillar_values[i].color = COLOR_ACCENT;
        pillar_values[i].text = desc->value;

        err = ls_label_create(screen, &pillar_captions[i]);
        if(err != LS_ERROR_CODE_OK) { printf("pillar_captions[%u] create failed: %d\n", i, err); return 1; }
        pillar_captions[i].x = desc->x + 125u;
        pillar_captions[i].y = desc->y + 96u;
        pillar_captions[i].align = LS_LABEL_ALIGN_CENTER;
        pillar_captions[i].font = &montserrat_12;
        pillar_captions[i].color = COLOR_TEXT_MUTED;
        if(tr_bind(&pillar_captions[i], desc->caption_tag)) return 1;
    }

    return 0;
}

/* =========================== 02 - LS_RECTANGLE =========================== */

static int init_slide_rectangle(ls_screen_t * screen)
{
    ls_error_code_t err;
    uint32_t i;

    if(init_caption(screen, &rect_ladder_caption, DEMO_X, 130, "rect_ladder_caption")) return 1;

    err = ls_rectangle_create(screen, &rect_ladder_paper);
    if(err != LS_ERROR_CODE_OK) { printf("rect_ladder_paper create failed: %d\n", err); return 1; }
    rect_ladder_paper.x = DEMO_X;
    rect_ladder_paper.y = 152;
    rect_ladder_paper.width = 480;
    rect_ladder_paper.height = 56;
    rect_ladder_paper.bg_color = COLOR_PAPER;

    for(i = 0; i < OPA_LADDER_CNT; i++) {
        err = ls_rectangle_create(screen, &rect_ladder[i]);
        if(err != LS_ERROR_CODE_OK) { printf("rect_ladder[%u] create failed: %d\n", i, err); return 1; }
        rect_ladder[i].x = DEMO_X + (i * 98u);
        rect_ladder[i].y = 152;
        rect_ladder[i].width = 88;
        rect_ladder[i].height = 56;
        rect_ladder[i].bg_color = COLOR_ACCENT;
        rect_ladder[i].bg_opa = (uint8_t)(51u * (i + 1u));
    }

    if(init_caption(screen, &rect_frame_caption, DEMO_X, 224, "rect_frame_caption")) return 1;

    err = ls_rectangle_create(screen, &rect_frame_edge);
    if(err != LS_ERROR_CODE_OK) { printf("rect_frame_edge create failed: %d\n", err); return 1; }
    rect_frame_edge.x = DEMO_X;
    rect_frame_edge.y = 246;
    rect_frame_edge.width = 200;
    rect_frame_edge.height = 64;
    rect_frame_edge.bg_color = COLOR_ACCENT;

    err = ls_rectangle_create(screen, &rect_frame_face);
    if(err != LS_ERROR_CODE_OK) { printf("rect_frame_face create failed: %d\n", err); return 1; }
    rect_frame_face.x = DEMO_X + 1u;
    rect_frame_face.y = 247;
    rect_frame_face.width = 198;
    rect_frame_face.height = 62;
    rect_frame_face.bg_color = COLOR_PANEL;

    err = ls_label_create(screen, &rect_frame_text);
    if(err != LS_ERROR_CODE_OK) { printf("rect_frame_text create failed: %d\n", err); return 1; }
    rect_frame_text.x = DEMO_X + 100u;
    rect_frame_text.y = 269;
    rect_frame_text.align = LS_LABEL_ALIGN_CENTER;
    rect_frame_text.font = &montserrat_14;
    rect_frame_text.color = COLOR_TEXT;
    if(tr_bind(&rect_frame_text, "rect_frame_text")) return 1;

    if(init_caption(screen, &rect_order_caption, DEMO_X, 330, "rect_order_caption")) return 1;

    /*Three squares created in sequence: the last one wins the overlap.*/
    for(i = 0; i < Z_ORDER_CNT; i++) {
        static const ls_color_t order_colors[Z_ORDER_CNT] = {
            COLOR_ALERT, COLOR_WARN, COLOR_TEAL
        };

        err = ls_rectangle_create(screen, &rect_order_squares[i]);
        if(err != LS_ERROR_CODE_OK) { printf("rect_order_squares[%u] create failed: %d\n", i, err); return 1; }
        rect_order_squares[i].x = DEMO_X + (i * 50u);
        rect_order_squares[i].y = 352u + (i * 16u);
        rect_order_squares[i].width = 72;
        rect_order_squares[i].height = 72;
        rect_order_squares[i].bg_color = order_colors[i];
    }

    if(init_caption(screen, &rect_order_note, 640, 384, "rect_order_note")) return 1;

    return 0;
}

/* ============================= 03 - LS_LABEL ============================= */

static int init_slide_label(ls_screen_t * screen)
{
    ls_error_code_t err;
    uint32_t i;

    if(init_caption(screen, &label_align_caption, DEMO_X, 126, "lbl_align_caption")) return 1;

    err = ls_rectangle_create(screen, &label_align_guide);
    if(err != LS_ERROR_CODE_OK) { printf("label_align_guide create failed: %d\n", err); return 1; }
    label_align_guide.x = DEMO_X;
    label_align_guide.y = 148;
    label_align_guide.width = 480;
    label_align_guide.height = 92;
    label_align_guide.bg_color = COLOR_PANEL_SUNKEN;

    /*All three lables share the same box and differ only in `align`, so the loop index
     *doubles as the alignment - LS_LABEL_ALIGN_LEFT/CENTER/RIGHT are 0/1/2. The
     *box is the guide rectangle inset by 12 px on each side, so right-aligned
     *text stops short of the frame instead of touching it.*/
    for(i = 0; i < ALIGN_DEMO_CNT; i++) {
        err = ls_label_create(screen, &label_aligns[i]);
        if(err != LS_ERROR_CODE_OK) { printf("label_aligns[%u] create failed: %d\n", i, err); return 1; }
        label_aligns[i].x = DEMO_X + 12u;
        label_aligns[i].y = 156u + (i * 30u);
        label_aligns[i].width = 456;
        label_aligns[i].align = (lv_label_align_t) i;
        label_aligns[i].font = &montserrat_14;
        label_aligns[i].color = COLOR_TEXT_MUTED;
        if(tr_bind(&label_aligns[i], align_demo_tags[i])) return 1;
    }

    if(init_caption(screen, &label_style_caption, DEMO_X, 254, "lbl_style_caption")) return 1;

    err = ls_label_create(screen, &label_style_demo);
    if(err != LS_ERROR_CODE_OK) { printf("label_style_demo create failed: %d\n", err); return 1; }
    label_style_demo.x = DEMO_X;
    label_style_demo.y = 276;
    label_style_demo.font = &montserrat_18;
    label_style_demo.color = COLOR_TEXT;
    label_style_demo.letter_space = 4;
    label_style_demo.underline_thickness = 2;
    if(tr_bind(&label_style_demo, "lbl_style_demo")) return 1;

    if(init_caption(screen, &label_bind_caption, DEMO_X, 318, "lbl_bind_caption")) return 1;

    /*Point the label at an int32_t and give it a printf format. Nothing has to
     *push the new value into the widget - the renderer formats whatever the
     *variable holds, every frame.*/
    err = ls_label_create(screen, &label_bind_demo);
    if(err != LS_ERROR_CODE_OK) { printf("label_bind_demo create failed: %d\n", err); return 1; }
    label_bind_demo.x = DEMO_X;
    label_bind_demo.y = 340;
    label_bind_demo.font = &montserrat_18;
    label_bind_demo.color = COLOR_OK;
    label_bind_demo.bind_int = &frame_count;
    /*The format itself is translated, so apply_language() owns bind_fmt.*/

    if(init_caption(screen, &label_translation_caption, DEMO_X, 382,
                    "lbl_translation_caption")) return 1;

    /*A label shows `text` if set, then `bind_int`, then `translation` - so
     *`text` is deliberately left NULL here.*/
    err = ls_label_create(screen, &label_translation_demo);
    if(err != LS_ERROR_CODE_OK) { printf("label_translation_demo create failed: %d\n", err); return 1; }
    label_translation_demo.x = DEMO_X;
    label_translation_demo.y = 404;
    label_translation_demo.font = &montserrat_18;
    label_translation_demo.color = COLOR_ACCENT;
    if(tr_bind(&label_translation_demo, "lbl_translation_demo")) return 1;

    return 0;
}

/* ============================== 04 - LS_ARC ============================== */

static int init_slide_arc(ls_screen_t * screen)
{
    ls_error_code_t err;
    uint32_t i;

    /*There is no gauge widget. The gauge on this screen is three arcs and two
     *labels stacked on one centre, created in the order they have to paint: the
     *dim full-sweep track first, then the value on top of it.*/
    err = ls_arc_create(screen, &arc_track);
    if(err != LS_ERROR_CODE_OK) { printf("arc_track create failed: %d\n", err); return 1; }
    arc_track.center_x = 620;
    arc_track.center_y = 290;
    arc_track.radius = 120;
    arc_track.thickness = 22;
    arc_track.start_deg = GAUGE_START_DEG;
    arc_track.end_deg = GAUGE_START_DEG + GAUGE_SWEEP_DEG;
    arc_track.color = COLOR_TRACK;

    /*The value arc: same geometry, shorter sweep. `end_deg` is recomputed in
     *animate() from `gauge_value`.*/
    err = ls_arc_create(screen, &arc_fill);
    if(err != LS_ERROR_CODE_OK) { printf("arc_fill create failed: %d\n", err); return 1; }
    arc_fill.center_x = 620;
    arc_fill.center_y = 290;
    arc_fill.radius = 120;
    arc_fill.thickness = 22;
    arc_fill.start_deg = GAUGE_START_DEG;
    arc_fill.end_deg = GAUGE_START_DEG + 1u;
    arc_fill.color = COLOR_TEAL;

    /*The limit marker: a 10 deg stub on a wider radius, sitting outside the
     *track at 80 % - the point where animate() turns the value arc red.*/
    err = ls_arc_create(screen, &arc_limit_marker);
    if(err != LS_ERROR_CODE_OK) { printf("arc_limit_marker create failed: %d\n", err); return 1; }
    arc_limit_marker.center_x = 620;
    arc_limit_marker.center_y = 290;
    arc_limit_marker.radius = 140;
    arc_limit_marker.thickness = 6;
    arc_limit_marker.start_deg = GAUGE_START_DEG + ((GAUGE_SWEEP_DEG * 80u) / 100u);
    arc_limit_marker.end_deg = arc_limit_marker.start_deg + 10u;
    arc_limit_marker.color = COLOR_ALERT;

    err = ls_label_create(screen, &arc_value);
    if(err != LS_ERROR_CODE_OK) { printf("arc_value create failed: %d\n", err); return 1; }
    arc_value.x = 620;
    arc_value.y = 268;
    arc_value.align = LS_LABEL_ALIGN_CENTER;
    arc_value.font = &montserrat_28;
    arc_value.color = COLOR_TEXT;
    arc_value.bind_int = &gauge_value;

    err = ls_label_create(screen, &arc_unit);
    if(err != LS_ERROR_CODE_OK) { printf("arc_unit create failed: %d\n", err); return 1; }
    arc_unit.x = 620;
    arc_unit.y = 312;
    arc_unit.align = LS_LABEL_ALIGN_CENTER;
    arc_unit.font = &montserrat_12;
    arc_unit.color = COLOR_TEXT_DIM;
    if(tr_bind(&arc_unit, "unit_percent")) return 1;

    for(i = 0; i < ARC_LEGEND_CNT; i++) {
        const arc_legend_desc_t * desc = &arc_legend_desc[i];

        err = ls_rectangle_create(screen, &arc_legend_swatches[i]);
        if(err != LS_ERROR_CODE_OK) { printf("arc_legend_swatches[%u] create failed: %d\n", i, err); return 1; }
        arc_legend_swatches[i].x = 810;
        arc_legend_swatches[i].y = desc->y;
        arc_legend_swatches[i].width = 14;
        arc_legend_swatches[i].height = 14;
        arc_legend_swatches[i].bg_color = desc->color;

        err = ls_label_create(screen, &arc_legend_names[i]);
        if(err != LS_ERROR_CODE_OK) { printf("arc_legend_names[%u] create failed: %d\n", i, err); return 1; }
        arc_legend_names[i].x = 834;
        arc_legend_names[i].y = desc->y - 1u;
        arc_legend_names[i].font = &montserrat_14;
        arc_legend_names[i].color = COLOR_TEXT_MUTED;
        if(tr_bind(&arc_legend_names[i], desc->name_tag)) return 1;
    }

    if(init_caption(screen, &arc_caption, DEMO_X, 440, "arc_caption")) return 1;

    return 0;
}

/* ============================ 05 - LS_BUTTON ============================= */

static int init_slide_button(ls_screen_t * screen)
{
    ls_error_code_t err;
    uint32_t i;

    for(i = 0; i < STATE_BUTTON_CNT; i++) {
        const state_button_desc_t * desc = &state_button_desc[i];

        err = ls_button_create(screen, &state_buttons[i]);
        if(err != LS_ERROR_CODE_OK) { printf("state_buttons[%u] create failed: %d\n", i, err); return 1; }
        state_buttons[i].x = desc->x;
        state_buttons[i].y = desc->y;
        state_buttons[i].width = 200;
        state_buttons[i].height = 70;

        /*One colour per state. The button picks the matching one at render
         *time; the application never repaints anything itself.*/
        state_buttons[i].bg_color_normal = COLOR_CONTROL;
        state_buttons[i].bg_color_pressed = COLOR_ACCENT;
        state_buttons[i].bg_color_focused = COLOR_CONTROL_FOCUS;
        state_buttons[i].bg_color_pressed_focused = COLOR_ACCENT;
        state_buttons[i].bg_color_disabled = COLOR_CONTROL_OFF;
        state_buttons[i].bg_color_checked = COLOR_OK;
        state_buttons[i].bg_color_checked_pressed = COLOR_OK_PRESSED;
        state_buttons[i].bg_color_checked_focused = COLOR_OK;
        state_buttons[i].bg_color_checked_pressed_focused = COLOR_OK_PRESSED;
        state_buttons[i].bg_opa_checked = LS_OPA_COVER;
        state_buttons[i].bg_opa_checked_pressed = LS_OPA_COVER;
        state_buttons[i].bg_opa_checked_focused = LS_OPA_COVER;
        state_buttons[i].bg_opa_checked_pressed_focused = LS_OPA_COVER;

        err = ls_label_create(screen, &state_button_texts[i]);
        if(err != LS_ERROR_CODE_OK) { printf("state_button_texts[%u] create failed: %d\n", i, err); return 1; }
        state_button_texts[i].x = desc->x + 100u;
        state_button_texts[i].y = desc->y + 26u;
        state_button_texts[i].align = LS_LABEL_ALIGN_CENTER;
        state_button_texts[i].font = &montserrat_14;
        state_button_texts[i].color = COLOR_TEXT;
        if(tr_bind(&state_button_texts[i], desc->text_tag)) return 1;

        err = ls_label_create(screen, &state_button_captions[i]);
        if(err != LS_ERROR_CODE_OK) { printf("state_button_captions[%u] create failed: %d\n", i, err); return 1; }
        state_button_captions[i].x = desc->x + 100u;
        state_button_captions[i].y = desc->y + 82u;
        state_button_captions[i].align = LS_LABEL_ALIGN_CENTER;
        state_button_captions[i].font = &montserrat_12;
        state_button_captions[i].color = COLOR_TEXT_DIM;
        if(tr_bind(&state_button_captions[i], desc->caption_tag)) return 1;
    }

    /*Button 0 keeps the plain normal/pressed look and takes focus slot 0.*/
    state_buttons[0].focus_index = 0;

    /*Button 1 latches: its callback flips `checked`, which selects the
     *checked_* colours from then on.*/
    state_buttons[1].checked = true;
    state_buttons[1].focus_index = 1;
    state_buttons[1].common.indev_event_cb = state_button_toggle_cb;
    state_button_texts[1].color = COLOR_TEXT_ON_OK;

    /*Button 2 is disabled: it renders the disabled colour and its built-in click
     *test rejects every point, so it can never fire an event. `focus_index` is
     *left at its default 0, which does not matter - a disabled button draws the
     *disabled colour whether the screen's focus is on it or not.*/
    state_buttons[2].disabled = true;
    state_button_texts[2].color = COLOR_TEXT_DIM;

    /*Button 3 is the focus demo - focus is one index per screen, not a flag per
     *widget, so exactly one widget can hold it at a time. That is also why the
     *three focusable buttons here number 0, 1, 2 and not 0, 1, 3.*/
    state_buttons[3].focus_index = 2;

    if(init_caption(screen, &state_button_note, DEMO_X, 420, "btn_note")) return 1;

    return 0;
}

/* ========================= 06 - LS_IMAGE_BUTTON ========================== */

static int init_slide_image_button(ls_screen_t * screen)
{
    ls_error_code_t err;
    uint32_t i;

    /*Checked: the switch swaps its whole artwork when `checked` flips.*/
    if(init_image_button(screen, &demo_switch, 470, image_button_desc[0].y,
                         &switch_off, &switch_off, &switch_on, &switch_on)) return 1;
    demo_switch.checked = true;
    demo_switch.common.indev_event_cb = demo_switch_cb;

    /*Pressed: momentary artwork, no latching.*/
    if(init_image_button(screen, &demo_press_button, 470, image_button_desc[1].y,
                         &nav_next_normal, &nav_next_pressed, NULL, NULL)) return 1;
    demo_press_button.common.indev_event_cb = demo_press_button_cb;

    /*Disabled: the built-in click test rejects everything.*/
    if(init_image_button(screen, &demo_disabled_button, 470, image_button_desc[2].y,
                         &nav_prev_normal, &nav_prev_pressed, NULL, NULL)) return 1;
    demo_disabled_button.disabled = true;

    for(i = 0; i < IMAGE_BUTTON_CNT; i++) {
        const image_button_desc_t * desc = &image_button_desc[i];

        err = ls_label_create(screen, &image_button_captions[i]);
        if(err != LS_ERROR_CODE_OK) { printf("image_button_captions[%u] create failed: %d\n", i, err); return 1; }
        image_button_captions[i].x = 580;
        image_button_captions[i].y = desc->y - 2u;
        image_button_captions[i].font = &montserrat_18;
        image_button_captions[i].color = COLOR_TEXT;
        if(tr_bind(&image_button_captions[i], desc->caption_tag)) return 1;

        err = ls_label_create(screen, &image_button_sub_captions[i]);
        if(err != LS_ERROR_CODE_OK) { printf("image_button_sub_captions[%u] create failed: %d\n", i, err); return 1; }
        image_button_sub_captions[i].x = 580;
        image_button_sub_captions[i].y = desc->y + 26u;
        image_button_sub_captions[i].font = &montserrat_12;
        image_button_sub_captions[i].color = COLOR_TEXT_DIM;
        if(tr_bind(&image_button_sub_captions[i], desc->sub_caption_tag)) return 1;
    }

    if(init_caption(screen, &image_button_note, DEMO_X, 430, "ibtn_note")) return 1;

    return 0;
}

/* ============================= 07 - LS_IMAGE ============================= */

static int init_slide_image(ls_screen_t * screen)
{
    ls_error_code_t err;
    uint32_t i;

    /*Rotation is done by the software renderer around `pivot_x/pivot_y`, given
     *relative to the image's own top-left corner.*/
    err = ls_image_create(screen, &spinning_logo);
    if(err != LS_ERROR_CODE_OK) { printf("spinning_logo create failed: %d\n", err); return 1; }
    spinning_logo.x = 540;
    spinning_logo.y = 180;
    spinning_logo.src = &img_benchmark_lvgl_logo_argb;
    spinning_logo.pivot_x = img_benchmark_lvgl_logo_argb.width / 2;
    spinning_logo.pivot_y = img_benchmark_lvgl_logo_argb.height / 2;

    err = ls_label_create(screen, &spinning_logo_caption);
    if(err != LS_ERROR_CODE_OK) { printf("spinning_logo_caption create failed: %d\n", err); return 1; }
    spinning_logo_caption.x = 590;
    spinning_logo_caption.y = 318;
    spinning_logo_caption.align = LS_LABEL_ALIGN_CENTER;
    spinning_logo_caption.font = &montserrat_12;
    spinning_logo_caption.color = COLOR_TEXT_DIM;
    if(tr_bind(&spinning_logo_caption, "img_logo_caption")) return 1;

    /*An A8 source is a mask - one alpha byte per pixel and no colour of its own.
     *The colour comes from `a8_color`, so one bitmap can be re-tinted per state
     *without a second asset.*/
    err = ls_image_create(screen, &tinted_glyph);
    if(err != LS_ERROR_CODE_OK) { printf("tinted_glyph create failed: %d\n", err); return 1; }
    tinted_glyph.x = 800;
    tinted_glyph.y = 206;
    tinted_glyph.src = &fault_triangle_a8;
    tinted_glyph.a8_color = COLOR_WARN;

    err = ls_label_create(screen, &tinted_glyph_caption);
    if(err != LS_ERROR_CODE_OK) { printf("tinted_glyph_caption create failed: %d\n", err); return 1; }
    tinted_glyph_caption.x = 824;
    tinted_glyph_caption.y = 276;
    tinted_glyph_caption.align = LS_LABEL_ALIGN_CENTER;
    tinted_glyph_caption.font = &montserrat_12;
    tinted_glyph_caption.color = COLOR_TEXT_DIM;
    if(tr_bind(&tinted_glyph_caption, "img_glyph_caption")) return 1;

    for(i = 0; i < TINT_SAMPLE_CNT; i++) {
        static const ls_color_t tints[TINT_SAMPLE_CNT] = {
            COLOR_OK, COLOR_WARN, COLOR_ALERT
        };

        err = ls_image_create(screen, &tint_samples[i]);
        if(err != LS_ERROR_CODE_OK) { printf("tint_samples[%u] create failed: %d\n", i, err); return 1; }
        tint_samples[i].x = 500u + (i * 100u);
        tint_samples[i].y = 356;
        tint_samples[i].src = &fault_triangle_a8;
        tint_samples[i].a8_color = tints[i];
    }

    if(init_caption(screen, &tint_samples_caption, DEMO_X + 10, 424, "img_tints_caption")) return 1;

    return 0;
}

/* ==================== 08 - HOOK 1: COMMON.INDEV_EVENT_CB ================= */

static int init_slide_hook_events(ls_screen_t * screen)
{
    ls_error_code_t err;
    uint32_t i;

    err = ls_button_create(screen, &events_button);
    if(err != LS_ERROR_CODE_OK) { printf("events_button create failed: %d\n", err); return 1; }
    events_button.x = 460;
    events_button.y = 150;
    events_button.width = 220;
    events_button.height = 90;
    events_button.bg_color_normal = COLOR_CONTROL;
    events_button.bg_color_pressed = COLOR_ACCENT;
    events_button.bg_color_focused = COLOR_CONTROL_FOCUS;
    events_button.bg_color_pressed_focused = COLOR_ACCENT;
    events_button.focus_index = 0;

    /*HOOK 1 - the event callback. A button already knows how to hit-test itself,
     *so this one field is the whole wiring; see events_button_cb().*/
    events_button.common.indev_event_cb = events_button_cb;

    err = ls_label_create(screen, &events_button_text);
    if(err != LS_ERROR_CODE_OK) { printf("events_button_text create failed: %d\n", err); return 1; }
    events_button_text.x = 570;
    events_button_text.y = 183;
    events_button_text.align = LS_LABEL_ALIGN_CENTER;
    events_button_text.font = &montserrat_18;
    events_button_text.color = COLOR_TEXT;
    if(tr_bind(&events_button_text, "ev_button_text")) return 1;

    /*One row per event type. The number is a `bind_int` label pointed at a
     *slot of `event_counts`, so the callback only has to increment an int.*/
    for(i = 0; i < EVENT_COUNTER_CNT; i++) {
        err = ls_label_create(screen, &event_counter_names[i]);
        if(err != LS_ERROR_CODE_OK) { printf("event_counter_names[%u] create failed: %d\n", i, err); return 1; }
        event_counter_names[i].x = 720;
        event_counter_names[i].y = event_counter_desc[i].y + 9u;
        event_counter_names[i].font = &montserrat_14;
        event_counter_names[i].color = COLOR_TEXT_DIM;
        if(tr_bind(&event_counter_names[i], event_counter_desc[i].name_tag)) return 1;

        err = ls_label_create(screen, &event_counter_values[i]);
        if(err != LS_ERROR_CODE_OK) { printf("event_counter_values[%u] create failed: %d\n", i, err); return 1; }
        event_counter_values[i].x = 960;
        event_counter_values[i].y = event_counter_desc[i].y - 9u;
        event_counter_values[i].align = LS_LABEL_ALIGN_RIGHT;
        event_counter_values[i].font = &montserrat_28;
        event_counter_values[i].color = COLOR_TEXT;
        event_counter_values[i].bind_int = &event_counts[i];
    }

    /*`text` points at a buffer apply_language() rewrites - the other way to make
     *a label dynamic when the value is not an integer.*/
    err = ls_label_create(screen, &events_last);
    if(err != LS_ERROR_CODE_OK) { printf("events_last create failed: %d\n", err); return 1; }
    events_last.x = 460;
    events_last.y = 290;
    events_last.font = &montserrat_18;
    events_last.color = COLOR_ACCENT;
    events_last.text = events_last_text;

    if(init_caption(screen, &events_note_bind, 460, 336, "ev_note_bind")) return 1;
    if(init_caption(screen, &events_note_pressing, 460, 362, "ev_note_pressing")) return 1;

    return 0;
}

/* ================= 09 - HOOK 2: COMMON.INDEV_CLICK_TEST_CB =============== */

static int init_slide_hook_click_test(ls_screen_t * screen)
{
    ls_error_code_t err;

    err = ls_arc_create(screen, &knob_track);
    if(err != LS_ERROR_CODE_OK) { printf("knob_track create failed: %d\n", err); return 1; }
    knob_track.center_x = 620;
    knob_track.center_y = 270;
    knob_track.radius = 100;
    knob_track.thickness = 24;
    knob_track.start_deg = GAUGE_START_DEG;
    knob_track.end_deg = GAUGE_START_DEG + GAUGE_SWEEP_DEG;
    knob_track.color = COLOR_TRACK;

    /*HOOK 2 - the hit test. Assigning a callback to `indev_click_test_cb` is
     *all it takes for the input engine to start offering points to this
     *widget; see knob_click_test_cb() for the implementation.*/
    knob_track.common.indev_click_test_cb = knob_click_test_cb;
    knob_track.common.indev_event_cb = knob_cb;

    err = ls_arc_create(screen, &knob_fill);
    if(err != LS_ERROR_CODE_OK) { printf("knob_fill create failed: %d\n", err); return 1; }
    knob_fill.center_x = 620;
    knob_fill.center_y = 270;
    knob_fill.radius = 100;
    knob_fill.thickness = 24;
    knob_fill.start_deg = GAUGE_START_DEG;
    knob_fill.end_deg = GAUGE_START_DEG + 1u;
    knob_fill.color = COLOR_OK;

    err = ls_label_create(screen, &knob_value);
    if(err != LS_ERROR_CODE_OK) { printf("knob_value create failed: %d\n", err); return 1; }
    knob_value.x = 620;
    knob_value.y = 250;
    knob_value.align = LS_LABEL_ALIGN_CENTER;
    knob_value.font = &montserrat_28;
    knob_value.color = COLOR_TEXT;
    knob_value.bind_int = &knob_value_pct;

    err = ls_label_create(screen, &knob_unit);
    if(err != LS_ERROR_CODE_OK) { printf("knob_unit create failed: %d\n", err); return 1; }
    knob_unit.x = 620;
    knob_unit.y = 294;
    knob_unit.align = LS_LABEL_ALIGN_CENTER;
    knob_unit.font = &montserrat_12;
    knob_unit.color = COLOR_TEXT_DIM;
    if(tr_bind(&knob_unit, "unit_percent")) return 1;

    err = ls_label_create(screen, &knob_hint_left);
    if(err != LS_ERROR_CODE_OK) { printf("knob_hint_left create failed: %d\n", err); return 1; }
    knob_hint_left.x = KNOB_HINT_X;
    knob_hint_left.y = 230;
    knob_hint_left.font = &montserrat_14;
    knob_hint_left.color = COLOR_TEXT_MUTED;
    if(tr_bind(&knob_hint_left, "hit_tap_left")) return 1;

    err = ls_label_create(screen, &knob_step_left);
    if(err != LS_ERROR_CODE_OK) { printf("knob_step_left create failed: %d\n", err); return 1; }
    knob_step_left.x = KNOB_HINT_STEP_X;
    knob_step_left.y = 230;
    knob_step_left.align = LS_LABEL_ALIGN_RIGHT;
    knob_step_left.font = &montserrat_14;
    knob_step_left.color = COLOR_TEXT_MUTED;
    if(tr_bind(&knob_step_left, "hit_step_minus")) return 1;

    err = ls_label_create(screen, &knob_hint_right);
    if(err != LS_ERROR_CODE_OK) { printf("knob_hint_right create failed: %d\n", err); return 1; }
    knob_hint_right.x = KNOB_HINT_X;
    knob_hint_right.y = 262;
    knob_hint_right.font = &montserrat_14;
    knob_hint_right.color = COLOR_TEXT_MUTED;
    if(tr_bind(&knob_hint_right, "hit_tap_right")) return 1;

    err = ls_label_create(screen, &knob_step_right);
    if(err != LS_ERROR_CODE_OK) { printf("knob_step_right create failed: %d\n", err); return 1; }
    knob_step_right.x = KNOB_HINT_STEP_X;
    knob_step_right.y = 262;
    knob_step_right.align = LS_LABEL_ALIGN_RIGHT;
    knob_step_right.font = &montserrat_14;
    knob_step_right.color = COLOR_TEXT_MUTED;
    if(tr_bind(&knob_step_right, "hit_step_plus")) return 1;

    if(init_caption(screen, &knob_note_area, DEMO_X, 410, "hit_note_area")) return 1;
    if(init_caption(screen, &knob_note_point, DEMO_X, 436, "hit_note_point")) return 1;

    return 0;
}

/* ===================== 10 - HOOK 3: COMMON.RENDER_CB ===================== */

static int init_slide_hook_render(ls_screen_t * screen)
{
    ls_error_code_t err;

    if(init_waveform(screen, &waveform, 450, 150, WAVE_W, WAVE_H)) return 1;

    /*The switch takes the waveform off the screen and puts it back. Widgets are
     *never destroyed in LVGL Safe, so `hidden` is the only way to do that - the
     *struct and its slot in the screen's list stay exactly where they were.*/
    if(init_image_button(screen, &waveform_switch, 450, 348,
                         &switch_off, &switch_off, &switch_on, &switch_on)) return 1;
    waveform_switch.checked = true;
    waveform_switch.common.indev_event_cb = waveform_switch_cb;

    err = ls_label_create(screen, &waveform_switch_caption);
    if(err != LS_ERROR_CODE_OK) { printf("waveform_switch_caption create failed: %d\n", err); return 1; }
    waveform_switch_caption.x = 546;
    waveform_switch_caption.y = 346;
    waveform_switch_caption.font = &montserrat_18;
    waveform_switch_caption.color = COLOR_TEXT;
    if(tr_bind(&waveform_switch_caption, "rcb_switch_caption")) return 1;

    if(init_caption(screen, &waveform_switch_sub_caption, 546, 374, "rcb_switch_sub")) return 1;
    if(init_caption(screen, &waveform_note, 450, 424, "rcb_note")) return 1;

    return 0;
}

/* ===================== 11 - HOOK 4: SCREEN.RENDER_CB ===================== */

static int init_slide_hook_screen_render(ls_screen_t * screen)
{
    ls_error_code_t err;
    uint32_t i;

    /*The switch does not touch the hook, only the flag which the hook reads: turning it
     *off makes gradient_screen_render_cb() delegate to the library's flat fill,
     *so you can see both backdrops without swapping any callback.*/
    if(init_image_button(screen, &gradient_switch, 450, 150,
                         &switch_off, &switch_off, &switch_on, &switch_on)) return 1;
    gradient_switch.checked = true;
    gradient_switch.common.indev_event_cb = gradient_switch_cb;

    err = ls_label_create(screen, &gradient_switch_caption);
    if(err != LS_ERROR_CODE_OK) { printf("gradient_switch_caption create failed: %d\n", err); return 1; }
    gradient_switch_caption.x = 546;
    gradient_switch_caption.y = 148;
    gradient_switch_caption.font = &montserrat_18;
    gradient_switch_caption.color = COLOR_TEXT;
    if(tr_bind(&gradient_switch_caption, "scr_switch_caption")) return 1;

    if(init_caption(screen, &gradient_switch_sub_caption, 546, 176, "scr_switch_sub")) return 1;

    err = ls_rectangle_create(screen, &render_order_panel);
    if(err != LS_ERROR_CODE_OK) { printf("render_order_panel create failed: %d\n", err); return 1; }
    render_order_panel.x = 450;
    render_order_panel.y = 230;
    render_order_panel.width = 490;
    render_order_panel.height = 180;
    render_order_panel.bg_color = COLOR_PANEL_SUNKEN;

    if(init_caption(screen, &render_order_caption, RENDER_STEP_INDEX_X, 248, "scr_order_caption")) return 1;

    for(i = 0; i < RENDER_STEP_CNT; i++) {
        uint32_t row_y = 280u + (i * 32u);

        err = ls_label_create(screen, &render_order_indices[i]);
        if(err != LS_ERROR_CODE_OK) { printf("render_order_indices[%u] create failed: %d\n", i, err); return 1; }
        render_order_indices[i].x = RENDER_STEP_INDEX_X;
        render_order_indices[i].y = row_y;
        render_order_indices[i].font = &montserrat_14;
        render_order_indices[i].color = COLOR_TEXT_MUTED;
        render_order_indices[i].text = render_step_index[i];

        err = ls_label_create(screen, &render_order_steps[i]);
        if(err != LS_ERROR_CODE_OK) { printf("render_order_steps[%u] create failed: %d\n", i, err); return 1; }
        render_order_steps[i].x = RENDER_STEP_TEXT_X;
        render_order_steps[i].y = row_y;
        render_order_steps[i].font = &montserrat_14;
        render_order_steps[i].color = COLOR_TEXT_MUTED;
        if(tr_bind(&render_order_steps[i], render_step_tags[i])) return 1;
    }

    if(init_caption(screen, &render_order_note, 450, 430, "scr_note")) return 1;

    return 0;
}

/* ===================== HOOK 3 - A USER-DRAWN WIDGET ====================== */

static int init_waveform(ls_screen_t * screen, waveform_t * wave, uint32_t x, uint32_t y,
                         uint32_t width, uint32_t height)
{
    /*Create it as an ordinary rectangle first - that registers it on the screen
     *and installs the stock hooks and defaults.*/
    ls_error_code_t err = ls_rectangle_create(screen, &wave->base);
    if(err != LS_ERROR_CODE_OK) { printf("waveform base create failed: %d\n", err); return 1; }

    wave->base.x = x;
    wave->base.y = y;
    wave->base.width = width;
    wave->base.height = height;
    wave->base.bg_color = COLOR_PANEL_SUNKEN;

    /*...then take the hook over. Keeping the original lets the custom hook
     *reuse the library's fill for the background instead of reimplementing it.*/
    wave->base_render_cb = wave->base.common.render_cb;
    wave->base.common.render_cb = waveform_render_cb;

    /*Fill the whole ring buffer up front so the trace is a continuous curve on
     *the first frame instead of growing in from the left.*/
    uint32_t i;
    for(i = 0; i < WAVE_SAMPLE_CNT; i++) {
        wave_samples[i] = wave_next_sample();
    }
    wave_head = 0;

    wave->samples = wave_samples;
    wave->sample_cnt = WAVE_SAMPLE_CNT;
    wave->head = 0;
    wave->trace_color = COLOR_TEAL;
    wave->fill_color = COLOR_TRACE_FILL;
    wave->grid_color = COLOR_TRACE_GRID;

    return 0;
}

/*One sample of the synthetic signal: a slow fundamental plus a smaller, faster
 *harmonic.*/
static int8_t wave_next_sample(void)
{
    wave_angle_slow = (wave_angle_slow + 3) % 360;
    wave_angle_fast = (wave_angle_fast + 8) % 360;

    const int32_t fundamental = (ls_sin(wave_angle_slow) * 62) / LS_TRIGO_SIN_MAX;
    const int32_t harmonic = (ls_sin(wave_angle_fast) * 22) / LS_TRIGO_SIN_MAX;

    return (int8_t)(fundamental + harmonic);
}

/**
 * Custom render hook for the waveform widget.
 *
 * Signature and contract are identical to the library's own widget hooks: get
 * the display, get the widget, return an `ls_error_code_t`. `ls_render` aborts
 * the frame if any hook reports an error, so this one bails out the moment the
 * frame-buffer accessor complains.
 */
static ls_error_code_t waveform_render_cb(ls_display_t * display, void * widget)
{
    if(display == NULL || widget == NULL) LS_RETURN_ERROR(LS_ERROR_CODE_INVALID_ARGUMENT);

    /*Safe because `ls_rectangle_t base` is the first member of waveform_t.*/
    waveform_t * wave = widget;

    /*Chain to the stock rectangle hook for the background fill.*/
    ls_error_code_t err = wave->base_render_cb(display, widget);
    if(err != LS_ERROR_CODE_OK) LS_RETURN_ERROR(err);

    if(wave->sample_cnt == 0u) LS_RETURN_ERROR(LS_ERROR_CODE_INVALID_CONFIGURATION);

    const int32_t x0 = (int32_t) wave->base.x;
    const int32_t y0 = (int32_t) wave->base.y;
    const int32_t w = (int32_t) wave->base.width;
    const int32_t h = (int32_t) wave->base.height;
    const int32_t mid_y = y0 + (h / 2);
    const int32_t amplitude = (h / 2) - 4;   /*4 px of headroom at full scale*/

    const uint16_t trace_c16 = ls_color_to_u16(wave->trace_color);
    const uint16_t fill_c16 = ls_color_to_u16(wave->fill_color);
    const uint16_t grid_c16 = ls_color_to_u16(wave->grid_color);

    int32_t xi;
    for(xi = 0; xi < w; xi++) {
        const int32_t x = x0 + xi;
        if(x < 0 || x >= (int32_t) display->hor_res) continue;

        /*`head` is the oldest sample, so reading forward from it puts the oldest
         *on the left and the newest on the right.*/
        const uint32_t index = (wave->head + (uint32_t) xi) % wave->sample_cnt;
        const int32_t value = wave->samples[index];
        const int32_t y = mid_y - ((value * amplitude) / 100);

        /*Fill the column between the zero line and the sample, then put the
         *bright trace pixel on the sample itself.*/
        const int32_t from_y = (y < mid_y) ? y : mid_y;
        const int32_t to_y = (y < mid_y) ? mid_y : y;

        int32_t yi;
        for(yi = from_y; yi <= to_y; yi++) {
            /*Stay inside the widget: a sample at full scale must not paint over
             *the panel behind it.*/
            if(yi <= y0 || yi >= (y0 + h)) continue;

            ls_frame_buffer_color_t * px;
            err = ls_render_goto_frame_buffer_px(display, &px, x, yi);
            if(err != LS_ERROR_CODE_OK) LS_RETURN_ERROR(err);

            *px = (yi == y) ? trace_c16 : fill_c16;
        }
    }

    /*The dotted zero line goes last. Every column above fills from its sample up
     *to and including `mid_y`.*/
    for(xi = 0; xi < w; xi += 16) {
        const int32_t x = x0 + xi;
        if(x < 0 || x >= (int32_t) display->hor_res) continue;

        ls_frame_buffer_color_t * px;
        err = ls_render_goto_frame_buffer_px(display, &px, x, mid_y);
        if(err != LS_ERROR_CODE_OK) LS_RETURN_ERROR(err);
        *px = grid_c16;
    }

    LS_RETURN_OK;
}

/* ================== HOOK 4 - A USER-DRAWN SCREEN BACKGROUND ============== */

/**
 * Custom render hook for a whole screen. `ls_render` calls this once, before
 * any widget, and the widgets then draw on top of whatever it left behind.
 */
static ls_error_code_t gradient_screen_render_cb(ls_display_t * display, void * widget)
{
    if(display == NULL || widget == NULL) LS_RETURN_ERROR(LS_ERROR_CODE_INVALID_ARGUMENT);

    /*When the gradient is switched off, hand the frame straight back to the
     *library's default flat fill */
    if(!background_gradient_on) {
        return stock_screen_render_cb(display, widget);
    }

    const ls_color_t top = COLOR_BG_TOP;
    const ls_color_t bottom = COLOR_BG_BOTTOM;
    const int32_t height = display->ver_res;
    if(height < 2) LS_RETURN_ERROR(LS_ERROR_CODE_INVALID_CONFIGURATION);

    ls_error_code_t err = LS_ERROR_CODE_OK;
    int32_t y;
    for(y = 0; y < height; y++) {
        /*Linear interpolation per row*/
        ls_color_t row;
        row.red = (uint8_t)(((int32_t) top.red * (height - 1 - y) + (int32_t) bottom.red * y) / (height - 1));
        row.green = (uint8_t)(((int32_t) top.green * (height - 1 - y) + (int32_t) bottom.green * y) / (height - 1));
        row.blue = (uint8_t)(((int32_t) top.blue * (height - 1 - y) + (int32_t) bottom.blue * y) / (height - 1));

        const uint16_t row_c16 = ls_color_to_u16(row);

        ls_frame_buffer_color_t * line;
        err = ls_render_goto_frame_buffer_px(display, &line, 0, y);
        if(err != LS_ERROR_CODE_OK) LS_RETURN_ERROR(err);

        int32_t x;
        for(x = 0; x < (int32_t) display->hor_res; x++) {
            line[x] = row_c16;
        }
    }

    LS_RETURN_OK;
}

/* ==================== HOOK 2 - A USER-DEFINED HIT TEST =================== */

/**
 * Hit test for the knob: accept a point only where the ring is actually drawn.
 * A point in the hole in the middle, out past the outer edge, or down in the gap
 * is rejected.
 *
 * The input engine calls this for every widget on every `ls_indev_process`, and
 * the last widget that accepts the point wins - that is the top-most one, since
 * widgets are walked in creation order.
 */
static bool knob_click_test_cb(ls_display_t * display, void * widget, uint32_t x, uint32_t y)
{
    UNUSED(display);

    const ls_arc_t * arc = widget;

    const int32_t dx = (int32_t) x - arc->center_x;
    const int32_t dy = (int32_t) y - arc->center_y;
    const int32_t distance_sq = (dx * dx) + (dy * dy);

    /*The ring runs from radius - thickness out to radius; the margin widens that
     *band on both sides so a fingertip does not have to be exact. Comparing
     *squared distances keeps the whole test in integers - no square root.*/
    int32_t outer = (int32_t) arc->radius + KNOB_TOUCH_MARGIN;
    int32_t inner = (int32_t) arc->radius - (int32_t) arc->thickness - KNOB_TOUCH_MARGIN;
    if(inner < 0) inner = 0;

    if(distance_sq > (outer * outer)) return false;
    if(distance_sq < (inner * inner)) return false;

    /*That band on its own is a full annulus, so it would also accept the 90 deg
     *the sweep never draws. With GAUGE_START_DEG 135 and GAUGE_SWEEP_DEG 270 the
     *gap runs 45..135 deg: the points below the centre that are no further out
     *sideways than they are down, dy >= |dx|. Two more integer compares, no
     *trigonometry.*/
    if((dy > 0) && (dy >= dx) && (dy >= -dx)) return false;

    knob_touch_direction = (dx < 0) ? -1 : 1;
    return true;
}

/* ============================ EVENT CALLBACKS ============================ */

/*Every callback below is an `ls_indev_event_cb_t`. It is handed the widget that
 *was hit and the event type. What it does with the model is entirely up to the
 *application.*/

static void nav_prev_cb(void * widget, ls_indev_event_type_t type)
{
    UNUSED(widget);

    if(type == LS_INDEV_EVENT_TYPE_CLICKED) {
        show_slide((active_slide == 0u) ? ((uint32_t) SLIDE_CNT - 1u) : (active_slide - 1u));
    }
}

static void nav_next_cb(void * widget, ls_indev_event_type_t type)
{
    UNUSED(widget);

    if(type == LS_INDEV_EVENT_TYPE_CLICKED) {
        show_slide((active_slide + 1u) % (uint32_t) SLIDE_CNT);
    }
}

static void cycle_language_cb(void * widget, ls_indev_event_type_t type)
{
    UNUSED(widget);

    if(type == LS_INDEV_EVENT_TYPE_CLICKED) {
        language_index = (language_index + 1u) % TR_LANGUAGE_CNT;

        /*Every label bound to a translation follows immediately - the call
         *rewrites the `current` pointer inside each `ls_translation_t`. The
         *whole deck changes language, not just the screen in view.*/
        if(tr_set_language(language_index)) return;

        apply_language();
    }
}

static void state_button_toggle_cb(void * widget, ls_indev_event_type_t type)
{
    ls_button_t * button = widget;
    if(button == NULL) return;

    if(type == LS_INDEV_EVENT_TYPE_CLICKED) {
        button->checked = !button->checked;
        /*Keep the caption readable against the checked/unchecked background.*/
        state_button_texts[1].color = button->checked ? COLOR_TEXT_ON_OK : COLOR_TEXT;
    }
}

static void demo_switch_cb(void * widget, ls_indev_event_type_t type)
{
    ls_image_button_t * button = widget;
    if(button == NULL) return;

    if(type == LS_INDEV_EVENT_TYPE_CLICKED) {
        button->checked = !button->checked;
    }
}

static void demo_press_button_cb(void * widget, ls_indev_event_type_t type)
{
    UNUSED(widget);

    /*The momentary button on slide 06.*/
    if(type == LS_INDEV_EVENT_TYPE_CLICKED) {
        glyph_alert = !glyph_alert;
    }
}

static void events_button_cb(void * widget, ls_indev_event_type_t type)
{
    UNUSED(widget);

    switch(type) {
        case LS_INDEV_EVENT_TYPE_PRESSED:
            events_last_index = 0;
            break;
        case LS_INDEV_EVENT_TYPE_PRESSING:
            events_last_index = 1;
            break;
        case LS_INDEV_EVENT_TYPE_CLICKED:
            events_last_index = 2;
            break;
        default:
            return;
    }

    event_counts[events_last_index]++;

    /*The event's own name is a translation, so the line is rebuilt here and
     *again on every language change.*/
    apply_language();
}

static void knob_cb(void * widget, ls_indev_event_type_t type)
{
    UNUSED(widget);

    if(type == LS_INDEV_EVENT_TYPE_CLICKED) {
        knob_value_pct += knob_touch_direction * KNOB_STEP;
        if(knob_value_pct < 0) knob_value_pct = 0;
        if(knob_value_pct > 100) knob_value_pct = 100;
    }
}

static void waveform_switch_cb(void * widget, ls_indev_event_type_t type)
{
    ls_image_button_t * button = widget;
    if(button == NULL) return;

    if(type == LS_INDEV_EVENT_TYPE_CLICKED) {
        button->checked = !button->checked;
        waveform.base.common.hidden = !button->checked;
    }
}

static void gradient_switch_cb(void * widget, ls_indev_event_type_t type)
{
    ls_image_button_t * button = widget;
    if(button == NULL) return;

    if(type == LS_INDEV_EVENT_TYPE_CLICKED) {
        button->checked = !button->checked;
        background_gradient_on = button->checked;
    }
}

/* ============================== DECK / MODEL ============================= */

/*Switching screens is a single pointer write; every screen and every widget on
 *it already exists and stays alive.*/
static void show_slide(uint32_t slide)
{
    active_slide = slide;
    display_1.screen_active = &screens[slide];
}

/*Called on a fixed cadence from the main loop. It only touches the model, the
 *widgets pick up the new values on the next render.*/
static void animate(void)
{
    static bool gauge_rising = true;

    frame_count++;

    /*Gauge sweeps 10..95 % and back.*/
    if(gauge_rising) {
        gauge_value++;
        if(gauge_value >= 95) gauge_rising = false;
    }
    else {
        gauge_value--;
        if(gauge_value <= 10) gauge_rising = true;
    }

    arc_fill.end_deg = GAUGE_START_DEG + ((GAUGE_SWEEP_DEG * (uint32_t) gauge_value) / 100u);
    arc_fill.common.hidden = (gauge_value <= 0);
    /*80 % is where arc_limit_marker sits, so the fill turns red as it passes it.*/
    arc_fill.color = (gauge_value >= 80) ? COLOR_ALERT : COLOR_TEAL;

    knob_fill.end_deg = GAUGE_START_DEG + ((GAUGE_SWEEP_DEG * (uint32_t) knob_value_pct) / 100u);
    knob_fill.common.hidden = (knob_value_pct <= 0);

    /*Spin the ARGB logo and let its opacity breathe between 120 and 252.*/
    spin_deg = (spin_deg + 3) % 360;
    spinning_logo.rotation = spin_deg;
    if(spin_opa_falling) {
        spin_opa -= 3;
        if(spin_opa <= 120) spin_opa_falling = false;
    }
    else {
        spin_opa += 3;
        if(spin_opa >= 252) spin_opa_falling = true;
    }
    spinning_logo.opa = spin_opa;

    /*Re-tint slide 07's warning triangle from the flag slide 06's button sets.
     *Same A8 bitmap, different colour - no second asset.*/
    tinted_glyph.a8_color = glyph_alert ? COLOR_ALERT : COLOR_WARN;

    /*Push one new sample into the ring buffer the custom render hook reads.
     *Overwriting the oldest slot and stepping past it scrolls the trace one pixel
     *to the left; nothing is copied and nothing is reallocated.*/
    wave_samples[wave_head] = wave_next_sample();
    wave_head = (wave_head + 1u) % WAVE_SAMPLE_CNT;
    waveform.head = wave_head;
}

/* ============================ INPUT PLUMBING ============================= */

/*UP and DOWN walk the focus. Focus is a single index stored on the screen: a
 *button renders its focused colours when the screen's `active_focus_index`
 *matches its own `focus_index`, so moving focus is one integer write and the
 *library has no notion of a focus order beyond that.*/
static void handle_focus_key(SDL_Keycode key)
{
    ls_screen_t * screen = display_1.screen_active;
    const uint32_t focus_cnt = slide_desc[active_slide].focus_cnt;
    uint32_t index = screen->active_focus_index;

    if(key == SDLK_DOWN) {
        index = (index + 1u) % focus_cnt;
    }
    else if(key == SDLK_UP) {
        index = (index == 0u) ? (focus_cnt - 1u) : (index - 1u);
    }
    else {
        return;
    }

    screen->active_focus_index = index;
}

/* ============================== SDL BACKEND ============================== */

static void sdl_init(void)
{
    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
    }

    SDL_Window * window = SDL_CreateWindow(
                              "LVGL Safe - API Tour",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              WIN_HOR_RES, WIN_VER_RES, 0);

    if(!window) {
        printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565,
                                SDL_TEXTUREACCESS_STATIC, WIN_HOR_RES, WIN_VER_RES);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
}

static void sdl_update(const void * fb)
{
    SDL_UpdateTexture(texture, NULL, fb, WIN_HOR_RES * LS_BYTE_PER_PIXEL);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}
