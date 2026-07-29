/*
 * Copyright (c) 2026 stroma
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

#define ZMK_CANVAS_DEMO_SIZE 48

struct zmk_widget_canvas_demo {
    lv_obj_t *obj;
    lv_color_t cbuf[ZMK_CANVAS_DEMO_SIZE * ZMK_CANVAS_DEMO_SIZE];
};

int zmk_widget_canvas_demo_init(struct zmk_widget_canvas_demo *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_canvas_demo_obj(struct zmk_widget_canvas_demo *widget);
