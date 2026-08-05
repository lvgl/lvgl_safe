/**
 * @file translations.c
 *
 * The whole text of the API tour, in English, German and French.
 *
 * One row per string: a tag, then the three languages in the order the footer
 * button cycles through them. `ls_translation_set_language` rewrites the
 * `current` pointer inside every row, so a label that was bound with tr_bind()
 * shows the new language on the very next render - the application pushes
 * nothing.
 *
 * Text is byte-indexed into the font, so every character above U+007F is written
 * as an octal escape of its Latin-1 code point. The four Montserrat faces in
 * fonts/ all cover 32..255, and these are the escapes this file uses:
 *
 *   \300 A-grave        \310 E-grave        \324 O-circumflex   \342 a-circumflex
 *   \304 A-umlaut       \311 E-acute        \326 O-umlaut       \344 a-umlaut
 *   \307 C-cedilla      \312 E-circumflex   \331 U-grave        \350 e-grave
 *   \316 I-circumflex   \334 U-umlaut       \337 sharp-s        \351 e-acute
 *   \340 a-grave        \352 e-circumflex   \356 i-circumflex   \364 o-circumflex
 *   \366 o-umlaut       \371 u-grave        \374 u-umlaut
 *
 * Language-invariant strings - the product name, an API identifier used as a
 * caption - go through TR_ALL rather than being left in main.c: the binding
 * code then has one shape everywhere, and there is exactly one file to open
 * when a string has to change.
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>

#include "translations.h"

#include <ls_error_codes.h>

/**********************
 *      MACROS
 **********************/

/*One row of the table. The compound literal has static storage duration at
 *file scope, so the library may keep the pointer for the whole run.*/
#define TR(tag_name, en, de, fr) \
    { .tag = (tag_name), .translations = (const char *[TR_LANGUAGE_CNT]){ (en), (de), (fr) } }

/*Spelled the same in every language: a brand, an API name, a number.*/
#define TR_ALL(tag_name, str) TR((tag_name), (str), (str), (str))

/**********************
 *  STATIC VARIABLES
 **********************/

/*Not const: `ls_translation_init` and `ls_translation_set_language` write the
 *`current` pointer of every row.*/
