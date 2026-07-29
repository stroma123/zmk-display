/*
 * Copyright (c) 2026 stroma
 * SPDX-License-Identifier: MIT
 *
 * Status screen composed of the ported mlego widgets: battery gauge,
 * output status, WPM sparkline and active layer. Layout adapts to the
 * panel geometry at runtime. The LVGL bring-up test widgets
 * (canvas_demo, test_image) remain in widgets/ for diagnostics but are
 * no longer compiled in.
 */

#include <zmk/display/status_screen.h>
#include "widgets/layer_label.h"
#include "widgets/output_status.h"
#include "widgets/util.h"
#if IS_ENABLED(CONFIG_ZMK_BATTERY_REPORTING)
#include "widgets/battery_status.h"
#endif
#if IS_ENABLED(CONFIG_ZMK_WPM)
#include "widgets/wpm_status.h"
#endif

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static struct zmk_widget_layer_label layer_label_widget;
static struct zmk_widget_output_status output_status_widget;
#if IS_ENABLED(CONFIG_ZMK_BATTERY_REPORTING)
static struct zmk_widget_battery_status battery_status_widget;
#endif
#if IS_ENABLED(CONFIG_ZMK_WPM)
static struct zmk_widget_wpm_status wpm_status_widget;
#endif

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen = lv_obj_create(NULL);

    // portrait LCD and 128x64 OLED get their own layouts
    const bool narrow = lv_disp_get_hor_res(NULL) < 120;
    const bool compact = !narrow && lv_disp_get_ver_res(NULL) < 70;

    lv_obj_set_style_bg_color(screen, LVGL_BACKGROUND, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);

#if IS_ENABLED(CONFIG_ZMK_BATTERY_REPORTING)
    zmk_widget_battery_status_init(&battery_status_widget, screen);
    lv_obj_t *battery_obj = zmk_widget_battery_status_obj(&battery_status_widget);
    if (narrow) {
        lv_obj_align(battery_obj, LV_ALIGN_TOP_MID, 0, 4);
    } else {
        lv_obj_align(battery_obj, LV_ALIGN_TOP_LEFT, 4, compact ? 2 : 4);
    }
#endif

    zmk_widget_output_status_init(&output_status_widget, screen);
    lv_obj_t *output_obj = zmk_widget_output_status_obj(&output_status_widget);
    lv_obj_set_style_text_font(output_obj, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_text_color(output_obj, LVGL_FOREGROUND, LV_PART_MAIN);
    if (narrow) {
        lv_obj_align(output_obj, LV_ALIGN_TOP_MID, 0, 26);
    } else {
        lv_obj_align(output_obj, LV_ALIGN_TOP_RIGHT, -6, compact ? 2 : 4);
    }

#if IS_ENABLED(CONFIG_ZMK_WPM)
    zmk_widget_wpm_status_init(&wpm_status_widget, screen);
    lv_obj_t *wpm_obj = zmk_widget_wpm_status_obj(&wpm_status_widget);
    if (narrow) {
        lv_obj_align(wpm_obj, LV_ALIGN_TOP_MID, 0, 46);
    } else {
        lv_obj_align(wpm_obj, LV_ALIGN_BOTTOM_RIGHT, compact ? -2 : -4, compact ? -2 : -4);
    }
#endif

    zmk_widget_layer_label_init(&layer_label_widget, screen);
    lv_obj_t *layer_obj = zmk_widget_layer_label_obj(&layer_label_widget);
    lv_obj_set_style_text_font(layer_obj, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_text_color(layer_obj, LVGL_FOREGROUND, LV_PART_MAIN);
    if (narrow) {
        lv_obj_align(layer_obj, LV_ALIGN_BOTTOM_MID, 0, -4);
    } else {
        lv_obj_align(layer_obj, LV_ALIGN_BOTTOM_LEFT, compact ? 2 : 4, compact ? -2 : -4);
    }

    return screen;
}
