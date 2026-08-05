# Examples Guide

A short getting-started guide for the two example applications shipped with the LVGL Safe
preview. Both run on your desktop in an SDL2 window, so you can evaluate the library
before any target hardware is involved.

## 1. What you need

| | |
|---|---|
| **CMake** | 3.16 or newer |
| **A C99 compiler (or newer)** | GCC or Clang |
| **SDL2 development files** | the simulator backend the examples draw into |
| **The LVGL Safe package** | already unpacked in the repository root ([../include/](../include/) and [../lib/](../lib/)) |

Installing the needed tools and dependencies:

```bash
sudo apt install cmake build-essential libsdl2-dev   # Debian / Ubuntu
brew install cmake sdl2                              # macOS
```

Python is **not** needed to build the examples. It is only needed if you want to convert
your own images and fonts — see [section 4](#4-bringing-your-own-images-and-fonts).

## 2. Build and run

From the repository root:

```bash
cmake -S . -B build
cmake --build build -j
```

That configures both examples and produces two binaries:

```bash
./build/examples/basic_example/basic_example
./build/examples/lvgl_safe_api_tour/lvgl_safe_api_tour
```

The package is already unpacked here, so nothing is downloaded or extracted at configure
time. The top-level [CMakeLists.txt](../CMakeLists.txt) finds the unpacked package in the
repository root and imports the library through the CMake config package it ships
(`lib/cmake/lvgl_safe/`). An unpacked `lvgl_safe-x.y.z-<platform>/` subdirectory is picked
up as well. To use a copy kept somewhere else, point at the directory that contains
`include/` and `lib/`:

```bash
cmake -S . -B build -DLS_PACKAGE_PREFIX=/path/to/lvgl_safe-0.1.0-linux-x86_64
```

A successful configure prints the version and location it resolved:

```
-- Found LVGL Safe 0.1.0 in /path/to/lvgl_safe
-- SDL2 target for examples: SDL2::SDL2
```

Linking your own application against the library is the same two lines the examples use
(see [basic_example/CMakeLists.txt](basic_example/CMakeLists.txt)):

```cmake
add_executable(my_app main.c fonts/my_font.c images/my_image.c)
target_link_libraries(my_app PRIVATE lvgl_safe::lvgl_safe ${LS_SDL2_LIBRARY})
```

> **Note on `ls_conf.h`:** the shipped [../include/ls_conf.h](../include/ls_conf.h) is
> authoritative. Its values are already baked into the struct layouts in
> `liblvgl_safe.a`, so it must not be edited — every feature of the preview library is
> enabled as shipped.

## 3. The two examples

We suggest you to try out the examples in the following order.

### `lvgl_safe_api_tour` - what the library can do

A deck of eleven screens, one topic per screen, at 1024x600. Click the arrows in the
footer to scroll through it. The deck wraps around in both directions. The UP/DOWN keys
move the keyboard focus; most screens have only the footer's language button to focus, but
the `ls_button` and event-callback screens have several.

It covers every widget type in v0.1.0 - rectangle, label, arc, button, image button,
image - and all four extension points: the input event callback, custom hit testing, a
per-widget render hook, and a whole-screen render hook. Nothing is hidden behind a
helper function, so each screen doubles as reference code for the topic it names.

The whole deck is translated. The button in the footer cycles English, German and French,
and every string on screen follows it, because the labels are bound to an `ls_translation_t`
rather than to a literal. The text itself lives in
[lvgl_safe_api_tour/translations.c](lvgl_safe_api_tour/translations.c) - one row per string,
three languages per row - so `main.c` holds the layout and the logic and none of the
wording. The only strings it builds itself are the ones that are not language at all, like
the `01 / 11` slide counter.

Start here to see the capabilities. → [lvgl_safe_api_tour/main.c](lvgl_safe_api_tour/main.c)

### `basic_example` - how an application is put together

One display, two screens, one widget of each basic kind, and a few callbacks, at 800x480.
Every element carries a dimmed tag naming its widget type, so what you see on screen maps
directly onto the code.

STEP +10 and RESET drive a single application variable; the readout and the arc both
follow it, and the switch shows and hides the arc. The arrow icons move between the two
screens - the second screen reads the same variable, which shows that application state
is not owned by a screen.

This file is the one to copy from when you are getting started with your own application.
It is laid out in a way that shows the general shape every LVGL Safe program - give the 
display a frame buffer you own, init a screen, create widgets onto it, configure them by 
writing struct fields, then loop over input, render, and flush.

Two properties worth noticing as you read: every widget is a caller-owned `static` struct
created once at start-up (nothing is allocated at run-time and nothing is ever
destroyed), and every fallible call returns an `ls_error_code_t` that is checked at the
call site.

See → [basic_example/main.c](basic_example/main.c)

## 4. Using your own images and fonts

LVGL Safe installs no default assets. Every image and font a widget uses is compiled into
the binary and declared explicitly. The two converters in [../scripts/](../scripts/) turn
ordinary asset files into the C sources you add to your build.

Their dependencies:

```bash
pip install pillow freetype-py   # cairosvg as well, only if you convert SVG input
```

### Images — `image_conv.py`

```
image_conv.py <image> <argb8888|a8|rgb565> <stride_align> <name> <c_output|file_output>
```

| Argument | Meaning |
|---|---|
| `image` | input file: `.png`, `.jpg`, `.jpeg`, `.bmp` or `.svg` |
| color format | `argb8888` for full colour with alpha, `a8` for an alpha-only mask that the widget tints, `rgb565` for opaque 16-bit |
| `stride_align` | row alignment in bytes; `4` is a safe default, match it to what your target's blitter wants |
| `name` | the C symbol name — this is what you `extern` in your application |
| mode | `c_output` writes the C source to **stdout**, so redirect it to a file |

```bash
python3 scripts/image_conv.py my_icon.png argb8888 4 my_icon c_output > images/my_icon.c
```

Then declare and use it:

```c
extern const ls_image_dsc_t my_icon;   /* the name you passed as argument 4 */
```

Add `images/my_icon.c` to your `add_executable(...)` list and it is linked in.

### Fonts — `font_conv.py`

```
font_conv.py <font.ttf> <first> <last> <render_px> <name> <c_output|file_output>
```

| Argument | Meaning |
|---|---|
| `font.ttf` | any TrueType font FreeType can read |
| `first`, `last` | the inclusive Unicode codepoint range to include — `32 127` is printable ASCII |
| `render_px` | pixel size to rasterise at; one size per generated font, so a 12 px and an 18 px face are two separate files |
| `name` | the C symbol name |
| mode | `c_output` writes to **stdout** |

```bash
python3 scripts/font_conv.py Montserrat-Medium.ttf 32 127 18 montserrat_18 c_output > fonts/montserrat_18.c
```

```c
extern ls_font_t montserrat_18;   /* note: not const */
```

Keep the range as narrow as your UI actually needs — it is the main driver of the
generated font's size in flash. If a string uses a codepoint outside the range you
converted, that glyph is not in the binary.

### `file_output` mode

Both scripts also accept `file_output` instead of `c_output`. Rather than emitting C, this
writes the raw asset as small binary blobs into the **current working directory**, for
setups that load assets at run time instead of linking them in:

* `image_conv.py` → `<image>.pixels` and `<image>.meta`
* `font_conv.py` → `<font>.<px>.bitmap`, `<font>.<px>.glyphs` and `<font>.<px>.meta`

Both examples use `c_output`; start there unless you specifically need run-time loading.

## 5. If something goes wrong

| Symptom | Cause |
|---|---|
| `No unpacked LVGL Safe package found in ...` | `include/` and `lib/` are missing from the repository root — extract the package there, or pass `-DLS_PACKAGE_PREFIX=<path>` |
| `... does not look like an unpacked LVGL Safe package` | the path given to `-DLS_PACKAGE_PREFIX` is not the directory holding `include/` and `lib/` (`lib/cmake/lvgl_safe/lvgl_safeConfig.cmake` was not found there) |
| SDL2 not found at configure time | install the SDL2 **development** package, not just the runtime |
| A widget draws nothing | check the return code of its `*_create` call, and whether `common.hidden` is set |
| An image button errors at render | the source for the state it is in is `NULL`; every state a widget can reach needs a source |
