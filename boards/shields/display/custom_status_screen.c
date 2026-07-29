/*
 * Copyright (c) 2026 stroma
 * SPDX-License-Identifier: MIT
 *
 * Minimal custom status screen: black background, three R/G/B bars to
 * verify color order / byte swap on the panel, and one event-driven
 * custom widget (active layer label).
 */

#include <zmk/display/status_screen.h>
#include "widgets/layer_label.h"
#include "widgets/canvas_demo.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static struct zmk_widget_layer_label layer_label_widget;
static struct zmk_widget_canvas_demo canvas_demo_widget;

LV_IMG_DECLARE(test_image);

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen = lv_obj_create(NULL);

    // reflow the test layout for narrow (portrait) panels
    const bool narrow = lv_disp_get_hor_res(NULL) < 120;

    lv_obj_set_style_bg_color(screen, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);

    // expected order: pure red, pure green, pure blue
    const uint32_t bar_hex[3] = {0xFF0000, 0x00FF00, 0x0000FF};
    for (int i = 0; i < 3; i++) {
        lv_obj_t *bar = lv_obj_create(screen);
        lv_obj_set_size(bar, 40, 14);
        if (narrow) {
            lv_obj_align(bar, LV_ALIGN_TOP_LEFT, 8, 8 + i * 18);
        } else {
            lv_obj_align(bar, LV_ALIGN_TOP_LEFT, 8 + i * 48, 8);
        }
        lv_obj_set_style_bg_color(bar, lv_color_hex(bar_hex[i]), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_border_width(bar, 0, LV_PART_MAIN);
        lv_obj_set_style_radius(bar, 0, LV_PART_MAIN);
    }

    // image from a C pixel array; quadrants must read TL red, TR green,
    // BL blue, BR yellow
    lv_obj_t *img = lv_img_create(screen);
    lv_img_set_src(img, &test_image);
    if (narrow) {
        lv_obj_align(img, LV_ALIGN_TOP_MID, 0, 114);
    } else {
        lv_obj_align(img, LV_ALIGN_BOTTOM_LEFT, 8, -8);
    }

    zmk_widget_canvas_demo_init(&canvas_demo_widget, screen);
    if (narrow) {
        lv_obj_align(zmk_widget_canvas_demo_obj(&canvas_demo_widget), LV_ALIGN_TOP_MID, 0, 62);
    } else {
        lv_obj_align(zmk_widget_canvas_demo_obj(&canvas_demo_widget), LV_ALIGN_TOP_RIGHT, -4, 4);
    }

    zmk_widget_layer_label_init(&layer_label_widget, screen);
    lv_obj_t *layer_obj = zmk_widget_layer_label_obj(&layer_label_widget);
    lv_obj_set_style_text_font(layer_obj, narrow ? &lv_font_montserrat_12 : &lv_font_montserrat_26,
                               LV_PART_MAIN);
    lv_obj_set_style_text_color(layer_obj, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(layer_obj, LV_ALIGN_BOTTOM_MID, 0, narrow ? 0 : -6);

    return screen;
}
