/**
 * @file main
 *
 * A small demo LVGL Safe application: one display, two screens, one
 * widget of each basic kind, and two button callbacks.
 *
 * Every element on the screen has a dimmed tag naming the widget type for easier
 * identification.
 *
 * To get you started it is recommended to build and run the 'lvgl_safe_api_tour'
 * example to see what capabilities LVGL Safe provides then come back to this
 * 'basic example' to see how the widgets are to be set up, populated and 
 * interacted with. 
 * 
 * This file is meant to show the general shape every LVGL Safe program has:
 *
 *   1. Give the display a frame buffer you own.
 *   2. Init a screen, then create widgets onto it.
 *   3. Configure widgets by writing their struct fields.
 *   4. Loop: feed input, render, flush to the panel.
 *
 * Two properties of the library are worth noticing while reading:
 *   - Every widget is a caller-owned `static` struct created once at start-up.
 *     Nothing is allocated at run-time and nothing is ever destroyed.
 *   - Every function call inside the library API which can fail 
 *     returns an `ls_error_code_t`, and it is checked here
 *     at every call site.
 *
 * Demo Usage:
 * 
 * Click STEP +10 / RESET to drive the value on the dial, notice the readout 
 * and the arc both follow it. The switch shows/hides the arc.
 * 
 * Click on the arrow icons to switch between the two screens already set up. 
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
#include <ls_rectangle.h>
#include <ls_render.h>
#include <ls_screen.h>

/*********************
 *      DEFINES
 *********************/

/*Define the size of your display here.*/
#define WIN_HOR_RES 800
#define WIN_VER_RES 480

#define UNUSED(x) (void)(x)

#define COLOR_BG      LS_COLOR_HEX(0x14181d)
#define COLOR_PANEL   LS_COLOR_HEX(0x1e242b)
#define COLOR_TEXT    LS_COLOR_HEX(0xe6ebef)
#define COLOR_DIM     LS_COLOR_HEX(0x8a97a3)
#define COLOR_TAG     LS_COLOR_HEX(0x6c7a87)
#define COLOR_ACCENT  LS_COLOR_HEX(0x3d8bfd)
#define COLOR_TRACK   LS_COLOR_HEX(0x2b333c)
#define COLOR_BUTTON  LS_COLOR_HEX(0x2a323b)

/*The arc sweeps 270 degrees starting at the lower left, so the gauge opens
 downward. 0 degrees points right and angles grow clockwise.*/
#define GAUGE_START_DEG 135u
#define GAUGE_SWEEP_DEG 270u

#define LEVEL_STEP 10
#define LEVEL_MAX  100

/**********************
 *  STATIC VARIABLES
 **********************/
static SDL_Renderer * renderer;
static SDL_Texture * texture;

static ls_display_t display_1;

/*Two screens. `screen_1` holds the basic usage of widgets, `screen_2` a header
 and a readout of the same application state. A display shows exactly one
 of them at a time - whichever `display_1.screen_active` points at.*/
static ls_screen_t screen_1;
static ls_screen_t screen_2;

/*The widgets are plain file-scoped structs - the library keeps pointers to these,
 it never copies them and never owns them. The `*_tag` labels are the
 captions naming each widget type on screen.*/
static ls_label_t title;
static ls_label_t title_tag;
static ls_rectangle_t title_sep_line;
static ls_label_t title_sep_line_tag;

static ls_image_t logo;
static ls_label_t logo_tag;

static ls_arc_t gauge_track;
static ls_arc_t gauge_fill;
static ls_label_t gauge_tag;

static ls_rectangle_t value_panel;
static ls_label_t value_panel_tag;
static ls_label_t value_caption;
static ls_label_t value_readout;
static ls_label_t value_readout_tag;

static ls_button_t step_button;
static ls_label_t step_button_text;
static ls_button_t reset_button;
static ls_label_t reset_button_text;
static ls_label_t button_tag;
static ls_label_t button_text_tag;

static ls_image_button_t gauge_switch;
static ls_label_t gauge_switch_text;
static ls_label_t gauge_switch_tag;
static ls_label_t gauge_switch_note;

static ls_label_t screen_tag;

/*Navigation is one image button per screen: forward on `screen_1`, back on
 `screen_2`.*/
static ls_image_button_t nav_next;
static ls_label_t nav_next_tag;

static ls_label_t screen_2_title;
static ls_rectangle_t screen_2_rule;
static ls_label_t screen_2_readout;
static ls_label_t screen_2_readout_tag;
static ls_image_button_t nav_prev;
static ls_label_t nav_prev_tag;