static ls_translation_t translation_table[] = {

    /* ---------------------------- CHROME --------------------------------- */

    TR_ALL("product", "LVGL SAFE"),
    TR("tagline", "DETERMINISTIC UI FOR SAFETY-CRITICAL SYSTEMS",
                  "DETERMINISTISCHE UI F\334R SICHERHEITSKRITISCHE SYSTEME",
                  "IHM D\311TERMINISTE POUR SYST\310MES CRITIQUES"),
    TR("panel_caption", "WHAT THIS SHOWS",
                        "WAS SIE HIER SEHEN",
                        "CE QUE VOUS VOYEZ"),

    /* ---------------------------- FOOTER --------------------------------- */

    TR("language_name", "ENGLISH", "DEUTSCH", "FRAN\307AIS"),
    TR_ALL("footer_lang_caption", "LS_TRANSLATION_T"),
    TR("footer_hint", "CLICK THROUGH - IT WRAPS AROUND",
                      "DURCHKLICKEN - ES L\304UFT UM",
                      "CLIQUEZ - LA VISITE BOUCLE"),
    TR("footer_note", "NO MALLOC - NO WIDGET DELETION",
                      "KEIN MALLOC - KEIN WIDGET-L\326SCHEN",
                      "PAS DE MALLOC NI SUPPRESSION DE WIDGET"),

    /* ========================== 01 - OVERVIEW ============================ */

    TR("title_01", "OVERVIEW", "\334BERBLICK", "APER\307U"),
    TR("sub_01", "THE LIBRARY IN ONE SCREEN",
                 "DIE BIBLIOTHEK AUF EINEN BLICK",
                 "LA BIBLIOTH\310QUE EN UN \311CRAN"),

    TR("s01_l00", "A from-scratch C UI library",
                  "Eine C-Bibliothek von Grund auf,",
                  "Une biblioth\350que C d'IHM"),
    TR("s01_l01", "for safety-critical applications.",
                  "f\374r sicherheitskritische Anwendungen.",
                  "pour applications critiques."),
    TR("s01_l02", "No runtime allocation.",
                  "Keine Allokation zur Laufzeit.",
                  "Aucune allocation \340 l'ex\351cution."),
    TR("s01_l03", "No library-owned state.",
                  "Kein bibliothekseigener Zustand.",
                  "Aucun \351tat interne \340 la biblioth\350que."),
    TR("s01_l04", "No widget deletion, ever.",
                  "Niemals L\366schen von Widgets.",
                  "Jamais de suppression de widget."),
    TR("s01_l05", "Every call into the API returns an error.",
                  "Jeder API-Aufruf liefert einen Fehler.",
                  "Chaque appel d'API renvoie une erreur."),
    TR("s01_l06", "The next ten screens show",
                  "Die n\344chsten zehn Schirme zeigen",
                  "Les dix \351crans suivants montrent"),
    TR("s01_l07", "every widget and every hook",
                  "jedes Widget und jeden Hook,",
                  "chaque widget et chaque hook"),
    TR("s01_l08", "that exists in v0.1.0.",
                  "den es in v0.1.0 gibt.",
                  "qui existe en v0.1.0."),

    TR("ov_pillar_alloc", "RUNTIME ALLOCATIONS",
                          "ALLOKATIONEN ZUR LAUFZEIT",
                          "ALLOCATIONS \300 L'EX\311CUTION"),
    TR("ov_pillar_widgets", "WIDGET TYPES", "WIDGET-TYPEN", "TYPES DE WIDGETS"),
    TR("ov_pillar_hooks", "EXTENSION POINTS", "ERWEITERUNGSPUNKTE", "POINTS D'EXTENSION"),
    TR("ov_pillar_buffer", "FRAME BUFFER, YOURS",
                           "FRAME BUFFER, IHRER",
                           "FRAME BUFFER, LE V\324TRE"),

    /* ========================= 02 - LS_RECTANGLE ========================= */

    TR_ALL("title_02", "LS_RECTANGLE"),
    TR("sub_02", "FILLS, OPACITY AND COMPOSED FRAMES",
                 "F\334LLUNGEN, OPAZIT\304T UND RAHMEN",
                 "REMPLISSAGES, OPACIT\311 ET CADRES"),

    TR("s02_l00", "The primitive, everything",
                  "Das Primitiv, aus dem alles",
                  "La primitive dont tout le"),
    TR("s02_l01", "else is built from: x, y,",
                  "andere gebaut wird: x, y,",
                  "reste est fait : x, y,"),
    TR("s02_l02", "width, height, bg_color",
                  "width, height, bg_color",
                  "width, height, bg_color"),
    TR("s02_l03", "and bg_opa.", "und bg_opa.", "et bg_opa."),
    TR("s02_l04", "bg_opa blends against what",
                  "bg_opa mischt mit dem, was",
                  "bg_opa se m\352le \340 ce qui est"),
    TR("s02_l05", "is already in the buffer.",
                  "schon im Puffer steht.",
                  "d\351j\340 dans le tampon."),

    TR_ALL("rect_ladder_caption", "BG_OPA  20 / 40 / 60 / 80 / 100 %"),
    TR("rect_frame_caption", "COMPOSED 1 PX FRAME = 2 FILLS",
                             "1 PX RAHMEN = 2 FL\304CHEN",
                             "CADRE 1 PX COMPOS\311 = 2 REMPLISSAGES"),
    TR("rect_frame_text", "NESTED FILLS", "FL\304CHE IN FL\304CHE", "REMPLISSAGES IMBRIQU\311S"),
    TR("rect_order_caption", "RENDER ORDER = CREATION ORDER",
                             "ZEICHENFOLGE = ERZEUGUNGSFOLGE",
                             "ORDRE DE RENDU = ORDRE DE CR\311ATION"),
    TR("rect_order_note", "CREATED LATER = ON TOP",
                          "SP\304TER ERZEUGT = OBEN",
                          "CR\311\311 PLUS TARD = AU-DESSUS"),

    /* =========================== 03 - LS_LABEL =========================== */

    TR_ALL("title_03", "LS_LABEL"),
    TR("sub_03", "THREE TEXT SOURCES, THREE ALIGNMENTS",
                 "DREI TEXTQUELLEN, DREI AUSRICHTUNGEN",
                 "TROIS SOURCES DE TEXTE, TROIS ALIGNEMENTS"),

    TR("s03_l00", "Text comes from one of",
                  "Text kommt aus einer von",
                  "Le texte vient d'une de"),
    TR("s03_l01", "three sources, in order:",
                  "drei Quellen, in dieser Folge:",
                  "trois sources, dans cet ordre :"),
    /*The 1 / 2 / 3 of this list are numbers, not text: they are labels of their
     *own in main.c so that every row's text starts at the same x.*/
    TR("s03_l02", "text - a plain pointer",
                  "text - ein einfacher Zeiger",
                  "text - un simple pointeur"),
    TR("s03_l03", "bind_int - int32 + fmt",
                  "bind_int - int32 + fmt",
                  "bind_int - int32 + fmt"),
    TR("s03_l04", "translation - a tag",
                  "translation - ein Tag",
                  "translation - un tag"),
    TR("s03_l05", "Alignment resolves against",
                  "Die Ausrichtung rechnet gegen",
                  "L'alignement se calcule sur"),
    TR("s03_l06", "the 'width' of the label",
                  "die 'width' des Labels,",
                  "la 'width' du label :"),
    TR("s03_l07", "so a centred label",
                  "ein zentriertes Label braucht",
                  "un label centr\351 a donc besoin"),
    TR("s03_l08", "needs a box to centre in.",
                  "also eine Box zum Zentrieren.",
                  "d'une bo\356te o\371 se centrer."),
    TR("s03_l09", "Glyphs are A8 bitmaps.",
                  "Glyphen sind A8-Bitmaps.",
                  "Les glyphes sont des A8."),

    TR("lbl_align_caption", "ALIGN, AGAINST A 456 PX BOX",
                            "ALIGN, GEGEN EINE 456 PX BOX",
                            "ALIGN, DANS UNE BO\316TE DE 456 PX"),
    TR("lbl_align_left", "ALIGN LEFT", "ALIGN LINKS", "ALIGN GAUCHE"),
    TR("lbl_align_center", "ALIGN CENTER", "ALIGN ZENTRIERT", "ALIGN CENTR\311"),
    TR("lbl_align_right", "ALIGN RIGHT", "ALIGN RECHTS", "ALIGN DROITE"),
    TR_ALL("lbl_style_caption", "LETTER_SPACE 4 + UNDERLINE_THICKNESS 2"),
    TR("lbl_style_demo", "UNDERLINED", "UNTERSTRICHEN", "SOULIGN\311"),
    TR("lbl_bind_caption", "BIND_INT - LIVE, NO PUSH CODE",
                           "BIND_INT - LIVE, OHNE PUSH-CODE",
                           "BIND_INT - LIVE, SANS CODE PUSH"),
    /*A printf format, not a finished string: the %d has to survive every
     *translation, and main.c re-reads it after each language switch.*/
    TR("lbl_bind_fmt", "FRAMES: %d", "BILDER: %d", "IMAGES : %d"),
    TR("lbl_translation_caption", "TRANSLATION - FOLLOWS THE FOOTER BUTTON",
                                  "TRANSLATION - FOLGT DEM FUSSZEILEN-KNOPF",
                                  "TRANSLATION - SUIT LE BOUTON DU PIED"),
    TR("lbl_translation_demo", "THIS LABEL FOLLOWS THE LANGUAGE",
                               "DIESES LABEL FOLGT DER SPRACHE",
                               "CE LABEL SUIT LA LANGUE"),

    /* ============================ 04 - LS_ARC ============================ */

    TR_ALL("title_04", "LS_ARC"),
    TR("sub_04", "ANGLES, TRACKS AND COMPOSED GAUGES",
                 "WINKEL, BAHNEN UND GEBAUTE ANZEIGEN",
                 "ANGLES, PISTES ET JAUGES COMPOS\311ES"),

    TR("s04_l00", "A centre, a radius, a",
                  "Ein Mittelpunkt, ein Radius,",
                  "Un centre, un rayon, une"),
    TR("s04_l01", "thickness and an angle",
                  "eine Dicke und ein",
                  "\351paisseur et une plage"),
    TR("s04_l02", "range. That is the whole",
                  "Winkelbereich. Das ist das",
                  "d'angles. C'est tout le"),
    TR("s04_l03", "widget - no knob logic,",
                  "ganze Widget - keine Knopf-",
                  "widget - aucune logique de"),
    TR("s04_l04", "no animation, no state.",
                  "Logik, keine Animation, kein Zustand.",
                  "bouton, aucune animation, aucun \351tat."),
    TR("s04_l05", "0 deg points right and",
                  "0 Grad zeigt nach rechts,",
                  "0 degr\351 pointe \340 droite et"),
    TR("s04_l06", "angles grow clockwise.",
                  "Winkel wachsen im Uhrzeigersinn.",
                  "les angles vont dans le sens"),
    TR("s04_l07", "This demo uses 135 deg",
                  "Diese Demo nutzt 135 Grad",
                  "horaire. Cette d\351mo prend 135"),
    TR("s04_l08", "plus a 270 deg sweep.",
                  "plus 270 Grad Bogen.",
                  "degr\351s plus 270 de balayage."),

    TR("arc_legend_track", "TRACK ARC", "BAHN-ARC", "ARC DE PISTE"),
    TR("arc_legend_value", "VALUE ARC", "WERT-ARC", "ARC DE VALEUR"),
    TR("arc_legend_limit", "LIMIT MARKER", "GRENZMARKE", "REP\310RE LIMITE"),
    TR("unit_percent", "PERCENT", "PROZENT", "POUR CENT"),
    TR("arc_caption", "THREE ARCS + TWO LABELS, COMPOSED BY YOU",
                      "DREI ARCS + ZWEI LABELS, VON IHNEN GEBAUT",
                      "TROIS ARCS + DEUX LABELS, COMPOS\311S PAR VOUS"),

    /* ========================== 05 - LS_BUTTON =========================== */

    TR_ALL("title_05", "LS_BUTTON"),
    TR("sub_05", "TEN STATES, ZERO ARRAYS",
                 "ZEHN ZUST\304NDE, KEINE ARRAYS",
                 "DIX \311TATS, AUCUN TABLEAU"),

    TR("s05_l00", "A button is a background,",
                  "Ein Button ist ein Hintergrund,",
                  "Un bouton est un fond, pas"),
    TR("s05_l01", "not a control: no text and",
                  "kein Steuerelement: kein Text,",
                  "un contr\364le : aucun texte,"),
    TR("s05_l02", "no callback plumbing.",
                  "keine Callback-Verdrahtung.",
                  "aucun c\342blage de callback."),
    TR("s05_l03", "Ten colour and ten opacity",
                  "Zehn Farb- und zehn Opazit\344ts-",
                  "Dix champs de couleur et dix"),
    TR("s05_l04", "fields, one pair per state.",
                  "Felder, ein Paar pro Zustand.",
                  "d'opacit\351, une paire par \351tat."),
    TR("s05_l05", "No arrays, so a state",
                  "Keine Arrays, ein Zustand kann",
                  "Aucun tableau : un \351tat ne"),
    TR("s05_l06", "cannot be mis-indexed.",
                  "also nicht falsch indiziert werden.",
                  "peut pas \352tre mal index\351."),
    TR("s05_l07", "Checked and disabled are",
                  "checked und disabled sind",
                  "checked et disabled sont de"),
    TR("s05_l08", "plain bools you write.",
                  "einfache bools, die Sie setzen.",
                  "simples bool\351ens que vous \351crivez."),
    TR("s05_l09", "Put a label on top for text.",
                  "F\374r Text ein Label darauflegen.",
                  "Pour du texte, posez un label."),

    TR("btn_press", "PRESS", "DR\334CKEN", "APPUYER"),
    TR("btn_toggle", "TOGGLE", "UMSCHALTEN", "BASCULER"),
    TR("btn_locked", "LOCKED", "GESPERRT", "VERROUILL\311"),
    TR("btn_focus", "FOCUS", "FOKUS", "FOCUS"),
    TR("btn_cap_press", "NORMAL / PRESSED", "NORMAL / GEDR\334CKT", "NORMAL / APPUY\311"),
    TR("btn_cap_toggle", "CHECKED - CLICK IT", "CHECKED - ANKLICKEN", "CHECKED - CLIQUEZ"),
    TR_ALL("btn_cap_locked", "DISABLED"),
    TR("btn_cap_focus", "FOCUSED - UP/DOWN KEY",
                        "FOCUSED - AUF/AB-TASTE",
                        "FOCUSED - TOUCHES HAUT/BAS"),
    TR("btn_note", "FOCUS IS ONE INDEX PER SCREEN - UP/DOWN",
                   "FOKUS IST EIN INDEX PRO SCHIRM - AUF/AB",
                   "LE FOCUS EST UN INDEX PAR \311CRAN - HAUT/BAS"),

    /* ======================= 06 - LS_IMAGE_BUTTON ======================== */

    TR_ALL("title_06", "LS_IMAGE_BUTTON"),
    TR("sub_06", "ONE SOURCE IMAGE PER STATE",
                 "EIN QUELLBILD PRO ZUSTAND",
                 "UNE IMAGE SOURCE PAR \311TAT"),

    TR("s06_l00", "The same state machine as",
                  "Dieselbe Zustandsmaschine wie",
                  "La m\352me machine \340 \351tats que"),
    TR("s06_l01", "ls_button, but each state",
                  "ls_button, nur nennt jeder",
                  "ls_button, mais chaque \351tat"),
    TR("s06_l02", "names an image instead of",
                  "Zustand ein Bild statt",
                  "nomme une image au lieu"),
    TR("s06_l03", "a colour.", "einer Farbe.", "d'une couleur."),
    TR("s06_l04", "A NULL source for the",
                  "Eine NULL-Quelle f\374r den",
                  "Une source NULL pour l'\351tat"),
    TR("s06_l05", "current state is reported",
                  "aktuellen Zustand wird als",
                  "courant est signal\351e comme"),
    TR("s06_l06", "as a configuration error.",
                  "Konfigurationsfehler gemeldet.",
                  "erreur de configuration."),
    TR("s06_l07", "The hit area is the source",
                  "Die Trefferfl\344che ist die",
                  "La zone active est la taille"),
    TR("s06_l08", "image's own size.",
                  "Gr\366\337e des Quellbildes.",
                  "de l'image source."),

    TR_ALL("ibtn_cap_checked", "CHECKED"),
    TR_ALL("ibtn_cap_pressed", "PRESSED"),
    TR_ALL("ibtn_cap_disabled", "DISABLED"),
    TR("ibtn_sub_checked", "SRC_CHECKED VS SRC_NORMAL",
                           "SRC_CHECKED STATT SRC_NORMAL",
                           "SRC_CHECKED AU LIEU DE SRC_NORMAL"),
    /*Says where to click and where the result shows up. The icon is the only
     *clickable part of this row - the caption beside it is a plain label - and
     *the effect lands on the next screen, so both facts have to be on screen or
     *a reader clicks the wrong pixels and then sees nothing.*/
    TR("ibtn_sub_pressed", "SRC_PRESSED WHILE HELD - CLICK IT TO TINT SLIDE 07",
                           "SRC_PRESSED SOLANGE GEDR\334CKT - F\304RBT SCHIRM 07",
                           "SRC_PRESSED PENDANT L'APPUI - TEINTE L'\311CRAN 07"),
    TR("ibtn_sub_disabled", "IGNORES INPUT ENTIRELY",
                            "IGNORIERT JEDE EINGABE",
                            "IGNORE TOUTE ENTR\311E"),
    TR("ibtn_note", "THE FOOTER ARROWS ARE IMAGE BUTTONS TOO",
                    "DIE PFEILE IN DER FUSSZEILE SIND AUCH IMAGE BUTTONS",
                    "LES FL\310CHES DU PIED SONT AUSSI DES IMAGE BUTTONS"),

    /* =========================== 07 - LS_IMAGE =========================== */

    TR_ALL("title_07", "LS_IMAGE"),
    TR("sub_07", "ARGB8888, A8 TINTING AND ROTATION",
                 "ARGB8888, A8-EINF\304RBUNG UND ROTATION",
                 "ARGB8888, TEINTE A8 ET ROTATION"),

    TR("s07_l00", "Three source formats:",
                  "Drei Quellformate:",
                  "Trois formats source :"),
    TR("s07_l01", "ARGB8888, RGB565 and A8.",
                  "ARGB8888, RGB565 und A8.",
                  "ARGB8888, RGB565 et A8."),
    TR("s07_l02", "A8 is a mask, not a picture:",
                  "A8 ist eine Maske, kein Bild:",
                  "A8 est un masque, pas une image :"),
    TR("s07_l03", "one alpha byte per pixel.",
                  "ein Alpha-Byte pro Pixel.",
                  "un octet alpha par pixel."),
    TR("s07_l04", "The colour is 'a8_color', so",
                  "Die Farbe kommt aus 'a8_color' -",
                  "La couleur vient de 'a8_color' :"),
    TR("s07_l05", "one bitmap fits every state.",
                  "eine Bitmap f\374r jeden Zustand.",
                  "une bitmap pour chaque \351tat."),
    TR("s07_l06", "Rotation is in whole",
                  "Rotation erfolgt in ganzen",
                  "La rotation est en degr\351s"),
    TR("s07_l07", "degrees around 'pivot_x'",
                  "Grad um 'pivot_x' und 'pivot_y',",
                  "entiers autour de 'pivot_x' et"),
    TR("s07_l08", "and 'pivot_y', and 'opa'",
                  "und 'opa' multipliziert das",
                  "'pivot_y', et 'opa' multiplie"),
    TR("s07_l09", "multiplies source alpha.",
                  "Alpha der Quelle.",
                  "l'alpha de la source."),

    TR_ALL("img_logo_caption", "ARGB8888 + ROTATION + OPA"),
    /*Names the source of the colour, so a reader who arrives here and wonders why
     *the triangle is amber rather than red knows which screen to go back to.*/
    TR("img_glyph_caption", "A8 + A8_COLOR - SET ON SLIDE 06",
                            "A8 + A8_COLOR - VON SCHIRM 06",
                            "A8 + A8_COLOR - DEPUIS L'\311CRAN 06"),
    TR("img_tints_caption", "SAME A8 BITMAP, THREE TINTS - NO EXTRA ASSET",
                            "GLEICHE A8-BITMAP, DREI T\326NE - KEIN ZUSATZ-ASSET",
                            "M\312ME BITMAP A8, TROIS TEINTES - AUCUN ASSET DE PLUS"),

    /* ==================== 08 - COMMON.INDEV_EVENT_CB ===================== */

    TR_ALL("title_08", "COMMON.INDEV_EVENT_CB"),
    TR("sub_08", "HOOK 1 OF 4 - INPUT EVENTS",
                 "HOOK 1 VON 4 - EINGABE-EVENTS",
                 "HOOK 1 SUR 4 - \311V\311NEMENTS D'ENTR\311E"),

    /*The field name keeps a row to itself in every language rather than being
     *broken at an underscore - slides 09 and 10 do the same.*/
    TR("s08_l00", "Assign a function to",
                  "Weisen Sie eine Funktion an",
                  "Affectez une fonction \340"),
    TR("s08_l01", "common.indev_event_cb and",
                  "common.indev_event_cb zu, und",
                  "common.indev_event_cb et le"),
    TR("s08_l02", "the input engine calls it.",
                  "die Eingabe ruft sie auf.",
                  "moteur d'entr\351e l'appelle."),
    TR("s08_l03", "Three types, derived from",
                  "Drei Typen, abgeleitet aus",
                  "Trois types, d\351duits de"),
    TR("s08_l04", "the current and previous",
                  "dem aktuellen und dem",
                  "l'\351tat d'entr\351e courant et"),
    TR("s08_l05", "input state:",
                  "vorherigen Eingabezustand:",
                  "du pr\351c\351dent :"),
    TR("s08_l06", "Pressed   edge, going down",
                  "Pressed   Flanke nach unten",
                  "Pressed   front, \340 l'appui"),
    TR("s08_l07", "Pressing  held, every pass",
                  "Pressing  gehalten, je Durchlauf",
                  "Pressing  maintenu, chaque passe"),
    TR("s08_l08", "Clicked   edge, released",
                  "Clicked   Flanke, losgelassen",
                  "Clicked   front, rel\342ch\351"),

    TR("ev_button_text", "PRESS ME", "DR\334CK MICH", "APPUYEZ-MOI"),
    TR_ALL("ev_counter_pressed", "PRESSED"),
    TR_ALL("ev_counter_pressing", "PRESSING"),
    TR_ALL("ev_counter_clicked", "CLICKED"),
    /*Assembled with snprintf, so this one is a format too.*/
    TR("ev_last_fmt", "LAST: %s", "ZULETZT: %s", "DERNIER : %s"),
    TR("ev_last_none", "NOTHING YET", "NOCH NICHTS", "RIEN ENCORE"),
    TR("ev_note_bind", "COUNTERS ARE BIND_INT LABELS",
                       "Z\304HLER SIND BIND_INT-LABELS",
                       "LES COMPTEURS SONT DES LABELS BIND_INT"),
    TR("ev_note_pressing", "PRESSING FIRES EVERY LS_INDEV_PROCESS",
                           "PRESSING FEUERT BEI JEDEM LS_INDEV_PROCESS",
                           "PRESSING SE D\311CLENCHE \300 CHAQUE LS_INDEV_PROCESS"),

    /* ================= 09 - COMMON.INDEV_CLICK_TEST_CB =================== */

    TR_ALL("title_09", "COMMON.INDEV_CLICK_TEST_CB"),
    TR("sub_09", "HOOK 2 OF 4 - CUSTOM HIT TESTING",
                 "HOOK 2 VON 4 - EIGENER TREFFERTEST",
                 "HOOK 2 SUR 4 - TEST DE CONTACT \300 VOUS"),

    TR("s09_l00", "A widget is only clickable",
                  "Ein Widget ist erst klickbar,",
                  "Un widget n'est cliquable"),
    TR("s09_l01", "once it has a hit test.",
                  "wenn es einen Treffertest hat.",
                  "qu'avec un test de contact."),
    TR("s09_l02", "An arc comes without one,",
                  "Ein Arc bringt keinen mit,",
                  "L'arc n'en fournit aucun ;"),
    TR("s09_l03", "so this demo supplies it.",
                  "diese Demo liefert ihn.",
                  "cette d\351mo en \351crit un."),
    /*The field name is long enough to need a line of its own: broken across two
     *rows it reads as a typo, and every language breaks it in a different place.*/
    TR("s09_l04", "Assign common.indev_click_test_cb",
                  "Setzen Sie common.indev_click_test_cb",
                  "Affectez common.indev_click_test_cb"),
    TR("s09_l05", "and the arc becomes a knob.",
                  "und der Arc wird zum Drehknopf.",
                  "et l'arc devient un bouton rotatif."),
    TR("s09_l06", "The whole hook is integer maths:",
                  "Der ganze Hook ist Ganzzahl-Mathematik:",
                  "Tout le hook est en entiers :"),
    TR("s09_l07", "is the point between the two",
                  "Punkt zwischen den Radien und",
                  "le point est-il entre les deux"),
    TR("s09_l08", "radii and clear of the gap?",
                  "au\337erhalb der L\374cke?",
                  "rayons et hors de l'ouverture ?"),
    TR("s09_l09", "Every widget is asked, if two accept,",
                  "Jedes Widget wird gefragt; nehmen",
                  "Chaque widget est interrog\351 ; si deux"),
    TR("s09_l10", "the one drawn on top gets the event.",
                  "zwei an, erh\344lt das obere das Ereignis.",
                  "acceptent, celui du dessus l'emporte."),

    TR("hit_tap_left", "TAP LEFT SIDE", "LINKS TIPPEN", "TOUCHEZ \300 GAUCHE"),
    TR("hit_tap_right", "TAP RIGHT SIDE", "RECHTS TIPPEN", "TOUCHEZ \300 DROITE"),
    TR_ALL("hit_step_minus", "-5"),
    TR_ALL("hit_step_plus", "+5"),
    TR("hit_note_area", "HIT AREA IS THE RING ONLY, NOT ITS BOX",
                        "TREFFERFL\304CHE IST NUR DER RING, NICHT DIE BOX",
                        "LA ZONE ACTIVE EST L'ANNEAU, PAS SA BO\316TE"),
    TR("hit_note_point", "THE HOOK ALSO CAPTURES WHERE YOU TOUCHED",
                         "DER HOOK MERKT SICH AUCH, WO BER\334HRT WURDE",
                         "LE HOOK RETIENT AUSSI O\331 VOUS AVEZ TOUCH\311"),

    /* ======================== 10 - COMMON.RENDER_CB ======================= */

    TR_ALL("title_10", "COMMON.RENDER_CB"),
    TR("sub_10", "HOOK 3 OF 4 - YOUR OWN PIXEL LOOP",
                 "HOOK 3 VON 4 - EIGENE PIXELSCHLEIFE",
                 "HOOK 3 SUR 4 - VOTRE BOUCLE DE PIXELS"),

    TR("s10_l00", "render_cb is a plain",
                  "render_cb ist ein einfacher",
                  "render_cb est un simple"),
    TR("s10_l01", "function pointer on every",
                  "Funktionszeiger auf jedem",
                  "pointeur de fonction sur"),
    TR("s10_l02", "widget.", "Widget.", "chaque widget."),
    TR("s10_l03", "Create a rectangle, keep",
                  "Rechteck erzeugen, dessen",
                  "Cr\351ez un rectangle, gardez"),
    TR("s10_l04", "its hook, install your",
                  "Hook behalten, den eigenen",
                  "son hook, installez le v\364tre"),
    TR("s10_l05", "own, and chain back for",
                  "einsetzen und f\374r die",
                  "et rappelez l'original pour"),
    TR("s10_l06", "the background fill.",
                  "Grundf\374llung zur\374ckrufen.",
                  "le fond."),
    TR("s10_l07", "The trace is drawn with",
                  "Die Spur entsteht mit",
                  "La trace est dessin\351e avec"),
    /*As on slides 08 and 09, the identifier gets a row of its own instead of
     *being broken at an underscore.*/
    TR_ALL("s10_l08", "ls_render_goto_frame_buffer_px"),
    TR("s10_l09", "- part of the public API.",
                  "- Teil der \366ffentlichen API.",
                  "- une fonction de l'API publique."),

    TR("rcb_switch_caption", "SHOW TRACE", "SPUR ZEIGEN", "AFFICHER LA TRACE"),
    TR("rcb_switch_sub", "HIDDEN, NEVER DELETED",
                         "HIDDEN, NIE GEL\326SCHT",
                         "HIDDEN, JAMAIS SUPPRIM\311"),
    TR("rcb_note", "LS_RECTANGLE_T FIRST IN THE STRUCT, THEN YOUR FIELDS",
                   "LS_RECTANGLE_T ZUERST IM STRUCT, DANN IHRE FELDER",
                   "LS_RECTANGLE_T EN PREMIER DANS LE STRUCT, PUIS VOS CHAMPS"),

    /* ======================== 11 - SCREEN.RENDER_CB ======================= */

    TR_ALL("title_11", "SCREEN.RENDER_CB"),
    TR("sub_11", "HOOK 4 OF 4 - A WHOLE-SCREEN BACKDROP",
                 "HOOK 4 VON 4 - HINTERGRUND F\334R DEN SCHIRM",
                 "HOOK 4 SUR 4 - UN FOND PLEIN \311CRAN"),

    TR("s11_l00", "A screen has a render_cb",
                  "Auch ein Schirm hat ein",
                  "Un \351cran a aussi un"),
    TR("s11_l01", "too. ls_render calls it",
                  "render_cb. ls_render ruft es",
                  "render_cb. ls_render l'appelle"),
    TR("s11_l02", "first, then every visible",
                  "zuerst, dann jedes sichtbare",
                  "d'abord, puis chaque widget"),
    TR("s11_l03", "widget in order.",
                  "Widget der Reihe nach.",
                  "visible dans l'ordre."),
    TR("s11_l04", "ls_screen_init installs a",
                  "ls_screen_init setzt eine",
                  "ls_screen_init installe un"),
    TR("s11_l05", "flat fill. This demo wraps",
                  "flache F\374llung. Diese Demo",
                  "fond plat. Cette d\351mo"),
    TR("s11_l06", "it, so the gradient behind",
                  "umh\374llt sie, der Verlauf",
                  "l'enrobe : le d\351grad\351 derri\350re"),
    TR("s11_l07", "every screen is application",
                  "hinter jedem Schirm ist also",
                  "chaque \351cran est donc du"),
    TR("s11_l08", "code - and the switch hands",
                  "Anwendungscode - und der",
                  "code applicatif - et"),
    TR("s11_l09", "the frame back to the",
                  "Schalter gibt das Bild an",
                  "l'interrupteur rend l'image"),
    TR("s11_l10", "original hook.",
                  "den urspr\374nglichen Hook.",
                  "au hook d'origine."),

    TR("scr_switch_caption", "BG GRADIENT", "BG-VERLAUF", "D\311GRAD\311 DE FOND"),
    TR("scr_switch_sub", "TOGGLE TO SEE THE DEFAULT FLAT FILL",
                         "UMSCHALTEN ZEIGT DIE FLACHE STANDARDF\334LLUNG",
                         "BASCULEZ POUR VOIR LE REMPLISSAGE PAR D\311FAUT"),
    TR("scr_order_caption", "WHAT LS_RENDER DOES, IN ORDER",
                            "WAS LS_RENDER TUT, IN DIESER FOLGE",
                            "CE QUE FAIT LS_RENDER, DANS L'ORDRE"),
    TR("scr_step_screen", "THE SCREEN HOOK", "DER SCHIRM-HOOK", "LE HOOK D'\311CRAN"),
    TR("scr_step_widgets", "WIDGET HOOKS, CREATION ORDER",
                           "WIDGET-HOOKS, ERZEUGUNGSFOLGE",
                           "HOOKS DES WIDGETS, ORDRE DE CR\311ATION"),
    TR("scr_step_hidden", "HIDDEN WIDGETS ARE SKIPPED",
                          "VERSTECKTE WIDGETS FALLEN AUS",
                          "LES WIDGETS CACH\311S SONT IGNOR\311S"),
    TR("scr_step_error", "ANY ERROR ABORTS THE FRAME",
                         "JEDER FEHLER BRICHT DAS BILD AB",
                         "TOUTE ERREUR ANNULE L'IMAGE"),
    TR("scr_note", "INTEGER LERP PER ROW - NO FLOAT ANYWHERE",
                   "GANZZAHL-LERP PRO ZEILE - NIRGENDS FLOAT",
                   "LERP ENTIER PAR LIGNE - AUCUN FLOTTANT"),
};

