add_executable(lvgl_safe_api_tour ./main.c
                             ./translations.c
                             ./fonts/montserrat_12.c
                             ./fonts/montserrat_14.c
                             ./fonts/montserrat_18.c
                             ./fonts/montserrat_28.c
                             ./images/img_benchmark_lvgl_logo_argb.c
                             ./images/lvgl_logo_dark.c
                             ./images/fault_triangle_a8.c
                             ./images/switch_on.c
                             ./images/switch_off.c
                             ./images/nav_prev_normal.c
                             ./images/nav_prev_pressed.c
                             ./images/nav_next_normal.c
                             ./images/nav_next_pressed.c)

target_link_libraries(lvgl_safe_api_tour PRIVATE lvgl_safe::lvgl_safe ${LS_SDL2_LIBRARY})
