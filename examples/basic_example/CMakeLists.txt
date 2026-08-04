add_executable(basic_example ./main.c
                             ./fonts/font_dejavu_30.c
                             ./fonts/montserrat_12.c
                             ./fonts/montserrat_18.c
                             ./images/img_benchmark_lvgl_logo_argb.c
                             ./images/switch_on.c
                             ./images/switch_off.c
                             ./images/nav_next_normal.c
                             ./images/nav_next_pressed.c
                             ./images/nav_prev_normal.c
                             ./images/nav_prev_pressed.c)

target_link_libraries(basic_example PRIVATE lvgl_safe::lvgl_safe ${LS_SDL2_LIBRARY})