#define TR_TAG_CNT ((uint32_t)(sizeof(translation_table) / sizeof(translation_table[0])))

/* ------------------------- EXPLANATION BLOCKS ---------------------------- */
/*One label per line, so the line pitch stays a layout decision instead of a
 *font metric. A translation may move words between the lines of a block, but
 *the number of lines is fixed here for every language.*/

static const char * const lines_overview[] = {
    "s01_l00", "s01_l01",
    TR_LINE_SPACER,
    "s01_l02", "s01_l03", "s01_l04", "s01_l05",
    TR_LINE_SPACER,
    "s01_l06", "s01_l07", "s01_l08",
};

static const char * const lines_rectangle[] = {
    "s02_l00", "s02_l01", "s02_l02", "s02_l03",
    TR_LINE_SPACER,
    "s02_l04", "s02_l05"
};

static const char * const lines_label[] = {
    "s03_l00", "s03_l01",
    TR_LINE_SPACER,
    "s03_l02", "s03_l03", "s03_l04",
    TR_LINE_SPACER,
    "s03_l05", "s03_l06", "s03_l07", "s03_l08",
    TR_LINE_SPACER,
    "s03_l09",
};

static const char * const lines_arc[] = {
    "s04_l00", "s04_l01", "s04_l02", "s04_l03", "s04_l04",
    TR_LINE_SPACER,
    "s04_l05", "s04_l06", "s04_l07", "s04_l08"
};

