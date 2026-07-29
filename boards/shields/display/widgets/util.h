/*
 * Copyright (c) 2023 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 *
 * Draw helpers ported from the mlego/nice!view widgets (see the
 * archive/2026-07-29-custom-status-wip branch for the originals).
 * Software canvas rotation was dropped - panel orientation is handled
 * in hardware via madctl.
 */

#pragma once

#include <lvgl.h>

// default is light-on-dark to match the custom status screen background
#define LVGL_BACKGROUND                                                                            \
    (IS_ENABLED(CONFIG_DISPLAY_WIDGET_INVERTED) ? lv_color_white() : lv_color_black())
#define LVGL_FOREGROUND                                                                            \
    (IS_ENABLED(CONFIG_DISPLAY_WIDGET_INVERTED) ? lv_color_black() : lv_color_white())

void draw_battery(lv_obj_t *canvas, uint8_t level, bool charging);
void init_label_dsc(lv_draw_label_dsc_t *label_dsc, lv_color_t color, const lv_font_t *font,
                    lv_text_align_t align);
void init_rect_dsc(lv_draw_rect_dsc_t *rect_dsc, lv_color_t bg_color);
void init_line_dsc(lv_draw_line_dsc_t *line_dsc, lv_color_t color, uint8_t width);
void init_arc_dsc(lv_draw_arc_dsc_t *arc_dsc, lv_color_t color, uint8_t width);
