/*
 * Copyright (c) 2023 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 *
 * WPM sparkline ported from the mlego status widget's draw_top: a
 * bordered graph of the last 10 samples plus the current value.
 * (The original drew into a nonexistent struct member on WPM updates;
 * here each widget owns its canvas buffer and sample ring.)
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/wpm_state_changed.h>
#include <zmk/wpm.h>

#include "wpm_status.h"
#include "util.h"

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct wpm_status_state {
    uint8_t wpm;
};

static void draw(struct zmk_widget_wpm_status *widget) {
    lv_obj_t *canvas = widget->obj;

    lv_draw_rect_dsc_t rect_black_dsc;
    init_rect_dsc(&rect_black_dsc, LVGL_BACKGROUND);
    lv_draw_rect_dsc_t rect_white_dsc;
    init_rect_dsc(&rect_white_dsc, LVGL_FOREGROUND);
    lv_draw_line_dsc_t line_dsc;
    init_line_dsc(&line_dsc, LVGL_FOREGROUND, 2);
    lv_draw_label_dsc_t label_dsc;
    init_label_dsc(&label_dsc, LVGL_FOREGROUND, &lv_font_unscii_8, LV_TEXT_ALIGN_RIGHT);

    // border
    lv_canvas_draw_rect(canvas, 0, 0, WPM_CANVAS_W, WPM_CANVAS_H, &rect_white_dsc);
    lv_canvas_draw_rect(canvas, 1, 1, WPM_CANVAS_W - 2, WPM_CANVAS_H - 2, &rect_black_dsc);

    // current value, top right
    char text[6] = {};
    snprintf(text, sizeof(text), "%d", widget->wpm[WPM_SAMPLES - 1]);
    lv_canvas_draw_text(canvas, WPM_CANVAS_W - 29, 3, 26, &label_dsc, text);

    // sparkline over the sample ring
    int max = 0;
    int min = 256;
    for (int i = 0; i < WPM_SAMPLES; i++) {
        if (widget->wpm[i] > max) {
            max = widget->wpm[i];
        }
        if (widget->wpm[i] < min) {
            min = widget->wpm[i];
        }
    }
    int range = max - min;
    if (range == 0) {
        range = 1;
    }

    lv_point_t points[WPM_SAMPLES];
    for (int i = 0; i < WPM_SAMPLES; i++) {
        points[i].x = 2 + i * ((WPM_CANVAS_W - 4) / (WPM_SAMPLES - 1));
        points[i].y = (WPM_CANVAS_H - 4) -
                      (widget->wpm[i] - min) * (WPM_CANVAS_H - 8) / range;
    }
    lv_canvas_draw_line(canvas, points, WPM_SAMPLES, &line_dsc);
}

static void set_wpm_status(struct zmk_widget_wpm_status *widget, struct wpm_status_state state) {
    for (int i = 0; i < WPM_SAMPLES - 1; i++) {
        widget->wpm[i] = widget->wpm[i + 1];
    }
    widget->wpm[WPM_SAMPLES - 1] = state.wpm;

    draw(widget);
}

static void wpm_status_update_cb(struct wpm_status_state state) {
    struct zmk_widget_wpm_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_wpm_status(widget, state); }
}

static struct wpm_status_state wpm_status_get_state(const zmk_event_t *eh) {
    return (struct wpm_status_state){.wpm = zmk_wpm_get_state()};
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_wpm_status, struct wpm_status_state, wpm_status_update_cb,
                            wpm_status_get_state)
ZMK_SUBSCRIPTION(widget_wpm_status, zmk_wpm_state_changed);

int zmk_widget_wpm_status_init(struct zmk_widget_wpm_status *widget, lv_obj_t *parent) {
    widget->obj = lv_canvas_create(parent);
    lv_canvas_set_buffer(widget->obj, widget->cbuf, WPM_CANVAS_W, WPM_CANVAS_H,
                         LV_IMG_CF_TRUE_COLOR);
    memset(widget->wpm, 0, sizeof(widget->wpm));
    draw(widget);

    sys_slist_append(&widgets, &widget->node);

    widget_wpm_status_init();
    return 0;
}

lv_obj_t *zmk_widget_wpm_status_obj(struct zmk_widget_wpm_status *widget) { return widget->obj; }