static const char * const lines_button[] = {
    "s05_l00", "s05_l01", "s05_l02",
    TR_LINE_SPACER,
    "s05_l03", "s05_l04", "s05_l05", "s05_l06",
    TR_LINE_SPACER,
    "s05_l07", "s05_l08", "s05_l09",
};

static const char * const lines_image_button[] = {
    "s06_l00", "s06_l01", "s06_l02", "s06_l03",
    TR_LINE_SPACER,
    "s06_l04", "s06_l05", "s06_l06",
    TR_LINE_SPACER,
    "s06_l07","s06_l08",
};

static const char * const lines_image[] = {
    "s07_l00", "s07_l01",
    TR_LINE_SPACER,
    "s07_l02", "s07_l03", "s07_l04", "s07_l05",
    TR_LINE_SPACER,
    "s07_l06", "s07_l07", "s07_l08", "s07_l09",
};

static const char * const lines_hook_events[] = {
    "s08_l00", "s08_l01", "s08_l02",
    TR_LINE_SPACER,
    "s08_l03", "s08_l04", "s08_l05",
    TR_LINE_SPACER,
    "s08_l06", "s08_l07", "s08_l08",
};

static const char * const lines_hook_click_test[] = {
    "s09_l00", "s09_l01", "s09_l02", "s09_l03",
    TR_LINE_SPACER,
    "s09_l04", "s09_l05", "s09_l06", "s09_l07", "s09_l08",
    TR_LINE_SPACER,
    "s09_l09", "s09_l10",
};