/*'level' is the one piece of application state, which is bound to the gauge label `value_readout`
 which is recomputed at every render cycle. This is all the callbacks touch.*/
static int32_t level = 40;
static bool gauge_visible = true;

static uint32_t indev_x;
static uint32_t indev_y;
static ls_indev_state_t indev_state = LS_INDEV_STATE_RELEASED;
static ls_indev_state_t indev_prev_state = LS_INDEV_STATE_RELEASED;

/*Assets compiled into the binary. Every image and font a widget uses has to be
 declared and linked explicitly - the library installs no defaults of its own.
 To try out your own assets use the supplied conversion scripts in the /scripts 
 directory. To understand how the supplied scripts work, see the relevant 
 sections of README.md*/ //TODO
extern const ls_image_dsc_t img_benchmark_lvgl_logo_argb;
extern const ls_image_dsc_t switch_on;
extern const ls_image_dsc_t switch_off;
extern const ls_image_dsc_t nav_next_normal;
extern const ls_image_dsc_t nav_next_pressed;
extern const ls_image_dsc_t nav_prev_normal;
extern const ls_image_dsc_t nav_prev_pressed;

extern ls_font_t font_dejavu_30;
extern ls_font_t montserrat_12;
extern ls_font_t montserrat_18;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void sdl_init(void);
static void sdl_update(const void * fb);

static int create_screen_1_widgets(void);
static int create_screen_2_widgets(void);
static int create_tag_on(ls_screen_t * screen, ls_label_t * tag, uint32_t x, uint32_t y,
                         lv_label_align_t align, const char * text);
static int create_nav_button(ls_screen_t * screen, ls_image_button_t * button, uint32_t x, uint32_t y,
                             const ls_image_dsc_t * normal, const ls_image_dsc_t * pressed,
                             ls_indev_event_cb_t event_cb);
static void update_gauge(void);

static void step_button_cb(void * widget, ls_indev_event_type_t type);
static void reset_button_cb(void * widget, ls_indev_event_type_t type);
static void gauge_switch_cb(void * widget, ls_indev_event_type_t type);
static void nav_next_cb(void * widget, ls_indev_event_type_t type);
static void nav_prev_cb(void * widget, ls_indev_event_type_t type);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

