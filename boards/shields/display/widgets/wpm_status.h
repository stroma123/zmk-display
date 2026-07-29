/*
 * Copyright (c) 2026 stroma
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

#define WPM_CANVAS_W 68
#define WPM_CANVAS_H 44
#define WPM_SAMPLES 10

struct zmk_widget_wpm_status {
    sys_snode_t node;
    lv_obj_t *obj;
    lv_color_t cbuf[WPM_CANVAS_W * WPM_CANVAS_H];
    uint8_t wpm[WPM_SAMPLES];
};

int zmk_widget_wpm_status_init(struct zmk_widget_wpm_status *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_wpm_status_obj(struct zmk_widget_wpm_status *widget);