static const char * const lines_hook_render[] = {
    "s10_l00", "s10_l01", "s10_l02",
    TR_LINE_SPACER,
    "s10_l03", "s10_l04", "s10_l05", "s10_l06",
    TR_LINE_SPACER,
    "s10_l07", "s10_l08", "s10_l09",
};

static const char * const lines_hook_screen_render[] = {
    "s11_l00", "s11_l01", "s11_l02", "s11_l03",
    TR_LINE_SPACER,
    "s11_l04", "s11_l05", "s11_l06", "s11_l07", "s11_l08", "s11_l09", "s11_l10",
};

#define LINES(a) (a), (uint32_t)(sizeof(a) / sizeof((a)[0]))

/**********************
 *   GLOBAL VARIABLES
 **********************/

const tr_slide_t tr_slide[TR_SLIDE_CNT] = {
    { "title_01", "sub_01", LINES(lines_overview) },
    { "title_02", "sub_02", LINES(lines_rectangle) },
    { "title_03", "sub_03", LINES(lines_label) },
    { "title_04", "sub_04", LINES(lines_arc) },
    { "title_05", "sub_05", LINES(lines_button) },
    { "title_06", "sub_06", LINES(lines_image_button) },
    { "title_07", "sub_07", LINES(lines_image) },
    { "title_08", "sub_08", LINES(lines_hook_events) },
    { "title_09", "sub_09", LINES(lines_hook_click_test) },
    { "title_10", "sub_10", LINES(lines_hook_render) },
    { "title_11", "sub_11", LINES(lines_hook_screen_render) },
};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