int main(int argc, char ** argv)
{
    UNUSED(argc);
    UNUSED(argv);

    sdl_init();

    /*The frame buffer belongs to the application, not to the library. Static
     *storage, sized at compile time.*/
    static ls_frame_buffer_color_t fb[WIN_HOR_RES * WIN_VER_RES];

    ls_error_code_t err;

    err = ls_display_init(&display_1);
    if(err != LS_ERROR_CODE_OK) { printf("display init failed: %d\n", err); return 1; }
    display_1.hor_res = WIN_HOR_RES;
    display_1.ver_res = WIN_VER_RES;
    display_1.stride = WIN_HOR_RES * LS_BYTE_PER_PIXEL;
    display_1.frame_buffer = fb;
    display_1.screen_active = &screen_1;

    err = ls_screen_init(&screen_1);
    if(err != LS_ERROR_CODE_OK) { printf("screen 1 init failed: %d\n", err); return 1; }
    screen_1.bg_color = COLOR_BG;

    err = ls_screen_init(&screen_2);
    if(err != LS_ERROR_CODE_OK) { printf("screen 2 init failed: %d\n", err); return 1; }
    screen_2.bg_color = COLOR_BG;

    if(create_screen_1_widgets()) return 1;
    if(create_screen_2_widgets()) return 1;
    update_gauge();

    while(1) {
        /*Works out which widget is under the mouse pointer and calls its event callback
         *if it has one.*/
        err = ls_indev_process(&display_1, indev_x, indev_y, indev_state, indev_prev_state);
        if(err != LS_ERROR_CODE_OK) { printf("indev process failed: %d\n", err); return 1; }
        indev_prev_state = indev_state;

        /*Draw the whole active screen into the frame buffer, then hand the
         *buffer to the panel - here, an SDL texture.*/
        err = ls_render(&display_1);
        if(err != LS_ERROR_CODE_OK) { printf("render failed: %d\n", err); return 1; }
        sdl_update(fb);

        usleep(1000 * 5);

        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    return 0;
                case SDL_MOUSEBUTTONDOWN:
                    if(event.button.button == SDL_BUTTON_LEFT) {
                        indev_state = LS_INDEV_STATE_PRESSED;
                        indev_x = (uint32_t) event.motion.x;
                        indev_y = (uint32_t) event.motion.y;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    if(event.button.button == SDL_BUTTON_LEFT) {
                        indev_state = LS_INDEV_STATE_RELEASED;
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

/*The name tags are all the same kind of label, so they get one helper, with the
 screen they belong to passed in.*/
static int create_tag_on(ls_screen_t * screen, ls_label_t * tag, uint32_t x, uint32_t y,
                         lv_label_align_t align, const char * text)
{
    ls_error_code_t err = ls_label_create(screen, tag);
    if(err != LS_ERROR_CODE_OK) { printf("tag '%s' create failed: %d\n", text, err); return 1; }

    tag->x = x;
    tag->y = y;
    tag->align = align;
    tag->font = &montserrat_12;
    tag->color = COLOR_TAG;
    tag->letter_space = 2;
    tag->text = text;

    return 0;
}

/*An image button draws the source belonging to its current state. If that source
 is NULL the library reports an error instead of quietly drawing nothing.*/
static int create_nav_button(ls_screen_t * screen, ls_image_button_t * button, uint32_t x, uint32_t y,
                             const ls_image_dsc_t * normal, const ls_image_dsc_t * pressed,
                             ls_indev_event_cb_t event_cb)
{
    ls_error_code_t err = ls_image_button_create(screen, button);
    if(err != LS_ERROR_CODE_OK) { printf("nav button create failed: %d\n", err); return 1; }

    button->x = x;
    button->y = y;

    button->src_normal = normal;
    button->src_pressed = pressed;

    /*The event callback which will handle what to do when the button is interacted with*/
    button->common.indev_event_cb = event_cb;

    return 0;
}

/*Widgets are created onto a screen and then configured by writing their fields
 directly - the library has no setters. Creation order is render order, so a
 widget created later draws on top of one created earlier.*/
static int create_screen_1_widgets(void)
{
    ls_error_code_t err;

    /* ------------------------------- HEADER ------------------------------- */

    err = ls_label_create(&screen_1, &title);
    if(err != LS_ERROR_CODE_OK) { printf("title create failed: %d\n", err); return 1; }
    title.x = 30;
    title.y = 26;
    title.font = &montserrat_18;
    title.color = COLOR_TEXT;
    title.letter_space = 2;
    title.text = "LVGL SAFE - BASIC EXAMPLE";

    if(create_tag_on(&screen_1, &title_tag, 32, 56, LS_LABEL_ALIGN_LEFT, "LS_LABEL")) return 1;

    /*A 1 px separotor line - is just a very thin rectangle.*/
    err = ls_rectangle_create(&screen_1, &title_sep_line);
    if(err != LS_ERROR_CODE_OK) { printf("title_sep_line create failed: %d\n", err); return 1; }
    title_sep_line.x = 30;
    title_sep_line.y = 84;
    title_sep_line.width = 740;
    title_sep_line.height = 1;
    title_sep_line.bg_color = COLOR_TRACK;

    if(create_tag_on(&screen_1, &title_sep_line_tag, 32, 90, LS_LABEL_ALIGN_LEFT, "LS_RECTANGLE")) return 1;

    /* -------------------------------- IMAGE ------------------------------- */

    /*This source is ARGB8888, so every pixel carries its own alpha and the image
     is blended over whatever sits beneath it - no separate mask needed.
     Here we added the LVGL logo for demonstration.*/
    err = ls_image_create(&screen_1, &logo);
    if(err != LS_ERROR_CODE_OK) { printf("logo create failed: %d\n", err); return 1; }
    logo.x = 30;
    logo.y = 150;
    logo.src = &img_benchmark_lvgl_logo_argb;

    if(create_tag_on(&screen_1, &logo_tag, 80, 128, LS_LABEL_ALIGN_CENTER, "LS_IMAGE")) return 1;

    /* -------------------------------- GAUGE ------------------------------- */

    /*A gauge is two arcs: a full-sweep grey track, and a shorter coloured arc on
     top of it whose `end_deg` is recomputed from 'level'.*/
    err = ls_arc_create(&screen_1, &gauge_track);
    if(err != LS_ERROR_CODE_OK) { printf("gauge_track create failed: %d\n", err); return 1; }
    gauge_track.center_x = 300;
    gauge_track.center_y = 212;
    gauge_track.radius = 58;
    gauge_track.thickness = 13;
    gauge_track.start_deg = GAUGE_START_DEG;
    gauge_track.end_deg = GAUGE_START_DEG + GAUGE_SWEEP_DEG;
    gauge_track.color = COLOR_TRACK;

    err = ls_arc_create(&screen_1, &gauge_fill);
    if(err != LS_ERROR_CODE_OK) { printf("gauge_fill create failed: %d\n", err); return 1; }
    gauge_fill.center_x = 300;
    gauge_fill.center_y = 212;
    gauge_fill.radius = 58;
    gauge_fill.thickness = 13;
    gauge_fill.start_deg = GAUGE_START_DEG;
    gauge_fill.color = COLOR_ACCENT;

    if(create_tag_on(&screen_1, &gauge_tag, 300, 128, LS_LABEL_ALIGN_CENTER, "LS_ARC")) return 1;

    /* ---------------------------- VALUE READOUT --------------------------- */

    err = ls_rectangle_create(&screen_1, &value_panel);
    if(err != LS_ERROR_CODE_OK) { printf("value_panel create failed: %d\n", err); return 1; }
    value_panel.x = 450;
    value_panel.y = 150;
    value_panel.width = 300;
    value_panel.height = 110;
    value_panel.bg_color = COLOR_PANEL;

    if(create_tag_on(&screen_1, &value_panel_tag, 450, 128, LS_LABEL_ALIGN_LEFT, "LS_RECTANGLE")) return 1;

    err = ls_label_create(&screen_1, &value_caption);
    if(err != LS_ERROR_CODE_OK) { printf("value_caption create failed: %d\n", err); return 1; }
    value_caption.x = 472;
    value_caption.y = 166;
    value_caption.font = &montserrat_18;
    value_caption.color = COLOR_DIM;
    value_caption.text = "LEVEL";

    /*`bind_int` points the label at an int32_t and formats it with `bind_fmt` on
     every render, so nothing has to push the new value into the widget when
     `level` changes. `text` is left NULL, which is what makes the binding take precedence.*/
    err = ls_label_create(&screen_1, &value_readout);
    if(err != LS_ERROR_CODE_OK) { printf("value_readout create failed: %d\n", err); return 1; }
    value_readout.x = 472;
    value_readout.y = 196;
    value_readout.font = &font_dejavu_30;
    value_readout.color = COLOR_ACCENT;
    value_readout.bind_int = &level;
    value_readout.bind_fmt = "%d %%";

    if(create_tag_on(&screen_1, &value_readout_tag, 452, 268, LS_LABEL_ALIGN_LEFT, "LS_LABEL, BIND_INT")) return 1;

    /* ------------------------------- BUTTONS ------------------------------ */

    /*A button draws only its background - it has no text of its own, so the
     caption is a separate label created afterwards, which puts it on top.
     Only the normal and pressed colours are set here; the focused, checked and
     disabled colours keep the defaults `ls_button_create` gave them.*/
    err = ls_button_create(&screen_1, &step_button);
    if(err != LS_ERROR_CODE_OK) { printf("step_button create failed: %d\n", err); return 1; }
    step_button.x = 30;
    step_button.y = 342;
    step_button.width = 170;
    step_button.height = 54;
    step_button.bg_color_normal = COLOR_BUTTON;
    step_button.bg_color_pressed = COLOR_ACCENT;
    step_button.common.indev_event_cb = step_button_cb;

    if(create_tag_on(&screen_1, &button_tag, 30, 318, LS_LABEL_ALIGN_LEFT, "LS_BUTTON")) return 1;

    err = ls_label_create(&screen_1, &step_button_text);
    if(err != LS_ERROR_CODE_OK) { printf("step_button_text create failed: %d\n", err); return 1; }
    step_button_text.x = 115;
    step_button_text.y = 357;
    step_button_text.align = LS_LABEL_ALIGN_CENTER;
    step_button_text.font = &montserrat_18;
    step_button_text.color = COLOR_TEXT;
    step_button_text.text = "STEP +10";

    if(create_tag_on(&screen_1, &button_text_tag, 30, 406, LS_LABEL_ALIGN_LEFT, "LS_LABEL ON TOP OF IT")) return 1;

    err = ls_button_create(&screen_1, &reset_button);
    if(err != LS_ERROR_CODE_OK) { printf("reset_button create failed: %d\n", err); return 1; }
    reset_button.x = 220;
    reset_button.y = 342;
    reset_button.width = 170;
    reset_button.height = 54;
    reset_button.bg_color_normal = COLOR_BUTTON;
    reset_button.bg_color_pressed = COLOR_ACCENT;
    reset_button.common.indev_event_cb = reset_button_cb;

    err = ls_label_create(&screen_1, &reset_button_text);
    if(err != LS_ERROR_CODE_OK) { printf("reset_button_text create failed: %d\n", err); return 1; }
    reset_button_text.x = 305;
    reset_button_text.y = 357;
    reset_button_text.align = LS_LABEL_ALIGN_CENTER;
    reset_button_text.font = &montserrat_18;
    reset_button_text.color = COLOR_TEXT;
    reset_button_text.text = "RESET";

    /* -------------------------------- SWITCH ------------------------------ */

    if(create_tag_on(&screen_1, &gauge_switch_tag, 450, 318, LS_LABEL_ALIGN_LEFT, "LS_IMAGE_BUTTON")) return 1;

    /*Mirrors the state model of `ls_button`, except each state names an image
     instead of a colour. This switch is never focused or disabled, so only the
     four states it can actually reach are filled in. Leaving a source NULL is
     safe until the widget enters that state: rather than drawing nothing, a
     missing source is reported as LS_ERROR_CODE_INVALID_CONFIGURATION.*/
    err = ls_image_button_create(&screen_1, &gauge_switch);
    if(err != LS_ERROR_CODE_OK) { printf("gauge_switch create failed: %d\n", err); return 1; }
    gauge_switch.x = 450;
    gauge_switch.y = 342;
    gauge_switch.src_normal = &switch_off;
    gauge_switch.src_pressed = &switch_off;
    gauge_switch.src_checked = &switch_on;
    gauge_switch.src_checked_pressed = &switch_on;
    gauge_switch.checked = gauge_visible;
    gauge_switch.common.indev_event_cb = gauge_switch_cb;

    err = ls_label_create(&screen_1, &gauge_switch_text);
    if(err != LS_ERROR_CODE_OK) { printf("gauge_switch_text create failed: %d\n", err); return 1; }
    gauge_switch_text.x = 536;
    gauge_switch_text.y = 350;
    gauge_switch_text.font = &montserrat_18;
    gauge_switch_text.color = COLOR_TEXT;
    gauge_switch_text.text = "SHOW GAUGE";

    if(create_tag_on(&screen_1, &gauge_switch_note, 450, 388, LS_LABEL_ALIGN_LEFT,
                     "CHECKED SWAPS THE SOURCE IMAGE")) return 1;

    /* ------------------------------- SCREEN ------------------------------- */

    /*Nothing is created here, because the background is not a widget: it is the
     screen's own `bg_color`, set in `main` and filled in before any widget
     is drawn on top. The tag below only points it out.*/
    if(create_tag_on(&screen_1, &screen_tag, 20, 450, LS_LABEL_ALIGN_LEFT, "LS_SCREEN BACKGROUND")) return 1;

    /* ----------------------------- NAVIGATION ----------------------------- */

    /*The only way off this screen. Clicking it points the display at
     `screen_2`.*/
    if(create_nav_button(&screen_1, &nav_next, 718, 418,
                         &nav_next_normal, &nav_next_pressed, nav_next_cb)) return 1;

    if(create_tag_on(&screen_1, &nav_next_tag, 706, 436, LS_LABEL_ALIGN_RIGHT, "LS_IMAGE_BUTTON, NEXT SCREEN")) return 1;

    return 0;
}

/*`screen_2` Widgets belong to exactly one screen, so even the header is a second set of structs rather than
 the ones `screen_1` already uses.*/
static int create_screen_2_widgets(void)
{
    ls_error_code_t err;

    /* ------------------------------- HEADER ------------------------------- */

    err = ls_label_create(&screen_2, &screen_2_title);
    if(err != LS_ERROR_CODE_OK) { printf("screen_2_title create failed: %d\n", err); return 1; }
    screen_2_title.x = 30;
    screen_2_title.y = 26;
    screen_2_title.font = &montserrat_18;
    screen_2_title.color = COLOR_TEXT;
    screen_2_title.letter_space = 2;
    screen_2_title.text = "LVGL SAFE - SECOND SCREEN";

    err = ls_rectangle_create(&screen_2, &screen_2_rule);
    if(err != LS_ERROR_CODE_OK) { printf("screen_2_rule create failed: %d\n", err); return 1; }
    screen_2_rule.x = 30;
    screen_2_rule.y = 84;
    screen_2_rule.width = 740;
    screen_2_rule.height = 1;
    screen_2_rule.bg_color = COLOR_TRACK;

    /* ---------------------------- SHARED STATE ---------------------------- */

    /*The same `level` the buttons on `screen_1` drive. State is not owned by a
     screen: this label reads the variable at every render, including while
     `screen_2` is not the active one, so coming back here shows the value the
     buttons left behind.*/
    err = ls_label_create(&screen_2, &screen_2_readout);
    if(err != LS_ERROR_CODE_OK) { printf("screen_2_readout create failed: %d\n", err); return 1; }
    screen_2_readout.x = WIN_HOR_RES / 2;
    screen_2_readout.y = 220;
    screen_2_readout.align = LS_LABEL_ALIGN_CENTER;
    screen_2_readout.font = &font_dejavu_30;
    screen_2_readout.color = COLOR_ACCENT;
    screen_2_readout.bind_int = &level;
    screen_2_readout.bind_fmt = "LEVEL IS STILL %d %%";

    if(create_tag_on(&screen_2, &screen_2_readout_tag, WIN_HOR_RES / 2, 262, LS_LABEL_ALIGN_CENTER,
                     "LS_LABEL, BIND_INT - SAME level AS SCREEN 1")) return 1;

    /* ----------------------------- NAVIGATION ----------------------------- */

    /*The mirror of `nav_next`: it hands the display back to `screen_1`. This is
     the last screen, so it gets no forward button.*/
    if(create_nav_button(&screen_2, &nav_prev, 30, 418,
                         &nav_prev_normal, &nav_prev_pressed, nav_prev_cb)) return 1;

    if(create_tag_on(&screen_2, &nav_prev_tag, 94, 436, LS_LABEL_ALIGN_LEFT,
                     "LS_IMAGE_BUTTON, PREVIOUS SCREEN")) return 1;

    return 0;
}

/*Turn `level` into an arc sweep. Called at start-up and from the button
 callbacks - the readout label needs no equivalent, it is bound to `level`.*/
static void update_gauge(void)
{
    gauge_fill.end_deg = GAUGE_START_DEG + ((GAUGE_SWEEP_DEG * (uint32_t) level) / (uint32_t) LEVEL_MAX);

    /*Widgets are never deleted in LVGL Safe; `hidden` is how one is "taken off"
     the screen.*/
    gauge_track.common.hidden = !gauge_visible;
    gauge_fill.common.hidden = (!gauge_visible) || (level <= 0);
}

/*An `ls_indev_event_cb_t` example. It is called with the widget that was hit and the
 event type: PRESSED on the way down, PRESSING while held, CLICKED on release.*/
static void step_button_cb(void * widget, ls_indev_event_type_t type)
{
    UNUSED(widget);

    if(type == LS_INDEV_EVENT_TYPE_CLICKED) {
        level += LEVEL_STEP;
        if(level > LEVEL_MAX) level = 0;
        update_gauge();
    }
}

static void reset_button_cb(void * widget, ls_indev_event_type_t type)
{
    UNUSED(widget);

    if(type == LS_INDEV_EVENT_TYPE_CLICKED) {
        level = 0;
        update_gauge();
    }
}

/*`checked` is a plain bool the application owns - the widget does not latch
 itself, so flipping it here is what makes this a toggle.*/
static void gauge_switch_cb(void * widget, ls_indev_event_type_t type)
{
    ls_image_button_t * button = widget;
    if(button == NULL) return;

    if(type == LS_INDEV_EVENT_TYPE_CLICKED) {
        button->checked = !button->checked;
        gauge_visible = button->checked;
        update_gauge();
    }
}

/*Switching screens is a single assignment: the display renders whatever
 `screen_active` points at, and every widget stays exactly as it was. Neither
 callback does any arithmetic on a screen index, so navigation cannot run off
 either end or wrap around to the other side.*/
static void nav_next_cb(void * widget, ls_indev_event_type_t type)
{
    UNUSED(widget);

    if(type == LS_INDEV_EVENT_TYPE_CLICKED) {
        display_1.screen_active = &screen_2;
    }
}

static void nav_prev_cb(void * widget, ls_indev_event_type_t type)
{
    UNUSED(widget);

    if(type == LS_INDEV_EVENT_TYPE_CLICKED) {
        display_1.screen_active = &screen_1;
    }
}

/* ------------------------------- SDL BACKEND ------------------------------ */

static void sdl_init(void)
{
    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
    }

    SDL_Window * window = SDL_CreateWindow(
                              "LVGL Safe - Basic Example",
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
