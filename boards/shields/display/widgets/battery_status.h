/*
 * Copyright (c) 2026 stroma
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

#define BATTERY_CANVAS_W 68
#define BATTERY_CANVAS_H 18

struct zmk_widget_battery_status {
    sys_snode_t node;
    lv_obj_t *obj;
    lv_color_t cbuf[BATTERY_CANVAS_W * BATTERY_CANVAS_H];
    uint8_t level;
    bool charging;
};

int zmk_widget_battery_status_init(struct zmk_widget_battery_status *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_battery_status_obj(struct zmk_widget_battery_status *widget);