int tr_init(uint32_t language_index)
{
    ls_error_code_t err = ls_translation_init(translation_table, TR_TAG_CNT, TR_LANGUAGE_CNT);
    if(err != LS_ERROR_CODE_OK) { printf("ls_translation_init failed: %d\n", err); return 1; }

    return tr_set_language(language_index);
}

int tr_set_language(uint32_t language_index)
{
    if(language_index >= TR_LANGUAGE_CNT) { printf("no such language: %u\n", language_index); return 1; }

    ls_error_code_t err = ls_translation_set_language(language_index);
    if(err != LS_ERROR_CODE_OK) { printf("ls_translation_set_language failed: %d\n", err); return 1; }

    return 0;
}

int tr_bind(ls_label_t * label, const char * tag)
{
    if(label == NULL || tag == NULL) { printf("tr_bind called with NULL\n"); return 1; }

    ls_error_code_t err = ls_translation_get(tag, &label->translation);
    if(err != LS_ERROR_CODE_OK) { printf("translation '%s' not found: %d\n", tag, err); return 1; }

    return 0;
}

const char * tr_current(const char * tag)
{
    const ls_translation_t * entry;

    ls_error_code_t err = ls_translation_get(tag, &entry);
    if(err != LS_ERROR_CODE_OK) {
        printf("translation '%s' not found: %d\n", tag, err);
        return tag;   /*Visible on screen, so the missing tag names itself*/
    }

    return entry->current;
}
